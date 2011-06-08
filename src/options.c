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
#include "options.h"
#include "util.h"

#define CHECK_ALPM_INIT(value) if (check_init() != 1) { \
  RET_ERR("pyalpm not initialized", value); \
  }

/**
 * Path options getters/setters
 */
PyObject * option_get_root_alpm(PyObject *self, void *closure)
{
  const char *str = alpm_option_get_root();
  if (!str) RET_ERR("failed getting root path", NULL);
  return Py_BuildValue("s", str);
}

PyObject* option_get_dbpath_alpm(PyObject *self, void *closure)
{
  const char *str = alpm_option_get_dbpath();

  if(str == NULL)
    RET_ERR("failed getting dbpath.", NULL);
  return Py_BuildValue("s", str);
}

int option_set_logfile_alpm(PyObject *self, PyObject *value, void* closure)
{
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

  CHECK_ALPM_INIT(-1);

  int ok = alpm_option_set_logfile(path);
  free(path);
  if (ok == -1) RET_ERR("failed setting logfile path", -1);

  return 0;
}

PyObject * option_get_logfile_alpm(PyObject *self, void* closure)
{
  const char *str = alpm_option_get_logfile();

  if(str == NULL)
  {
    RET_ERR("failed getting logfile.", NULL);
  }
  else
  {
    return Py_BuildValue("s", str);
  }
}

PyObject* option_get_lockfile_alpm(PyObject *self, void *closure)
{
  const char *str = alpm_option_get_lockfile();

  if(str == NULL)
  {
    RET_ERR("failed getting lockfile", NULL);
  }
  else
    return PyUnicode_FromString(str);
}

/**
 * String options
 */

int option_set_arch_alpm(PyObject *self, PyObject *value, void* closure)
{
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

  CHECK_ALPM_INIT(-1);
  alpm_option_set_arch(string);
  free(string);
  return 0;
}

