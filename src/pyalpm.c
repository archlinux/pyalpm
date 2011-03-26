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

extern unsigned short init;

/* pyalpm errors */
PyObject *alpm_error = NULL;

/*pyalpm functions*/
static PyObject * initialize_alpm(PyObject *self, PyObject *dummy)
{
  if(alpm_initialize() == -1)
  {
    PyErr_SetString(alpm_error, "failed to initialize alpm");
    return NULL;
  }
  else
  {
    init = 1;
    return Py_None;
  }
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
    init = 0;
    return Py_None;
  }
}

void test_cb(pmloglevel_t level, char *fmt, va_list args)
{
  if(strlen(fmt))
  {
    switch(level)
    {
      case PM_LOG_ERROR: printf("%s", error); break;
      case PM_LOG_WARNING: printf("%s", warning); break;
      case PM_LOG_DEBUG: printf("%s", debug); break;
      case PM_LOG_FUNCTION: printf("%s", function); break;
      default: return;
    }
    vprintf(fmt, args);
  }
}

static PyObject * option_set_logcb_alpm(PyObject *self, PyObject *args)
{
  if(!PyArg_ParseTuple(args, "ssss", &error, &warning, &debug, &function))
  {
    PyErr_SetString(alpm_error, "incorrect arguments");
    return NULL;
  }
  
  else
  {
    alpm_option_set_logcb(test_cb);
    
    return Py_None;
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
  if(init == 0)
  {
    return Py_False;
  }
  if(init == 1)
  {
    return Py_True;
  }
  else
  {
    PyErr_SetString(alpm_error, "internal error");
    return NULL;
  }
}

static PyObject* pyalpm_get_localdb(PyObject *self) {
  PyObject *db;

  if (self == NULL) {
    PyErr_SetString(PyExc_RuntimeError, "unable to create DB object");
    return NULL;
  }

  db = pyalpm_db_from_pmdb(alpm_option_get_localdb());
  return (PyObject *)db;
}

static PyMethodDef methods[] = {
  {"initialize", initialize_alpm, METH_VARARGS, "initialize alpm."},
  {"release", release_alpm, METH_VARARGS, "release alpm."},
  {"version", version_alpm, METH_VARARGS, "returns pyalpm version."},
  {"alpmversion", alpmversion_alpm, METH_VARARGS, "returns alpm version."},
  {"checkinit", check_init_alpm, METH_VARARGS, "checks if the library was initialized."},

  {"get_localdb", pyalpm_get_localdb, METH_NOARGS, "returns an object representing the local DB"},

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

  alpm_error = PyErr_NewException("alpm.error", NULL, NULL);

  init_pyalpm_options(m);
  init_pyalpm_package(m);
  init_pyalpm_db(m);
  
  return m;
}

