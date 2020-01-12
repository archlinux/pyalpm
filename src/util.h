/**
 * util.h : utility functions for pyalpm
 *
 *  Copyright 2008 Imanol Celaya <ilcra1989@gmail.com>
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

#ifndef _PY_ALPM_UTIL_H
#define _PY_ALPM_UTIL_H

#include <Python.h>
#include <alpm.h>
#include <alpm_list.h>

/** error objects */
extern PyObject* alpm_error;
void init_pyalpm_error(PyObject* module);

#define RET_ERR(msg, errno, ret) do { \
      PyObject *error_obj = Py_BuildValue("(siO)", msg, errno, Py_None); \
      Py_INCREF(Py_None); \
      PyErr_SetObject(alpm_error, error_obj); \
      return ret;                          \
    } while(0)

/* macro to return Python objects in the exception: the use of 'N'
 * format char is to steal a reference to arg
 */
#define RET_ERR_DATA(msg, errno, arg, ret) do { \
      PyObject *error_obj = Py_BuildValue("(siN)", msg, errno, arg); \
      PyErr_SetObject(alpm_error, error_obj); \
      return ret;                          \
    } while(0)

typedef PyObject *(pyobjectbuilder)(void*);
typedef PyObject *(pyobjectbuilder2)(void*, PyObject*);
PyObject* pyobject_from_string(void *s);

/** List conversion functions */
PyObject* alpmlist_to_pylist(alpm_list_t *prt, pyobjectbuilder pybuilder);
PyObject* alpmlist_to_pylist2(alpm_list_t *prt, pyobjectbuilder2 pybuilder, PyObject *self);
int pylist_string_to_alpmlist(PyObject *list, alpm_list_t* *result);

#endif

/* vim: set ts=2 sw=2 et: */
