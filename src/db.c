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

extern PyObject* alpm_error;

typedef struct _AlpmDB {
  PyObject_HEAD
  pmdb_t *c_data;
} AlpmDB;

static void pyalpm_db_dealloc(AlpmDB *self) {
  Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject* _pyobject_from_pmgrp(void *group) {
  pmgrp_t* grp = (pmgrp_t*)group;
  PyObject *fst = PyUnicode_FromString(alpm_grp_get_name(grp));
  PyObject *snd = alpmlist_to_pylist(alpm_grp_get_pkgs(grp),
				     pyalpm_package_from_pmpkg);
  PyObject *tuple = PyTuple_Pack(2, fst, snd);
  Py_DECREF(fst);
  Py_DECREF(snd);
  return tuple;
}

#define CHECK_IF_INITIALIZED() if (! self->c_data) { \
  PyErr_SetString(alpm_error, "data is not initialized"); \
  return NULL; \
  }

static PyObject* pyalpm_db_get_pkg(AlpmDB* self, PyObject* args) {
  char *pkgname;
  pmpkg_t *p;

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

static PyObject* pyalpm_db_get_pkgcache(AlpmDB* self, void* closure) {
  alpm_list_t *pkglist = alpm_db_get_pkgcache(self->c_data);
  return alpmlist_to_pylist(pkglist, pyalpm_package_from_pmpkg);
}

static PyObject* pyalpm_db_get_grpcache(AlpmDB* self, void* closure) {
  alpm_list_t *grplist = alpm_db_get_grpcache(self->c_data);
  return alpmlist_to_pylist(grplist, _pyobject_from_pmgrp);
}

static struct PyMethodDef db_methods[] = {
  { "get_pkg", pyalpm_db_get_pkg, METH_VARARGS, "get a package by name" },
  { NULL },
};

struct PyGetSetDef db_getset[] = {
  /* description properties */
  { "name", (getter)pyalpm_db_get_name, 0, "name", NULL } ,
  { "url", (getter)pyalpm_db_get_url, 0, "URL", NULL } ,
  { "pkgcache", (getter)pyalpm_db_get_pkgcache, 0, "list of packages", NULL } ,
  { "grpcache", (getter)pyalpm_db_get_grpcache, 0, "list of package groups", NULL } ,
  { NULL }
};

PyTypeObject AlpmDBType = {
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

PyObject *pyalpm_db_from_pmdb(pmdb_t *db) {
  AlpmDB *self;
  self = (AlpmDB*)AlpmDBType.tp_alloc(&AlpmDBType, 0);
  if (self == NULL) {
    PyErr_SetString(PyExc_RuntimeError, "unable to create DB object");
    return NULL;
  }

  self->c_data = db;
  return (PyObject *)self;
}

