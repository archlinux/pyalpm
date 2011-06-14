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

/**
 * Path options getters/setters
 */

PyObject * option_get_root_alpm(PyObject *self, void *closure)
{
  pmhandle_t *handle = ALPM_HANDLE(self);
  const char *str = alpm_option_get_root(handle);
  if (!str) RET_ERR("failed getting root path", alpm_errno(handle), NULL);
  return Py_BuildValue("s", str);
}

PyObject* option_get_dbpath_alpm(PyObject *self, void *closure)
{
  pmhandle_t *handle = ALPM_HANDLE(self);
  const char *str = alpm_option_get_dbpath(handle);

  if(str == NULL)
    RET_ERR("failed getting dbpath.", alpm_errno(handle), NULL);
  return Py_BuildValue("s", str);
}

int option_set_logfile_alpm(PyObject *self, PyObject *value, void* closure)
{
  pmhandle_t *handle = ALPM_HANDLE(self);
  char *path = NULL;
  int ret;
  if (PyBytes_Check(value)) {
    path = strdup(PyBytes_AS_STRING(value));
  } else if (PyUnicode_Check(value)) {
    PyObject* utf8 = PyUnicode_AsUTF8String(value);
    path = strdup(PyBytes_AS_STRING(utf8));
    Py_DECREF(utf8);
  } else {
    PyErr_SetString(PyExc_TypeError, "logfile path must be a string");
    return -1;
  }

  ret = alpm_option_set_logfile(handle, path);
  free(path);
  if (ret == -1) RET_ERR("failed setting logfile path", alpm_errno(handle), -1);

  return 0;
}

PyObject * option_get_logfile_alpm(PyObject *self, void* closure)
{
  pmhandle_t *handle = ALPM_HANDLE(self);
  const char *str = alpm_option_get_logfile(handle);

  if(str == NULL)
  {
    RET_ERR("failed getting logfile.", alpm_errno(handle), NULL);
  }
  else
  {
    return Py_BuildValue("s", str);
  }
}

PyObject* option_get_lockfile_alpm(PyObject *self, void *closure)
{
  pmhandle_t *handle = ALPM_HANDLE(self);
  const char *str = alpm_option_get_lockfile(handle);

  if(str == NULL)
  {
    RET_ERR("failed getting lockfile", alpm_errno(handle), NULL);
  }
  else
    return PyUnicode_FromString(str);
}

/**
 * String options
 */

int option_set_arch_alpm(PyObject *self, PyObject *value, void* closure)
{
  pmhandle_t *handle = ALPM_HANDLE(self);
  char *string = NULL;
  if (PyBytes_Check(value)) {
    string = strdup(PyBytes_AS_STRING(value));
  } else if (PyUnicode_Check(value)) {
    PyObject* utf8 = PyUnicode_AsUTF8String(value);
    string = strdup(PyBytes_AS_STRING(utf8));
    Py_DECREF(utf8);
  } else {
    PyErr_SetString(PyExc_TypeError, "arch must be a string");
    return -1;
  }

  alpm_option_set_arch(handle, string);
  free(string);
  return 0;
}

PyObject* option_get_arch_alpm(PyObject *self, void* closure) {
  pmhandle_t *handle = ALPM_HANDLE(self);
  const char *str = alpm_option_get_arch(handle);

  if(str == NULL) {
    RET_ERR("failed getting arch", alpm_errno(handle), NULL);
  }
  return Py_BuildValue("s", str);
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
  pmhandle_t *handle = ALPM_HANDLE(self);
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
  pmhandle_t *handle = ALPM_HANDLE(self);
  if(!PyLong_Check(value))
  {
    PyErr_SetString(PyExc_TypeError, "wrong arguments");
    return -1;
  }

  alpm_option_set_usesyslog(handle, PyLong_AsLong(value));
  return 0;
}

