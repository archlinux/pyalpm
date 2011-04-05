/**
 * db.c : wrapper class around pmdb_t
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

#include <alpm.h>
#include <Python.h>
#include "db.h"
#include "package.h"
#include "util.h"

typedef struct _AlpmDB {
  PyObject_HEAD
  pmdb_t *c_data;
} AlpmDB;

static PyTypeObject AlpmDBType;

static void pyalpm_db_dealloc(AlpmDB *self) {
  Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject* _pyobject_from_pmgrp(void *group) {
  pmgrp_t* grp = (pmgrp_t*)group;
  if (!grp)
    Py_RETURN_NONE;
  PyObject *fst = PyUnicode_FromString(alpm_grp_get_name(grp));
  PyObject *snd = alpmlist_to_pylist(alpm_grp_get_pkgs(grp),
				     pyalpm_package_from_pmpkg);
  PyObject *tuple = PyTuple_Pack(2, fst, snd);
  Py_DECREF(fst);
  Py_DECREF(snd);
  return tuple;
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
  CHECK_IF_INITIALIZED();
  const char* name = alpm_db_get_name(self->c_data);
  if (!name)
    Py_RETURN_NONE;
  return PyUnicode_FromString(name);
}

static PyObject* pyalpm_db_get_url(AlpmDB* self, void* closure) {
  CHECK_IF_INITIALIZED();
  const char* url = alpm_db_get_url(self->c_data);
  if (!url)
    Py_RETURN_NONE;
  return PyUnicode_FromString(url);
}

static int pyalpm_db_set_server(AlpmDB* self, PyObject* value, void* closure) {
  char *path = NULL;
  PyObject *utf8 = NULL;
  int ret;
  if (PyBytes_Check(value)) {
    path = PyBytes_AS_STRING(value);
  } else if (PyUnicode_Check(value)) {
    utf8 = PyUnicode_AsUTF8String(value);
    path = PyBytes_AS_STRING(utf8);
  } else {
    PyErr_SetString(PyExc_TypeError, "expected a string value");
    return -1;
  }

  ret = alpm_db_setserver(self->c_data, path);
  if (utf8) Py_DECREF(utf8);
  if (ret == -1)
    PyErr_SetString(alpm_error, alpm_strerrorlast());

  return ret;
}

static PyObject* pyalpm_db_get_pkgcache(AlpmDB* self, void* closure) {
  alpm_list_t *pkglist = alpm_db_get_pkgcache(self->c_data);
  return alpmlist_to_pylist(pkglist, pyalpm_package_from_pmpkg);
}

static PyObject* pyalpm_db_get_grpcache(AlpmDB* self, void* closure) {
  alpm_list_t *grplist = alpm_db_get_grpcache(self->c_data);
  return alpmlist_to_pylist(grplist, _pyobject_from_pmgrp);
}

/** Package get/set operations */

