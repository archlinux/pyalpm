/**
 * db.c : wrapper class around alpm_db_t
 *
 *  Copyright (c) 2011 Rémy Oudompheng <remy@archlinux.org>
 *
 *  This file is part of pyalpm.
 *
 *  pyalpm is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  pyalpm is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with pyalpm.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <pyconfig.h>
#include <alpm.h>
#include <Python.h>
#include "db.h"
#include "package.h"
#include "util.h"

typedef struct _AlpmDB {
  PyObject_HEAD
  alpm_db_t *c_data;
} AlpmDB;

#define ALPM_DB(self) (((AlpmDB*)self)->c_data)

static PyTypeObject AlpmDBType;

static void pyalpm_db_dealloc(AlpmDB *self) {
  Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject* _pyobject_from_pmgrp(void *group) {
  const alpm_group_t* grp = (alpm_group_t*)group;
  if (!grp)
    Py_RETURN_NONE;
  else {
    PyObject *fst = PyUnicode_FromString(grp->name);
    PyObject *snd = alpmlist_to_pylist(grp->packages,
              pyalpm_package_from_pmpkg);
    PyObject *tuple = PyTuple_Pack(2, fst, snd);
    Py_DECREF(fst);
    Py_DECREF(snd);
    return tuple;
  }
}

/** Converts a Python list of databases to an alpm_list_t linked list.
 * return 0 on success, -1 on failure
 */
int pylist_db_to_alpmlist(PyObject *list, alpm_list_t **result) {
  alpm_list_t *ret = NULL;
  PyObject *iterator = PyObject_GetIter(list);
  PyObject *item;

  if(iterator == NULL) {
    PyErr_SetString(PyExc_TypeError, "object is not iterable");
    return -1;
  }

  while((item = PyIter_Next(iterator)))
  {
    if (PyObject_TypeCheck(item, &AlpmDBType)) {
      ret = alpm_list_add(ret, ((AlpmDB*)item)->c_data);
    } else {
      PyErr_SetString(PyExc_TypeError, "list must contain only Database objects");
      FREELIST(ret);
      Py_DECREF(item);
      Py_DECREF(iterator);
      return -1;
    }
    Py_DECREF(item);
  }
  Py_DECREF(iterator);

  *result = ret;
  return 0;
}

#define CHECK_IF_INITIALIZED() if (! self->c_data) { \
  PyErr_SetString(alpm_error, "data is not initialized"); \
  return NULL; \
  }

/** Database properties */

static PyObject* pyalpm_db_get_name(AlpmDB* self, void* closure) {
  const char* name;
  CHECK_IF_INITIALIZED();
  name = alpm_db_get_name(self->c_data);
  if (!name)
    Py_RETURN_NONE;
  return PyUnicode_FromString(name);
}

static PyObject* pyalpm_db_get_servers(PyObject *self, void* closure) {
  alpm_db_t *db = ALPM_DB(self);
  return alpmlist_to_pylist(alpm_db_get_servers(db), pyobject_from_string);
}

static int pyalpm_db_set_servers(PyObject* self, PyObject* value, void* closure) {
  alpm_db_t *db = ALPM_DB(self);
  alpm_list_t *target;
  if (pylist_string_to_alpmlist(value, &target) == -1)
    return -1;
  if (alpm_db_set_servers(db, target) == -1)
    RET_ERR("unable to set servers", 0, -1);
  return 0;
}

static PyObject* pyalpm_db_get_pkgcache(AlpmDB* self, void* closure) {
  alpm_list_t *pkglist = alpm_db_get_pkgcache(self->c_data);
  return alpmlist_to_pylist(pkglist, pyalpm_package_from_pmpkg);
}

static PyObject* pyalpm_db_get_grpcache(AlpmDB* self, void* closure) {
  alpm_list_t *grplist = alpm_db_get_groupcache(self->c_data);
  return alpmlist_to_pylist(grplist, _pyobject_from_pmgrp);
}