PyObject* option_get_usedelta_alpm(PyObject *self, void* closure) {
  pmhandle_t *handle = ALPM_HANDLE(self);
  int ret = alpm_option_get_usedelta(handle);
  if (ret == -1) {
    RET_ERR("failed getting usedelta", alpm_errno(handle), NULL);
  } else
    return PyLong_FromLong(ret);
}

int option_set_usedelta_alpm(PyObject *self, PyObject *value, void* closure)
{
  pmhandle_t *handle = ALPM_HANDLE(self);
  if(!PyLong_Check(value))
  {
    PyErr_SetString(PyExc_TypeError, "wrong arguments");
    return -1;
  }

  alpm_option_set_usedelta(handle, PyLong_AsLong(value));
  return 0;
}

PyObject* option_get_checkspace_alpm(PyObject *self, void* closure) {
  pmhandle_t *handle = ALPM_HANDLE(self);
  int ret = alpm_option_get_checkspace(handle);
  if (ret == -1) {
    RET_ERR("failed getting checkspace", alpm_errno(handle), NULL);
  } else
    return PyLong_FromLong(ret);
}

int option_set_checkspace_alpm(PyObject *self, PyObject *value, void* closure)
{
  pmhandle_t *handle = ALPM_HANDLE(self);
  if(!PyLong_Check(value))
  {
    PyErr_SetString(PyExc_TypeError, "wrong arguments");
    return -1;
  }
  alpm_option_set_checkspace(handle, PyLong_AsLong(value));
  return 0;
}

/**
 * List options
 * in addition to getters/setters, these have add/remove methods
 */

PyObject* option_get_cachedirs_alpm(PyObject *self, void* closure) {
  pmhandle_t *handle = ALPM_HANDLE(self);
  return alpmlist_to_pylist(alpm_option_get_cachedirs(handle), pyobject_from_string);
}

int option_set_cachedirs_alpm(PyObject *self, PyObject *value, void *closure)
{
  pmhandle_t *handle = ALPM_HANDLE(self);
  alpm_list_t *target;
  if (pylist_string_to_alpmlist(value, &target) == -1)
    return -1;

  alpm_option_set_cachedirs(handle, target);
  return 0;
}

PyObject* option_get_noupgrades_alpm(PyObject *self, void* closure) {
  pmhandle_t *handle = ALPM_HANDLE(self);
  return alpmlist_to_pylist(alpm_option_get_noupgrades(handle), pyobject_from_string);
}

int option_set_noupgrades_alpm(PyObject *self, PyObject *value, void *closure)
{
  pmhandle_t *handle = ALPM_HANDLE(self);
  alpm_list_t *target;
  if (pylist_string_to_alpmlist(value, &target) == -1)
    return -1;

  alpm_option_set_noupgrades(handle, target);
  return 0;
}

PyObject* option_get_noextracts_alpm(PyObject *self, void* closure) {
  pmhandle_t *handle = ALPM_HANDLE(self);
  return alpmlist_to_pylist(alpm_option_get_noextracts(handle), pyobject_from_string);
}

int option_set_noextracts_alpm(PyObject *self, PyObject *value, void *closure)
{
  pmhandle_t *handle = ALPM_HANDLE(self);
  alpm_list_t *target;
  if (pylist_string_to_alpmlist(value, &target) == -1)
    return -1;

  alpm_option_set_noextracts(handle, target);
  return 0;
}

PyObject* option_get_ignorepkgs_alpm(PyObject *self, void* closure) {
  pmhandle_t *handle = ALPM_HANDLE(self);
  return alpmlist_to_pylist(alpm_option_get_ignorepkgs(handle), pyobject_from_string);
}

int option_set_ignorepkgs_alpm(PyObject *self, PyObject *value, void *closure)
{
  pmhandle_t *handle = ALPM_HANDLE(self);
  alpm_list_t *target;
  if (pylist_string_to_alpmlist(value, &target) == -1)
    return -1;

  alpm_option_set_ignorepkgs(handle, target);
  return 0;
}

PyObject* option_get_ignoregrps_alpm(PyObject *self, void* closure) {
  pmhandle_t *handle = ALPM_HANDLE(self);
  return alpmlist_to_pylist(alpm_option_get_ignoregrps(handle), pyobject_from_string);
}