static PyObject* pyalpm_db_get_pkg(PyObject *rawself, PyObject* args) {
  char *pkgname;
  pmpkg_t *p;
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

static PyObject* pyalpm_db_set_pkgreason(PyObject* rawself, PyObject* args) {
  char *pkgname;
  int reason;
  AlpmDB *self = (AlpmDB*)rawself;
  int ret;
  if (!PyArg_ParseTuple(args, "si", &pkgname, &reason)) {
    PyErr_SetString(PyExc_TypeError, "expected arguments (str, int)");
    return NULL;
  }

  PYALPM_ERR(self->c_data == NULL, "data is not initialized");
  ret = alpm_db_set_pkgreason(self->c_data, pkgname, reason);

  PYALPM_ERR(ret == -1, "failed setting install reason");
  Py_RETURN_NONE;
}

static PyObject* pyalpm_db_readgrp(PyObject* rawself, PyObject* args) {
  AlpmDB* self = (AlpmDB*)rawself;
  char *grpname;
  pmgrp_t *grp;
  if (!PyArg_ParseTuple(args, "s", &grpname)) {
    PyErr_SetString(PyExc_TypeError, "expected string argument");
    return NULL;
  }

  grp = alpm_db_readgrp(self->c_data, grpname);
  return _pyobject_from_pmgrp(grp);
}

static struct PyMethodDef db_methods[] = {
  { "get_pkg", pyalpm_db_get_pkg, METH_VARARGS,
    "get a package by name\n"
    "args: a package name (string)\n"
    "returns: a Package object or None if not found" },
  { "read_grp", pyalpm_db_readgrp, METH_VARARGS,
    "get contents of a group\n"
    "args: a group name (string)\n"
    "returns: a tuple (group name, list of packages)" },
  { "set_pkgreason", pyalpm_db_set_pkgreason, METH_VARARGS,
    "set install reason for a package\n"
    "args: a package name (string), a reason (PKG_REASON_DEPEND, PKG_REASON_EXPLICIT)\n"
    "returns: None" },
  { NULL },
};

struct PyGetSetDef db_getset[] = {
  /* description properties */
  { "name", (getter)pyalpm_db_get_name, 0,
    "database name (e.g. \"core\", \"extra\")", NULL } ,
  { "url", (getter)pyalpm_db_get_url, (setter)pyalpm_db_set_server, "URL (for sync DBs)", NULL } ,
  { "pkgcache", (getter)pyalpm_db_get_pkgcache, 0, "(read only) list of packages", NULL } ,
  { "grpcache", (getter)pyalpm_db_get_grpcache, 0, "(read only) list of package groups", NULL } ,
  { NULL }
};

static PyTypeObject AlpmDBType = {
  PyVarObject_HEAD_INIT(NULL, 0)
  "alpm.DB",             /*tp_name*/
  sizeof(AlpmDB),        /*tp_basicsize*/
  0,                          /*tp_itemsize*/
  (destructor)pyalpm_db_dealloc,     /*tp_dealloc*/
  0,                          /*tp_print*/
  0,                          /*tp_getattr*/
  0,                          /*tp_setattr*/
  NULL,                       /*tp_reserved*/
  0,                          /*tp_repr*/
  0,                          /*tp_as_number*/
  0,                          /*tp_as_sequence*/
  0,                          /*tp_as_mapping*/
  0,                          /*tp_hash */
  0,                          /*tp_call*/
  0,                          /*tp_str*/
  0,                          /*tp_getattro*/
  0,                          /*tp_setattro*/
  0,                          /*tp_as_buffer*/
  Py_TPFLAGS_DEFAULT,         /*tp_flags*/
  "libalpm DB object",        /* tp_doc */
  0,                          /* tp_traverse */
  0,                          /* tp_clear */
  0,                          /* tp_richcompare */
  0,                          /* tp_weaklistoffset */
  0,                          /* tp_iter */
  0,                          /* tp_iternext */
  db_methods,                 /* tp_methods */
  0,                          /* tp_members */
  db_getset,                  /* tp_getset */
  0,                          /* tp_base */
};

/** Initializes Pb class in module */
void init_pyalpm_db(PyObject *module) {
  PyObject *type;

  if (PyType_Ready(&AlpmDBType) < 0)
    return;
  type = (PyObject*)&AlpmDBType;
  Py_INCREF(type);
  PyModule_AddObject(module, "DB", type);
}


PyObject *pyalpm_db_from_pmdb(void* data) {
  pmdb_t *db = (pmdb_t*)data;
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

  int ret = pylist_db_to_alpmlist(dbs, &db_list);
  if (ret == -1)
    return NULL;

  pkg_list = alpm_find_grp_pkgs(db_list, grpname);
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
  pmpkg_t *result;
  if(!PyArg_ParseTuple(args, "OO", &pkg, &dbs)
      || !PyAlpmPkg_Check(pkg)
      || pylist_db_to_alpmlist(dbs, &db_list) == -1)
  {
    PyErr_SetString(PyExc_TypeError, "sync_newversion() takes a Package and a list of DBs");
    return NULL;
  }

  pmpkg_t *rawpkg = pmpkg_from_pyalpm_pkg(pkg);
  if (!rawpkg)
    return NULL;

  result = alpm_sync_newversion(pmpkg_from_pyalpm_pkg((void*)rawpkg), db_list);
  if (!result)
    Py_RETURN_NONE;
  else
    return pyalpm_package_from_pmpkg(result);
}

/* vim: set ts=2 sw=2 et: */
