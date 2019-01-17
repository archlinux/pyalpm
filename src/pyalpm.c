/**
 * pyalpm - a Python C module wrapping libalpm
 *
 *  Copyright 2008 Imanol Celaya <ilcra1989@gmail.com>
 *  Copyright (c) 2011 Rémy Oudompheng <remy@archlinux.org>
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
 */

#include "pyalpm.h"
#include "util.h"
#include "package.h"
#include "db.h"

static PyObject * alpmversion_alpm(PyObject *self, PyObject *dummy)
{
  const char *str;
  str = alpm_version();

  return Py_BuildValue("s", str);
}

static PyObject * version_alpm(PyObject *self, PyObject *dummy)
{
  return Py_BuildValue("s", VERSION);
}

/** Finds a package satisfying a dependency constraint in a package list */
static PyObject* pyalpm_find_satisfier(PyObject *self, PyObject* args) {
  PyObject *pkglist;
  char *depspec;
  alpm_list_t *alpm_pkglist;
  alpm_pkg_t *p;

  if(!PyArg_ParseTuple(args, "Os", &pkglist, &depspec))
  {
    PyErr_SetString(PyExc_TypeError, "find_satisfier() takes a Package list and a string");
    return NULL;
  }

  if(pylist_pkg_to_alpmlist(pkglist, &alpm_pkglist) == -1)
    return NULL;

  p = alpm_find_satisfier(alpm_pkglist, depspec);
  alpm_list_free(alpm_pkglist);

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

static PyObject *pyalpm_vercmp(PyObject *self, PyObject *args) {
  const char *x, *y;
  int result;
  if (!PyArg_ParseTuple(args, "ss", &x, &y))
    return NULL;
  result = alpm_pkg_vercmp(x, y);
  return PyLong_FromLong(result);
}

static PyMethodDef methods[] = {
  {"version", version_alpm, METH_NOARGS, "returns pyalpm version."},
  {"alpmversion", alpmversion_alpm, METH_NOARGS, "returns alpm version."},
  {"vercmp", pyalpm_vercmp, METH_VARARGS, "compares version strings"},

  { "find_satisfier", pyalpm_find_satisfier, METH_VARARGS,
    "finds a package satisfying the given dependency among a list\n"
    "args: a list of packages, a dependency string\n"
    "returns: a Package object or None" },

  {"sync_newversion", pyalpm_sync_newversion, METH_VARARGS,
    "finds an available upgrade for a package in a list of databases\n"
    "args: a package, a list of databases\n"
    "returns: an upgrade candidate or None" },

  /* from db.c */
  {"find_grp_pkgs", pyalpm_find_grp_pkgs, METH_VARARGS,
   "find packages from a given group across databases\n"
   "args: a list of databases, a group name"},

  {NULL, NULL, 0, NULL}
};

static int pyalpm_clear(PyObject *m)
{
  PyObject *dict = PyModule_GetDict(m);
  if (dict == NULL) {
    return 0;
  }

  PyObject *error = PyDict_GetItemString(dict, "error");
  if (error == NULL) {
    return 0;
  }

  Py_DECREF(error);

  return 0;
}

static struct PyModuleDef pyalpm_def = {
  PyModuleDef_HEAD_INIT,
  "alpm",
  "This module wraps the libalpm library",
  -1,
  methods,
  NULL, NULL,
  pyalpm_clear,
  NULL,
};

PyMODINIT_FUNC PyInit_pyalpm(void)
{
  PyObject* m = PyModule_Create(&pyalpm_def);

  init_pyalpm_error(m);
  init_pyalpm_handle(m);
  init_pyalpm_package(m);
  init_pyalpm_db(m);
  init_pyalpm_transaction(m);

  return m;
}

/* vim: set ts=2 sw=2 et: */