int option_set_ignoregrps_alpm(PyObject *self, PyObject *value, void *closure)
{
  pmhandle_t *handle = ALPM_HANDLE(self);
  alpm_list_t *target;
  if (pylist_string_to_alpmlist(value, &target) == -1)
    return -1;

  alpm_option_set_ignoregrps(handle, target);
  return 0;
}

/* list options modifiers : add/remove */

PyObject* option_add_noupgrade_alpm(PyObject *self, PyObject *args)
{
  pmhandle_t *handle = ALPM_HANDLE(self);
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
  pmhandle_t *handle = ALPM_HANDLE(self);
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
  pmhandle_t *handle = ALPM_HANDLE(self);
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
  pmhandle_t *handle = ALPM_HANDLE(self);
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
  pmhandle_t *handle = ALPM_HANDLE(self);
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
  pmhandle_t *handle = ALPM_HANDLE(self);
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
  pmhandle_t *handle = ALPM_HANDLE(self);
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
  pmhandle_t *handle = ALPM_HANDLE(self);
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
  pmhandle_t *handle = ALPM_HANDLE(self);
  const char *str;

  if(!PyArg_ParseTuple(args, "s", &str)) {
    PyErr_SetString(PyExc_TypeError, "expecting a string argument");
    return NULL;
  }
  alpm_option_add_ignoregrp(handle, str);
  Py_RETURN_NONE;
}

PyObject* option_remove_ignoregrp_alpm(PyObject *self, PyObject *args)
{
  pmhandle_t *handle = ALPM_HANDLE(self);
  const char *str;

  if(!PyArg_ParseTuple(args, "s", &str)) {
    PyErr_SetString(PyExc_TypeError, "expecting a string argument");
    return NULL;
  }
  alpm_option_remove_ignoregrp(handle, str);
  Py_RETURN_NONE;
}

/** Callback options
 * We use Python callable objects as callbacks: they are
 * stored in static variables, and the reference count is
 * increased accordingly.
 *
 * These Python functions are wrapped into C functions 
 * that are passed to libalpm.
 */

static PyObject *pyalpm_logcb = NULL;
static PyObject *pyalpm_dlcb = NULL;
static PyObject *pyalpm_totaldlcb = NULL;
static PyObject *pyalpm_fetchcb = NULL;

/** callback wrappers */

static void pyalpm_logcb_wrapper(pmloglevel_t level, const char *fmt, va_list va_args) {
  char *log;
  vasprintf(&log, fmt, va_args);
  PyObject *result = PyObject_CallFunction(pyalpm_logcb, "s", log);
  if (!result) PyErr_Print();
  Py_CLEAR(result);
}

static void pyalpm_dlcb_wrapper(const char *filename, off_t xfered, off_t total) {
  PyObject *result;
  result = PyObject_CallFunction(pyalpm_dlcb, "sii", filename, xfered, total);
  if (!result) PyErr_Print();
  Py_CLEAR(result);
}

static void pyalpm_totaldlcb_wrapper(off_t total) {
  PyObject *result;
  result = PyObject_CallFunction(pyalpm_totaldlcb, "i", total);
  if (!result) PyErr_Print();
  Py_CLEAR(result);
}

static int pyalpm_fetchcb_wrapper(const char *url, const char *localpath, int force) {
  PyObject *result;
  result = PyObject_CallFunction(pyalpm_fetchcb, "ssi", url, localpath, force);
  if (!result) return -1;
  if (!PyLong_Check(result)) {
    return -1;
  } else {
    int overflow;
    long ret = PyLong_AsLongAndOverflow(result, &overflow);
    Py_DECREF(result);
    if (overflow != 0)
      return -1;
    else
      return ret;
  }
}

/** callback setters */