/** Package get/set operations */

static PyObject* pyalpm_db_get_pkg(PyObject *rawself, PyObject* args) {
  char *pkgname;
  alpm_pkg_t *p;
  AlpmDB *self = (AlpmDB*)rawself;

  if(!PyArg_ParseTuple(args, "s", &pkgname))
  {
    PyErr_SetString(PyExc_TypeError, "get_pkg() takes a string argument");
    return NULL;
  }

  CHECK_IF_INITIALIZED();

  p = alpm_db_get_pkg(self->c_data, pkgname);

  if (p == NULL) {
    Py_RETURN_NONE;
  } else {
    PyObject *result;
    result = pyalpm_package_from_pmpkg(p);
    if (result == NULL) {
      return NULL;
    } else {
      return result;
    }
  }
}

static PyObject* pyalpm_db_get_group(PyObject* rawself, PyObject* args) {
  AlpmDB* self = (AlpmDB*)rawself;
  char *grpname;
  alpm_group_t *grp;
  if (!PyArg_ParseTuple(args, "s", &grpname)) {
    PyErr_SetString(PyExc_TypeError, "expected string argument");
    return NULL;
  }

  grp = alpm_db_get_group(self->c_data, grpname);
  return _pyobject_from_pmgrp(grp);
}

static PyObject *pyalpm_db_update(PyObject *rawself, PyObject *args, PyObject *kwargs) {
  AlpmDB* self = (AlpmDB*)rawself;
  char* keyword[] = {"force", NULL};
  int ret;
  PyObject *force;
  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O!", keyword, &PyBool_Type, &force))
    return NULL;

  ret = alpm_db_update((force == Py_True), self->c_data);

  switch(ret) {
  case -1:
    RET_ERR("unable to update database", 0, NULL);
  case 0:
    Py_RETURN_TRUE;
  case 1:
    Py_RETURN_FALSE;
  default:
    RET_ERR("invalid return code from alpm_db_update()", 0, NULL);
  }
}

static PyObject* pyalpm_db_search(PyObject *rawself, PyObject *args) {
  AlpmDB* self = (AlpmDB *)rawself;
  alpm_list_t* rawargs;
  alpm_list_t* result;
  int ok = pylist_string_to_alpmlist(args, &rawargs);
  if (ok == -1) return NULL;

  result = alpm_db_search(self->c_data, rawargs);
  return alpmlist_to_pylist(result, pyalpm_package_from_pmpkg);
}

static struct PyMethodDef db_methods[] = {
  { "get_pkg", pyalpm_db_get_pkg, METH_VARARGS,
    "get a package by name\n"
    "args: a package name (string)\n"
    "returns: a Package object or None if not found" },
  { "search", pyalpm_db_search, METH_VARARGS,
    "search for packages matching a list of regexps\n"
    "args: a variable number of regexps (strings)\n"
    "returns: packages matching all these regexps" },
  { "read_grp", pyalpm_db_get_group, METH_VARARGS,
    "get contents of a group\n"
    "args: a group name (string)\n"
    "returns: a tuple (group name, list of packages)" },
  { "update", (PyCFunction)pyalpm_db_update, METH_VARARGS | METH_KEYWORDS,
    "update a database from its url attribute\n"
    "args: force (update even if DB is up to date, boolean)\n"
    "returns: True if an update has been done" },
  { NULL },
};

struct PyGetSetDef db_getset[] = {
  /* description properties */
  { "name", (getter)pyalpm_db_get_name, 0,
    "database name (e.g. \"core\", \"extra\")", NULL } ,
  { "servers", (getter)pyalpm_db_get_servers, (setter)pyalpm_db_set_servers,
    "a list of URLs (for sync DBs)", NULL } ,
  { "pkgcache", (getter)pyalpm_db_get_pkgcache, 0, "(read only) list of packages", NULL } ,
  { "grpcache", (getter)pyalpm_db_get_grpcache, 0, "(read only) list of package groups", NULL } ,
  { NULL }
};