PyObject* option_get_arch_alpm(PyObject *self, void* closure) {
  CHECK_ALPM_INIT(NULL);
  const char *str = alpm_option_get_arch();

  if(str == NULL) {
    RET_ERR("failed getting arch", NULL);
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
  int ret = alpm_option_get_usesyslog();

  if(ret == -1)
  {
    RET_ERR("failed getting usesyslog", NULL);
  }
  else
    return PyLong_FromLong(ret);
}

int option_set_usesyslog_alpm(PyObject *self, PyObject *value, void* closure)
{
  if(!PyLong_Check(value))
  {
    PyErr_SetString(PyExc_TypeError, "wrong arguments");
    return -1;
  }

  CHECK_ALPM_INIT(-1);
  alpm_option_set_usesyslog(PyLong_AsLong(value));
  return 0;
}

PyObject* option_get_usedelta_alpm(PyObject *self, void* closure) {
  int ret = alpm_option_get_usedelta();
  if (ret == -1) {
    RET_ERR("failed getting usedelta", NULL);
  } else
    return PyLong_FromLong(ret);
}

int option_set_usedelta_alpm(PyObject *self, PyObject *value, void* closure)
{
  if(!PyLong_Check(value))
  {
    PyErr_SetString(PyExc_TypeError, "wrong arguments");
    return -1;
  }

  CHECK_ALPM_INIT(-1);
  alpm_option_set_usedelta(PyLong_AsLong(value));
  return 0;
}

PyObject* option_get_checkspace_alpm(PyObject *self, void* closure) {
  int ret = alpm_option_get_checkspace();
  if (ret == -1) {
    RET_ERR("failed getting checkspace", NULL);
  } else
    return PyLong_FromLong(ret);
}

int option_set_checkspace_alpm(PyObject *self, PyObject *value, void* closure)
{
  if(!PyLong_Check(value))
  {
    PyErr_SetString(PyExc_TypeError, "wrong arguments");
    return -1;
  }
  CHECK_ALPM_INIT(-1);
  alpm_option_set_checkspace(PyLong_AsLong(value));
  return 0;
}

/**
 * List options
 * in addition to getters/setters, these have add/remove methods
 */

PyObject* option_get_cachedirs_alpm(PyObject *self, void* closure) {
  return alpmlist_to_pylist(alpm_option_get_cachedirs(), pyobject_from_string);
}

int option_set_cachedirs_alpm(PyObject *self, PyObject *value, void *closure)
{
  alpm_list_t *target;
  if (pylist_string_to_alpmlist(value, &target) == -1)
    return -1;

  alpm_option_set_cachedirs(target);
  return 0;
}

PyObject* option_get_noupgrades_alpm(PyObject *self, void* closure) {
  return alpmlist_to_pylist(alpm_option_get_noupgrades(), pyobject_from_string);
}

int option_set_noupgrades_alpm(PyObject *self, PyObject *value, void *closure)
{
  alpm_list_t *target;
  if (pylist_string_to_alpmlist(value, &target) == -1)
    return -1;

  alpm_option_set_noupgrades(target);
  return 0;
}

PyObject* option_get_noextracts_alpm(PyObject *self, void* closure) {
  return alpmlist_to_pylist(alpm_option_get_noextracts(), pyobject_from_string);
}

int option_set_noextracts_alpm(PyObject *self, PyObject *value, void *closure)
{
  alpm_list_t *target;
  if (pylist_string_to_alpmlist(value, &target) == -1)
    return -1;

  alpm_option_set_noextracts(target);
  return 0;
}

PyObject* option_get_ignorepkgs_alpm(PyObject *self, void* closure) {
  return alpmlist_to_pylist(alpm_option_get_ignorepkgs(), pyobject_from_string);
}

int option_set_ignorepkgs_alpm(PyObject *self, PyObject *value, void *closure)
{
  alpm_list_t *target;
  if (pylist_string_to_alpmlist(value, &target) == -1)
    return -1;

  alpm_option_set_ignorepkgs(target);
  return 0;
}

PyObject* option_get_ignoregrps_alpm(PyObject *self, void* closure) {
  return alpmlist_to_pylist(alpm_option_get_ignoregrps(), pyobject_from_string);
}

int option_set_ignoregrps_alpm(PyObject *self, PyObject *value, void *closure)
{
  alpm_list_t *target;
  if (pylist_string_to_alpmlist(value, &target) == -1)
    return -1;

  alpm_option_set_ignoregrps(target);
  return 0;
}

/* list options modifiers : add/remove */

PyObject* option_add_noupgrade_alpm(PyObject *self, PyObject *args)
{
  const char *str;

  if(!PyArg_ParseTuple(args, "s", &str)) {
    PyErr_SetString(PyExc_TypeError, "expecting a string argument");
    return NULL;
  }
  CHECK_ALPM_INIT(NULL);
  alpm_option_add_noupgrade(str);
  Py_RETURN_NONE;
}

PyObject* option_remove_noupgrade_alpm(PyObject *self, PyObject *args)
{
  const char *str;

  if(!PyArg_ParseTuple(args, "s", &str)) {
    PyErr_SetString(PyExc_TypeError, "expecting a string argument");
    return NULL;
  }
  CHECK_ALPM_INIT(NULL);
  alpm_option_remove_noupgrade(str);
  Py_RETURN_NONE;
}

PyObject* option_add_cachedir_alpm(PyObject *self, PyObject *args)
{
  const char *str;

  if(!PyArg_ParseTuple(args, "s", &str)) {
    PyErr_SetString(PyExc_TypeError, "expecting a string argument");
    return NULL;
  }
  CHECK_ALPM_INIT(NULL);
  alpm_option_add_cachedir(str);
  Py_RETURN_NONE;
}

PyObject* option_remove_cachedir_alpm(PyObject *self, PyObject *args)
{
  const char *str;

  if(!PyArg_ParseTuple(args, "s", &str)) {
    PyErr_SetString(PyExc_TypeError, "expecting a string argument");
    return NULL;
  }
  CHECK_ALPM_INIT(NULL);
  alpm_option_remove_cachedir(str);
  Py_RETURN_NONE;
}

PyObject* option_add_noextract_alpm(PyObject *self, PyObject *args)
{
  const char *str;

  if(!PyArg_ParseTuple(args, "s", &str)) {
    PyErr_SetString(PyExc_TypeError, "expecting a string argument");
    return NULL;
  }
  CHECK_ALPM_INIT(NULL);
  alpm_option_add_noextract(str);
  Py_RETURN_NONE;
}

PyObject* option_remove_noextract_alpm(PyObject *self, PyObject *args)
{
  const char *str;

  if(!PyArg_ParseTuple(args, "s", &str)) {
    PyErr_SetString(PyExc_TypeError, "expecting a string argument");
    return NULL;
  }
  CHECK_ALPM_INIT(NULL);
  alpm_option_remove_noextract(str);
  Py_RETURN_NONE;
}

PyObject* option_add_ignorepkg_alpm(PyObject *self, PyObject *args)
{
  const char *str;

  if(!PyArg_ParseTuple(args, "s", &str)) {
    PyErr_SetString(PyExc_TypeError, "expecting a string argument");
    return NULL;
  }
  CHECK_ALPM_INIT(NULL);
  alpm_option_add_ignorepkg(str);
  Py_RETURN_NONE;
}

PyObject* option_remove_ignorepkg_alpm(PyObject *self, PyObject *args)
{
  const char *str;

  if(!PyArg_ParseTuple(args, "s", &str)) {
    PyErr_SetString(PyExc_TypeError, "expecting a string argument");
    return NULL;
  }
  CHECK_ALPM_INIT(NULL);
  alpm_option_remove_ignorepkg(str);
  Py_RETURN_NONE;
}

PyObject* option_add_ignoregrp_alpm(PyObject *self, PyObject *args)
{
  const char *str;

  if(!PyArg_ParseTuple(args, "s", &str)) {
    PyErr_SetString(PyExc_TypeError, "expecting a string argument");
    return NULL;
  }
  CHECK_ALPM_INIT(NULL);
  alpm_option_add_ignoregrp(str);
  Py_RETURN_NONE;
}

PyObject* option_remove_ignoregrp_alpm(PyObject *self, PyObject *args)
{
  const char *str;

  if(!PyArg_ParseTuple(args, "s", &str)) {
    PyErr_SetString(PyExc_TypeError, "expecting a string argument");
    return NULL;
  }
  CHECK_ALPM_INIT(NULL);
  alpm_option_remove_ignoregrp(str);
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
  CHECK_ALPM_INIT(-1);
  if (value == Py_None) {
    Py_CLEAR(pyalpm_logcb);
    alpm_option_set_logcb(NULL);
  } else if (PyCallable_Check(value)) {
    Py_XDECREF(pyalpm_logcb);
    Py_INCREF(value);
    pyalpm_logcb = value;
    alpm_option_set_logcb(pyalpm_logcb_wrapper);
  } else {
    PyErr_SetString(PyExc_TypeError, "logcb must be None or a function");
    return -1;
  }

  return 0;
}

int pyalpm_option_set_dlcb(PyObject *self, PyObject *value, void *closure)
{
  CHECK_ALPM_INIT(-1);
  if (value == Py_None) {
    Py_CLEAR(pyalpm_dlcb);
    alpm_option_set_dlcb(NULL);
    return 0;
  } else if (PyCallable_Check(value)) {
    Py_XDECREF(pyalpm_dlcb);
    Py_INCREF(value);
    pyalpm_dlcb = value;
    alpm_option_set_dlcb(pyalpm_dlcb_wrapper);
    return 0;
  } else {
    PyErr_SetString(PyExc_TypeError, "dlcb must be None or a function");
    return -1;
  }
}

int pyalpm_option_set_totaldlcb(PyObject *self, PyObject *value, void *closure)
{
  CHECK_ALPM_INIT(-1);
  if (value == Py_None) {
    Py_CLEAR(pyalpm_totaldlcb);
    alpm_option_set_totaldlcb(NULL);
    return 0;
  } else if (PyCallable_Check(value)) {
    Py_XDECREF(pyalpm_totaldlcb);
    Py_INCREF(value);
    pyalpm_totaldlcb = value;
    alpm_option_set_totaldlcb(pyalpm_totaldlcb_wrapper);
    return 0;
  } else {
    PyErr_SetString(PyExc_TypeError, "totaldlcb must be None or a function");
    return -1;
  }
}

int pyalpm_option_set_fetchcb(PyObject *self, PyObject *value, void *closure)
{
  CHECK_ALPM_INIT(-1);
  if (value == Py_None) {
    Py_CLEAR(pyalpm_fetchcb);
    alpm_option_set_fetchcb(NULL);
    return 0;
  } else if (PyCallable_Check(value)) {
    Py_XDECREF(pyalpm_fetchcb);
    Py_INCREF(value);
    pyalpm_fetchcb = value;
    alpm_option_set_fetchcb(pyalpm_fetchcb_wrapper);
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
