/**
 * util.c : utility functions for pyalpm
 *
 *  Copyright 2008 Imanol Celaya <ilcra1989@gmail.com>
 *  Copyright 2011 Rémy Oudompheng <remy@archlinux.org>
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

/* Include pyconfig for feature test macros. */
#include <pyconfig.h>
#include <string.h>

#include <Python.h>
#include <alpm.h>
#include <alpm_list.h>

/** Errors */

PyObject* alpm_error = NULL;

/** Formats an alpm.error exception using errno from libalpm. */
static PyObject* pyalpm_error_str(PyObject* exception) {
  PyObject* args = PyObject_GetAttrString(exception, "args");
  PyObject* result;
  const char* errstring;
  enum _alpm_errno_t errcode;
  PyObject *data;

  int handle_format;
  {
    /* check whether alpm.error was set with standard args */
    PyObject *exctype, *excvalue, *exctraceback;
    PyErr_Fetch(&exctype, &excvalue, &exctraceback);
    handle_format = PyArg_ParseTuple(args, "siO",
        &errstring, &errcode, &data);
    if (!handle_format) {
      PyErr_Clear();
      result = PyObject_Str(args);
    }
    PyErr_Restore(exctype, excvalue, exctraceback);
  }

  if(handle_format) {
    if (data == Py_None) {
      result = PyUnicode_FromFormat("%s, pm_errno %d (%s)", errstring, errcode, alpm_strerror(errcode));
    } else {
      result = PyUnicode_FromFormat("%s, pm_errno %d (%s), %S", errstring, errcode, alpm_strerror(errcode), data);
    }
  }

  Py_DECREF(args);
  return result;
}

void init_pyalpm_error(PyObject* module) {
  alpm_error = PyErr_NewExceptionWithDoc("alpm.error",
      "Exception raised when an error arises from libalpm\n"
      "The args attribute will usually contain a tuple "
      "(error message, errno from libalpm, extra data)\n",
      NULL, NULL);
  ((PyTypeObject*)alpm_error)->tp_str = pyalpm_error_str;
  PyModule_AddObject(module, "error", alpm_error);
  Py_INCREF(alpm_error);
}

/** Python lists and libalpm lists */

/** Converts a Python list of strings to an alpm_list_t linked list.
 * return 0 on success, -1 on failure
 */
int pylist_string_to_alpmlist(PyObject *list, alpm_list_t* *result)
{
  alpm_list_t *ret = NULL;
  PyObject *iterator = PyObject_GetIter(list);
  PyObject *item;

  if(iterator == NULL) {
    PyErr_SetString(PyExc_TypeError, "object is not iterable");
    return -1;
  }

  while((item = PyIter_Next(iterator)))
  {
    if (PyBytes_Check(item)) {
      ret = alpm_list_add(ret, strdup(PyBytes_AS_STRING(item)));
    } else if (PyUnicode_Check(item)) {
      PyObject* utf8 = PyUnicode_AsUTF8String(item);
      ret = alpm_list_add(ret, strdup(PyBytes_AS_STRING(utf8)));
      Py_DECREF(utf8);
    } else {
      PyErr_SetString(PyExc_TypeError, "list must contain only strings");
      FREELIST(ret);
      Py_DECREF(item);
      return -1;
    }
    Py_DECREF(item);
  }
  Py_DECREF(iterator);

  *result = ret;
  return 0;
}

PyObject* pyobject_from_string(void *s) {
  return Py_BuildValue("s", (char*)s);
}

PyObject* alpmlist_to_pylist(alpm_list_t *prt, PyObject* pybuilder(void*))
{
  PyObject *output, *stritem;
  alpm_list_t *tmp;

  output = PyList_New(0);
  if(output == NULL) {
    PyErr_SetString(PyExc_RuntimeError, "unable to create list object");
    return NULL;
  }

  for(tmp = prt; tmp; tmp = alpm_list_next(tmp)) {
    stritem = pybuilder(tmp->data);
    if (!stritem) {
      Py_CLEAR(stritem);
      return NULL;
    }
    PyList_Append(output, stritem);
    Py_CLEAR(stritem);
  }

  return output;
}

PyObject* alpmlist_to_pylist2(alpm_list_t *prt, PyObject* pybuilder2(void*, PyObject *self), PyObject *self)
{
  /* Check if *self is null to remove this function? */
  PyObject *output, *stritem;
  alpm_list_t *tmp;

  output = PyList_New(0);
  if(output == NULL) {
    PyErr_SetString(PyExc_RuntimeError, "unable to create list object");
    return NULL;
  }

  for(tmp = prt; tmp; tmp = alpm_list_next(tmp)) {
    stritem = pybuilder2(tmp->data, self);
    if (!stritem) {
      Py_CLEAR(stritem);
      return NULL;
    }
    PyList_Append(output, stritem);
    Py_CLEAR(stritem);
  }

  return output;
}

/* vim: set ts=2 sw=2 et: */