static PyTypeObject AlpmDBType = {
  PyVarObject_HEAD_INIT(NULL, 0)
  "alpm.DB",             /*tp_name*/
  sizeof(AlpmDB),        /*tp_basicsize*/
  0,                          /*tp_itemsize*/
  .tp_dealloc = (destructor)pyalpm_db_dealloc,
  .tp_flags = Py_TPFLAGS_DEFAULT,
  .tp_doc = "libalpm DB object",
  .tp_methods = db_methods,
  .tp_getset = db_getset,
};

/** Initializes Pb class in module */
void init_pyalpm_db(PyObject *module) {
  PyObject *type;

  if (PyType_Ready(&AlpmDBType) < 0)
    return;
  type = (PyObject*)&AlpmDBType;
  Py_INCREF(type);
  PyModule_AddObject(module, "DB", type);

  /* signature check levels */
  PyModule_AddIntConstant(module, "SIG_DATABASE", ALPM_SIG_DATABASE);
  PyModule_AddIntConstant(module, "SIG_DATABASE_OPTIONAL", ALPM_SIG_DATABASE_OPTIONAL);
  PyModule_AddIntConstant(module, "SIG_DATABASE_MARGINAL_OK", ALPM_SIG_DATABASE_MARGINAL_OK);
  PyModule_AddIntConstant(module, "SIG_DATABASE_UNKNOWN_OK", ALPM_SIG_DATABASE_UNKNOWN_OK);
}


PyObject *pyalpm_db_from_pmdb(void* data) {
  alpm_db_t *db = (alpm_db_t*)data;
  AlpmDB *self;
  self = (AlpmDB*)AlpmDBType.tp_alloc(&AlpmDBType, 0);
  if (self == NULL) {
    PyErr_SetString(PyExc_RuntimeError, "unable to create DB object");
    return NULL;
  }

  self->c_data = db;
  return (PyObject *)self;
}

/** non-class methods */
PyObject* pyalpm_find_grp_pkgs(PyObject* self, PyObject *args) {
  PyObject *dbs;
  char *grpname;
  alpm_list_t *db_list = NULL;
  alpm_list_t *pkg_list;
  PyObject *result;

  if (!PyArg_ParseTuple(args, "Os", &dbs, &grpname)) {
    PyErr_SetString(PyExc_TypeError, "expected arguments (list of dbs, group name)");
    return NULL;
  }

  {
    int ret = pylist_db_to_alpmlist(dbs, &db_list);
    if (ret == -1)
      return NULL;
  }
  pkg_list = alpm_find_group_pkgs(db_list, grpname);
  result = alpmlist_to_pylist(pkg_list, pyalpm_package_from_pmpkg);
  alpm_list_free(db_list);
  alpm_list_free(pkg_list);
  return result;
}

/** Finds an available upgrade for a package in a list of databases */
PyObject* pyalpm_sync_newversion(PyObject *self, PyObject* args) {
  PyObject *pkg;
  PyObject *dbs;
  alpm_list_t *db_list;
  alpm_pkg_t *result;
  if(!PyArg_ParseTuple(args, "OO", &pkg, &dbs)
      || !PyAlpmPkg_Check(pkg)
      || pylist_db_to_alpmlist(dbs, &db_list) == -1)
  {
    PyErr_SetString(PyExc_TypeError, "sync_newversion() takes a Package and a list of DBs");
    return NULL;
  }

  {
    alpm_pkg_t *rawpkg = pmpkg_from_pyalpm_pkg(pkg);
    if (!rawpkg) return NULL;
    result = alpm_sync_newversion(rawpkg, db_list);
  }
  if (!result)
    Py_RETURN_NONE;
  else
    return pyalpm_package_from_pmpkg(result);
}

/* vim: set ts=2 sw=2 et: */
