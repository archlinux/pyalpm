/**
 * options.c : options module for pyalpm
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

#include <Python.h>
#include <alpm.h>
#include "handle.h"
#include "options.h"
#include "util.h"

static int PyLong_to_int(PyObject *value, int overflow_val)
{
  int overflow;
  long lval = PyLong_AsLongAndOverflow(value, &overflow);
  if (overflow != 0) return overflow_val;
  if (lval > INT_MAX || lval < INT_MIN) return overflow_val;
  return (int)lval;
}

/** Boolean options
 */
/*
receives and returns an int type
1 = enabled
0 = disabled
*/
PyObject * option_get_usesyslog_alpm(PyObject *self, void* closure)
{
  alpm_handle_t *handle = ALPM_HANDLE(self);
  int ret = alpm_option_get_usesyslog(handle);

  if(ret == -1)
  {
    RET_ERR("failed getting usesyslog", alpm_errno(handle), NULL);
  }
  else
    return PyLong_FromLong(ret);
}

int option_set_usesyslog_alpm(PyObject *self, PyObject *value, void* closure)
{
  alpm_handle_t *handle = ALPM_HANDLE(self);
  if(!PyLong_Check(value))
  {
    PyErr_SetString(PyExc_TypeError, "wrong arguments");
    return -1;
  }

  alpm_option_set_usesyslog(handle, PyLong_to_int(value, -1));
  return 0;
}

PyObject* option_get_checkspace_alpm(PyObject *self, void* closure) {
  alpm_handle_t *handle = ALPM_HANDLE(self);
  int ret = alpm_option_get_checkspace(handle);
  if (ret == -1) {
    RET_ERR("failed getting checkspace", alpm_errno(handle), NULL);
  } else
    return PyLong_FromLong(ret);
}

int option_set_checkspace_alpm(PyObject *self, PyObject *value, void* closure)
{
  alpm_handle_t *handle = ALPM_HANDLE(self);
  if(!PyLong_Check(value))
  {
    PyErr_SetString(PyExc_TypeError, "wrong arguments");
    return -1;
  }
  alpm_option_set_checkspace(handle, PyLong_to_int(value, -1));
  return 0;
}

/**
 * List options
 * in addition to getters/setters, these have add/remove methods
 */

PyObject* option_get_cachedirs_alpm(PyObject *self, void* closure) {
  alpm_handle_t *handle = ALPM_HANDLE(self);
  return alpmlist_to_pylist(alpm_option_get_cachedirs(handle), pyobject_from_string);
}

int option_set_cachedirs_alpm(PyObject *self, PyObject *value, void *closure)
{
  alpm_handle_t *handle = ALPM_HANDLE(self);
  alpm_list_t *target;
  if (pylist_string_to_alpmlist(value, &target) == -1)
    return -1;

  alpm_option_set_cachedirs(handle, target);
  return 0;
}

PyObject* option_get_noupgrades_alpm(PyObject *self, void* closure) {
  alpm_handle_t *handle = ALPM_HANDLE(self);
  return alpmlist_to_pylist(alpm_option_get_noupgrades(handle), pyobject_from_string);
}

int option_set_noupgrades_alpm(PyObject *self, PyObject *value, void *closure)
{
  alpm_handle_t *handle = ALPM_HANDLE(self);
  alpm_list_t *target;
  if (pylist_string_to_alpmlist(value, &target) == -1)
    return -1;

  alpm_option_set_noupgrades(handle, target);
  return 0;
}

PyObject* option_get_noextracts_alpm(PyObject *self, void* closure) {
  alpm_handle_t *handle = ALPM_HANDLE(self);
  return alpmlist_to_pylist(alpm_option_get_noextracts(handle), pyobject_from_string);
}

int option_set_noextracts_alpm(PyObject *self, PyObject *value, void *closure)
{
  alpm_handle_t *handle = ALPM_HANDLE(self);
  alpm_list_t *target;
  if (pylist_string_to_alpmlist(value, &target) == -1)
    return -1;

  alpm_option_set_noextracts(handle, target);
  return 0;
}

PyObject* option_get_ignorepkgs_alpm(PyObject *self, void* closure) {
  alpm_handle_t *handle = ALPM_HANDLE(self);
  return alpmlist_to_pylist(alpm_option_get_ignorepkgs(handle), pyobject_from_string);
}

int option_set_ignorepkgs_alpm(PyObject *self, PyObject *value, void *closure)
{
  alpm_handle_t *handle = ALPM_HANDLE(self);
  alpm_list_t *target;
  if (pylist_string_to_alpmlist(value, &target) == -1)
    return -1;

  alpm_option_set_ignorepkgs(handle, target);
  return 0;
}

PyObject* option_get_ignoregrps_alpm(PyObject *self, void* closure) {
  alpm_handle_t *handle = ALPM_HANDLE(self);
  return alpmlist_to_pylist(alpm_option_get_ignoregroups(handle), pyobject_from_string);
}

int option_set_ignoregrps_alpm(PyObject *self, PyObject *value, void *closure)
{
  alpm_handle_t *handle = ALPM_HANDLE(self);
  alpm_list_t *target;
  if (pylist_string_to_alpmlist(value, &target) == -1)
    return -1;

  alpm_option_set_ignoregroups(handle, target);
  return 0;
}

/* list options modifiers : add/remove */

PyObject* option_add_noupgrade_alpm(PyObject *self, PyObject *args)
{
  alpm_handle_t *handle = ALPM_HANDLE(self);
  const char *str;

  if(!PyArg_ParseTuple(args, "s", &str)) {
    PyErr_SetString(PyExc_TypeError, "expecting a string argument");
    return NULL;
  }
  alpm_option_add_noupgrade(handle, str);
  Py_RETURN_NONE;
}

