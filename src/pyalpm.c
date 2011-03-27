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
#include "optionsobject.h"

/*pyalpm functions*/
static PyObject * initialize_alpm(PyObject *self, PyObject *dummy)
{
  if(alpm_initialize() == -1) {
    PyErr_SetString(alpm_error, "could not initialize libalpm");
    return NULL;
  }

  set_init(1);
  Py_RETURN_NONE;
}

static PyObject * release_alpm(PyObject *self, PyObject *dummy)
{
  /*clean_memory(addresses);*/
  if(alpm_release() == -1)
  {
    PyErr_SetString(alpm_error, "failed to release alpm");
    return NULL;
  }
  else
  {
    set_init(0);
    Py_RETURN_NONE;
  }
}

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

static PyObject * check_init_alpm(PyObject *self, PyObject *dummy)
{
  if(check_init() == 0)
  {
    Py_RETURN_FALSE;
  }
  if(check_init() == 1)
  {
    Py_RETURN_TRUE;
  }
  else
  {
    PyErr_SetString(alpm_error, "internal error");
    return NULL;
  }
}

static PyObject* pyalpm_get_localdb(PyObject *self, PyObject *dummy) {
  return pyalpm_db_from_pmdb(alpm_option_get_localdb());
}

static PyObject* pyalpm_get_syncdbs(PyObject *self, PyObject *dummy) {
  return alpmlist_to_pylist(alpm_option_get_syncdbs(),
			    pyalpm_db_from_pmdb);
}

static PyObject* pyalpm_register_syncdb(PyObject *self, PyObject *args) {
  const char *dbname;
  pmdb_t *result;

  if (!PyArg_ParseTuple(args, "s", &dbname)) {
    PyErr_SetString(PyExc_TypeError, "expected a string argument");
    return NULL;
  }

  result = alpm_db_register_sync(dbname);
  if (! result) {
    PyErr_Format(alpm_error, "unable to register sync database %s", dbname);
    return NULL;
  }

  return pyalpm_db_from_pmdb(result);
}

static PyMethodDef methods[] = {
  {"initialize", initialize_alpm, METH_NOARGS, "initialize alpm."},
  {"release", release_alpm, METH_NOARGS, "release alpm."},
  {"version", version_alpm, METH_NOARGS, "returns pyalpm version."},
  {"alpmversion", alpmversion_alpm, METH_NOARGS, "returns alpm version."},
  {"checkinit", check_init_alpm, METH_VARARGS, "checks if the library was initialized."},

  {"register_syncdb", pyalpm_register_syncdb, METH_VARARGS,
   "registers the database with the given name\n"
   "returns the new database on success"},
  {"get_localdb", pyalpm_get_localdb, METH_NOARGS, "returns an object representing the local DB"},
  {"get_syncdbs", pyalpm_get_syncdbs, METH_NOARGS, "returns a list of sync DBs"},

  /* from package.c */
  {"load_pkg", pyalpm_package_load, METH_VARARGS, "loads package information from a tarball"},

  {NULL, NULL, 0, NULL}
};

static struct PyModuleDef pyalpm_def = {
  PyModuleDef_HEAD_INIT,
  "alpm",
  "This module wraps the libalpm library",
  -1,
  methods
};

PyMODINIT_FUNC PyInit_pyalpm()
{
  PyObject* m = PyModule_Create(&pyalpm_def);

  init_pyalpm_error(m);
  init_pyalpm_options(m);
  init_pyalpm_package(m);
  init_pyalpm_db(m);

  return m;
}