#define SET_CALLABLE_OR_NULL(dest, value) \
  if (value == Py_None) {     \
    Py_XDECREF(dest); \
    dest = NULL;      \
    return 0;         \
  } else if (PyCallable_Check(value)) { \
    Py_XDECREF(dest);  \
    Py_INCREF(value);  \
    dest = value;      \
    return 0;          \
  }

int pyalpm_option_set_logcb(PyObject *self, PyObject *value, void *closure)
{
  pmhandle_t *handle = ALPM_HANDLE(self);
  if (value == Py_None) {
    Py_CLEAR(pyalpm_logcb);
    alpm_option_set_logcb(handle, NULL);
  } else if (PyCallable_Check(value)) {
    Py_XDECREF(pyalpm_logcb);
    Py_INCREF(value);
    pyalpm_logcb = value;
    alpm_option_set_logcb(handle, pyalpm_logcb_wrapper);
  } else {
    PyErr_SetString(PyExc_TypeError, "logcb must be None or a function");
    return -1;
  }

  return 0;
}

int pyalpm_option_set_dlcb(PyObject *self, PyObject *value, void *closure)
{
  pmhandle_t *handle = ALPM_HANDLE(self);
  if (value == Py_None) {
    Py_CLEAR(pyalpm_dlcb);
    alpm_option_set_dlcb(handle, NULL);
    return 0;
  } else if (PyCallable_Check(value)) {
    Py_XDECREF(pyalpm_dlcb);
    Py_INCREF(value);
    pyalpm_dlcb = value;
    alpm_option_set_dlcb(handle, pyalpm_dlcb_wrapper);
    return 0;
  } else {
    PyErr_SetString(PyExc_TypeError, "dlcb must be None or a function");
    return -1;
  }
}

int pyalpm_option_set_totaldlcb(PyObject *self, PyObject *value, void *closure)
{
  pmhandle_t *handle = ALPM_HANDLE(self);
  if (value == Py_None) {
    Py_CLEAR(pyalpm_totaldlcb);
    alpm_option_set_totaldlcb(handle, NULL);
    return 0;
  } else if (PyCallable_Check(value)) {
    Py_XDECREF(pyalpm_totaldlcb);
    Py_INCREF(value);
    pyalpm_totaldlcb = value;
    alpm_option_set_totaldlcb(handle, pyalpm_totaldlcb_wrapper);
    return 0;
  } else {
    PyErr_SetString(PyExc_TypeError, "totaldlcb must be None or a function");
    return -1;
  }
}

int pyalpm_option_set_fetchcb(PyObject *self, PyObject *value, void *closure)
{
  pmhandle_t *handle = ALPM_HANDLE(self);
  if (value == Py_None) {
    Py_CLEAR(pyalpm_fetchcb);
    alpm_option_set_fetchcb(handle, NULL);
    return 0;
  } else if (PyCallable_Check(value)) {
    Py_XDECREF(pyalpm_fetchcb);
    Py_INCREF(value);
    pyalpm_fetchcb = value;
    alpm_option_set_fetchcb(handle, pyalpm_fetchcb_wrapper);
    return 0;
  } else {
    PyErr_SetString(PyExc_TypeError, "fetchcb must be None or a function");
    return -1;
  }
}

/** callback getters */

#define RETURN_OR_NONE(obj) if (obj == NULL) { \
    Py_RETURN_NONE; \
  } else {          \
    Py_INCREF(obj); \
    return obj;     \
  }

PyObject* pyalpm_option_get_logcb(PyObject *self, void *closure)
{
  RETURN_OR_NONE(pyalpm_logcb);
}

PyObject* pyalpm_option_get_dlcb(PyObject *self, void *closure)
{
  RETURN_OR_NONE(pyalpm_dlcb);
}

PyObject* pyalpm_option_get_totaldlcb(PyObject *self, void *closure)
{
  RETURN_OR_NONE(pyalpm_totaldlcb);
}

PyObject* pyalpm_option_get_fetchcb(PyObject *self, void *closure)
{
  RETURN_OR_NONE(pyalpm_fetchcb);
}

/* vim: set ts=2 sw=2 et: */