PyObject* option_remove_noupgrade_alpm(PyObject *self, PyObject *args)
{
  alpm_handle_t *handle = ALPM_HANDLE(self);
  const char *str;

  if(!PyArg_ParseTuple(args, "s", &str)) {
    PyErr_SetString(PyExc_TypeError, "expecting a string argument");
    return NULL;
  }
  alpm_option_remove_noupgrade(handle, str);
  Py_RETURN_NONE;
}

PyObject* option_add_cachedir_alpm(PyObject *self, PyObject *args)
{
  alpm_handle_t *handle = ALPM_HANDLE(self);
  const char *str;

  if(!PyArg_ParseTuple(args, "s", &str)) {
    PyErr_SetString(PyExc_TypeError, "expecting a string argument");
    return NULL;
  }
  alpm_option_add_cachedir(handle, str);
  Py_RETURN_NONE;
}

PyObject* option_remove_cachedir_alpm(PyObject *self, PyObject *args)
{
  alpm_handle_t *handle = ALPM_HANDLE(self);
  const char *str;

  if(!PyArg_ParseTuple(args, "s", &str)) {
    PyErr_SetString(PyExc_TypeError, "expecting a string argument");
    return NULL;
  }
  alpm_option_remove_cachedir(handle, str);
  Py_RETURN_NONE;
}

PyObject* option_add_noextract_alpm(PyObject *self, PyObject *args)
{
  alpm_handle_t *handle = ALPM_HANDLE(self);
  const char *str;

  if(!PyArg_ParseTuple(args, "s", &str)) {
    PyErr_SetString(PyExc_TypeError, "expecting a string argument");
    return NULL;
  }
  alpm_option_add_noextract(handle, str);
  Py_RETURN_NONE;
}

PyObject* option_remove_noextract_alpm(PyObject *self, PyObject *args)
{
  alpm_handle_t *handle = ALPM_HANDLE(self);
  const char *str;

  if(!PyArg_ParseTuple(args, "s", &str)) {
    PyErr_SetString(PyExc_TypeError, "expecting a string argument");
    return NULL;
  }
  alpm_option_remove_noextract(handle, str);
  Py_RETURN_NONE;
}

PyObject* option_add_ignorepkg_alpm(PyObject *self, PyObject *args)
{
  alpm_handle_t *handle = ALPM_HANDLE(self);
  const char *str;

  if(!PyArg_ParseTuple(args, "s", &str)) {
    PyErr_SetString(PyExc_TypeError, "expecting a string argument");
    return NULL;
  }
  alpm_option_add_ignorepkg(handle, str);
  Py_RETURN_NONE;
}

PyObject* option_remove_ignorepkg_alpm(PyObject *self, PyObject *args)
{
  alpm_handle_t *handle = ALPM_HANDLE(self);
  const char *str;

  if(!PyArg_ParseTuple(args, "s", &str)) {
    PyErr_SetString(PyExc_TypeError, "expecting a string argument");
    return NULL;
  }
  alpm_option_remove_ignorepkg(handle, str);
  Py_RETURN_NONE;
}

PyObject* option_add_ignoregrp_alpm(PyObject *self, PyObject *args)
{
  alpm_handle_t *handle = ALPM_HANDLE(self);
  const char *str;

  if(!PyArg_ParseTuple(args, "s", &str)) {
    PyErr_SetString(PyExc_TypeError, "expecting a string argument");
    return NULL;
  }
  alpm_option_add_ignoregroup(handle, str);
  Py_RETURN_NONE;
}

PyObject* option_remove_ignoregrp_alpm(PyObject *self, PyObject *args)
{
  alpm_handle_t *handle = ALPM_HANDLE(self);
  const char *str;

  if(!PyArg_ParseTuple(args, "s", &str)) {
    PyErr_SetString(PyExc_TypeError, "expecting a string argument");
    return NULL;
  }
  alpm_option_remove_ignoregroup(handle, str);
  Py_RETURN_NONE;
}

/** Callback wrappers */
extern PyObject *global_py_callbacks[N_CALLBACKS];

void pyalpm_logcb(alpm_loglevel_t level, const char *fmt, va_list va_args) {
  char *log;
  PyObject *result;
  int ret;

  ret = vasprintf(&log, fmt, va_args);
  if(ret == -1)
    log = "pyalpm_logcb: could not allocate memory";
  result = PyObject_CallFunction(global_py_callbacks[CB_LOG], "is", level, log);
  if (!result) PyErr_Print();
  Py_CLEAR(result);
  if (ret != -1) free(log);
}

void pyalpm_dlcb(const char *filename, off_t xfered, off_t total) {
  PyObject *result;
  result = PyObject_CallFunction(global_py_callbacks[CB_DOWNLOAD], "sii", filename, xfered, total);
  if (!result) PyErr_Print();
  Py_CLEAR(result);
}

void pyalpm_totaldlcb(off_t total) {
  PyObject *result;
  result = PyObject_CallFunction(global_py_callbacks[CB_TOTALDL], "i", total);
  if (!result) PyErr_Print();
  Py_CLEAR(result);
}

int pyalpm_fetchcb(const char *url, const char *localpath, int force) {
  PyObject *result;
  result = PyObject_CallFunction(global_py_callbacks[CB_FETCH], "ssi", url, localpath, force);
  if (!result) return -1;
  if (!PyLong_Check(result)) {
    return -1;
  } else {
    int ret = PyLong_to_int(result, -1);
    Py_DECREF(result);
    return ret;
  }
}

/* vim: set ts=2 sw=2 et: */
