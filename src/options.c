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

extern int init;
static PyObject *alpm_error;

/*
The following functions take a string as argument(*_set_*)
while other (*_get_*) return a string
*/
PyObject * option_get_root_alpm(PyObject *self, PyObject *args)
{
  const char *str = alpm_option_get_root();
  
  if(str == NULL)
  {
    PyErr_SetString(alpm_error, "failed getting root path");
    return NULL;
  }
  else
  {
    return Py_BuildValue("s", str);
  }
}

PyObject * option_set_root_alpm(PyObject *self, PyObject *args)
{
  const char *path;

  if(!PyArg_ParseTuple(args, "s", &path))
  {
    PyErr_SetString(alpm_error, "error in the args");
    return NULL;
  }
  else
  {
    if(alpm_option_set_root(path) == -1)
    {
      PyErr_SetString(alpm_error, "failed setting root path");
      return NULL;
    }
    else
    {
      return Py_None;
    }
  }
}

PyObject * option_set_dbpath_alpm(PyObject *self, PyObject *args)
{
  const char *path;
  if(!PyArg_ParseTuple(args, "s", &path))
  {
    PyErr_SetString(alpm_error, "error in the args");
    return NULL;
  }
  else
  {
    if(alpm_option_set_dbpath(path) == -1)
    {
      PyErr_SetString(alpm_error, "failed setting dbpath");
      return NULL;
    }
    else
    {
      return Py_None;
    }
  }
}

PyObject * option_get_dbpath_alpm(PyObject *self, PyObject *args)
{
  const char *str = alpm_option_get_dbpath();
  
  if(str == NULL)
  {
    PyErr_SetString(alpm_error, "failed getting dbpath.");
    return NULL;
  }
  else
  {
    return Py_BuildValue("s", str);
  }
}

PyObject * option_set_logfile_alpm(PyObject *self, PyObject *args)
{
  const char *path;
  if(!PyArg_ParseTuple(args, "s", &path))
  {
    PyErr_SetString(alpm_error, "error in the args");
    return NULL;
  }
  else
  {
    if(alpm_option_set_logfile(path) == -1)
    {
      PyErr_SetString(alpm_error, "failed setting logfile");
      return NULL;
    }
    else
    {
      return Py_None;
    }
  }
}

PyObject * option_get_logfile_alpm(PyObject *self, PyObject *args)
{
  const char *str = alpm_option_get_logfile();
  
  if(str == NULL)
  {
    PyErr_SetString(alpm_error, "failed getting logfile.");
    return NULL;
  }
  else
  {
    return Py_BuildValue("s", str);
  }
}

/*
receives and returns an int type
1 = enabled
0 = disabled
*/
PyObject * option_get_usesyslog_alpm(PyObject *self, PyObject *args)
{
  unsigned short str = alpm_option_get_usesyslog();
  
  if(str == -1)
  {
    PyErr_SetString(alpm_error, "failed getting usesyslog");
    return NULL;
  }
  else
  {
    return Py_BuildValue("i", str);
  }
}

PyObject * option_set_usesyslog_alpm(PyObject *self, PyObject *args)
{
  int value;
  if(!PyArg_ParseTuple(args, "i", &value))
  {
    PyErr_SetString(alpm_error, "wrong arguments");
    return NULL;
  }
  else
  {
    alpm_option_set_usesyslog(value);
    return Py_None;
  }
}

/*write only function*/

PyObject * option_set_usedelta_alpm(PyObject *self, PyObject *args)
{
  int value;
  if(!PyArg_ParseTuple(args, "i", &value))
  {
    PyErr_SetString(alpm_error, "wrong arguments");
    return NULL;
  }
  else
  {
    alpm_option_set_usedelta(value);
    return Py_None;
  }
}

/*read-only functions*/

PyObject * option_get_lockfile_alpm(PyObject *self, PyObject *args)
{
  const char *str = NULL;
  str = alpm_option_get_lockfile();
  
  if(str == NULL)
  {
    PyErr_SetString(alpm_error, "failed getting lockfile");
    return NULL;
  }
  else
  {
    return Py_BuildValue("s", str);
  }
}

PyObject * option_set_noupgrades_alpm(PyObject *self, PyObject *args)
{
  alpm_list_t *target, *adtmp;
  PyObject *tmp;
  
  if(!PyArg_ParseTuple(args, "O", &tmp))
  {
    PyErr_SetString(alpm_error, "error in the args.");
    return NULL;
  }
  else
  {
    target = tuple_alpm_list_t(tmp);
    alpm_option_set_noupgrades(target);
    return Py_None;
  }
}

PyObject * option_get_noupgrades_alpm(PyObject *self, PyObject *args)
{
  PyObject *ret;
  alpm_list_t *output;
  
  output = alpm_option_get_noupgrades();
  
  ret = alpm_list_t_tuple(output);
  
  return ret;
}

PyObject * option_add_noupgrade_alpm(PyObject *self, PyObject *args)
{
  const char *str;
  
  if(!PyArg_ParseTuple(args, "s", &str))
  {
    PyErr_SetString(alpm_error, "wrong arguments");
    return NULL;
  }
  else
  {
    if(check_init() == 1)
    {
      alpm_option_add_noupgrade(str);
      return Py_None;
    }
    else
    {
      PyErr_SetString(alpm_error, "pyalpm not initialized");
      return NULL;
    }
  }
}

PyObject * option_remove_noupgrade_alpm(PyObject *self, PyObject *args)
{
  const char *str;
  
  if(!PyArg_ParseTuple(args, "s", &str))
  {
    PyErr_SetString(alpm_error, "wrong arguments");
    return NULL;
  }
  else
  {
    if(check_init() == 1)
    {
      alpm_option_remove_noupgrade(str);
      return Py_None;
    }
    else
    {
      PyErr_SetString(alpm_error, "pyalpm not initialized");
      return NULL;
    }
  }
}

PyObject * option_add_cachedir_alpm(PyObject *self, PyObject *args)
{
  const char *str;
  
  if(!PyArg_ParseTuple(args, "s", &str))
  {
    PyErr_SetString(alpm_error, "wrong arguments");
    return NULL;
  }
  else
  {
    if(check_init() == 1)
    {
      alpm_option_add_cachedir(str);
      return Py_None;
    }
    else
    {
      PyErr_SetString(alpm_error, "pyalpm not initialized");
      return NULL;
    }
  }
}

PyObject * option_remove_cachedir_alpm(PyObject *self, PyObject *args)
{
  const char *str;
  
  if(!PyArg_ParseTuple(args, "s", &str))
  {
    PyErr_SetString(alpm_error, "wrong arguments");
    return NULL;
  }
  else
  {
    if(check_init() == 1)
    {
      alpm_option_remove_cachedir(str);
      return Py_None;
    }
    else
    {
      PyErr_SetString(alpm_error, "pyalpm not initialized");
      return NULL;
    }
  }
}

PyObject * option_add_noextract_alpm(PyObject *self, PyObject *args)
{
  const char *str;
  
  if(!PyArg_ParseTuple(args, "s", &str))
  {
    PyErr_SetString(alpm_error, "wrong arguments");
    return NULL;
  }
  else
  {
    if(check_init() == 1)
    {
      alpm_option_add_noextract(str);
      return Py_None;
    }
    else
    {
      PyErr_SetString(alpm_error, "pyalpm not initialized");
      return NULL;
    }
  }
}

PyObject * option_remove_noextract_alpm(PyObject *self, PyObject *args)
{
  const char *str;
  
  if(!PyArg_ParseTuple(args, "s", &str))
  {
    PyErr_SetString(alpm_error, "wrong arguments");
    return NULL;
  }
  else
  {
    if(check_init() == 1)
    {
      alpm_option_remove_noextract(str);
      return Py_None;
    }
    else
    {
      PyErr_SetString(alpm_error, "pyalpm not initialized");
      return NULL;
    }
  }
}

PyObject * option_add_ignorepkg_alpm(PyObject *self, PyObject *args)
{
  const char *str;
  
  if(!PyArg_ParseTuple(args, "s", &str))
  {
    PyErr_SetString(alpm_error, "wrong arguments");
    return NULL;
  }
  else
  {
    if(check_init() == 1)
    {
      alpm_option_add_ignorepkg(str);
      return Py_None;
    }
    else
    {
      PyErr_SetString(alpm_error, "pyalpm not initialized");
      return NULL;
    }
  }
}

PyObject * option_remove_ignorepkg_alpm(PyObject *self, PyObject *args)
{
  const char *str;
  
  if(!PyArg_ParseTuple(args, "s", &str))
  {
    PyErr_SetString(alpm_error, "wrong arguments");
    return NULL;
  }
  else
  {
    if(check_init() == 1)
    {
      alpm_option_remove_ignorepkg(str);
      return Py_None;
    }
    else
    {
      PyErr_SetString(alpm_error, "pyalpm not initialized");
      return NULL;
    }
  }
}

PyObject * option_add_ignoregrps_alpm(PyObject *self, PyObject *args)
{
  const char *str;
  
  if(!PyArg_ParseTuple(args, "s", &str))
  {
    PyErr_SetString(alpm_error, "wrong arguments");
    return NULL;
  }
  else
  {
    if(check_init() == 1)
    {
      alpm_option_add_ignoregrp(str);
      return Py_None;
    }
    else
    {
      PyErr_SetString(alpm_error, "pyalpm not initialized");
      return NULL;
    }
  }
}

PyObject * option_remove_ignoregrps_alpm(PyObject *self, PyObject *args)
{
  const char *str;
  
  if(!PyArg_ParseTuple(args, "s", &str))
  {
    PyErr_SetString(alpm_error, "wrong arguments");
    return NULL;
  }
  else
  {
    if(check_init() == 1)
    {
      alpm_option_remove_ignoregrp(str);
      return Py_None;
    }
    else
    {
      PyErr_SetString(alpm_error, "pyalpm not initialized");
      return NULL;
    }
  }
}

static PyMethodDef pyalpm_options_methods[] = {
  {"getroot", option_get_root_alpm, METH_VARARGS, "gets the root path."},
  {"setroot", option_set_root_alpm, METH_VARARGS, "sets the root path."},
  {"getdbpath", option_get_dbpath_alpm, METH_VARARGS, "gets the dbpath."},
  {"setdbpath", option_set_dbpath_alpm, METH_VARARGS, "sets the dbpath."},
  {"getlogfile", option_get_logfile_alpm, METH_VARARGS, "gets the logfile."},
  {"setlogfile", option_set_logfile_alpm, METH_VARARGS, "sets the logfile."},
  {"getlockfile", option_get_lockfile_alpm, METH_VARARGS, "gets the lockfile."},
  {"getusesyslog", option_get_usesyslog_alpm, METH_VARARGS, "gets usesyslog value."},
  {"setusesyslog", option_set_usesyslog_alpm, METH_VARARGS, "sets usesyslog value."},
  {"setusedelta", option_set_usedelta_alpm, METH_VARARGS, "sets usedelta value."},
  {"setnoupgrades", option_set_noupgrades_alpm, METH_VARARGS, "sets noupgrades."},
  {"getnoupgrades", option_get_noupgrades_alpm, METH_VARARGS, "gets noupgrades."},
  {"addnoupgrade", option_add_noupgrade_alpm, METH_VARARGS, "add a noupgrade package."},
  {"removenoupgrade", option_remove_noupgrade_alpm, METH_VARARGS, "removes a noupgrade package."},
  {"addcachedir", option_add_cachedir_alpm, METH_VARARGS, "adds a cachedir."},
  {"removecachedir", option_remove_cachedir_alpm, METH_VARARGS, "removes a cachedir."},
  {"addnoextract", option_add_noextract_alpm, METH_VARARGS, "add a noextract package."},
  {"removenoextract", option_remove_noextract_alpm, METH_VARARGS, "remove a noextract package."},
  {"addignorepkg", option_add_ignorepkg_alpm, METH_VARARGS, "add an ignorepkg."},
  {"removeignorepkg", option_remove_ignorepkg_alpm, METH_VARARGS, "remove an ignorepkg."},
  {"addignoregrps", option_add_ignoregrps_alpm, METH_VARARGS, "add an ignoregrps."},
  {"removeignoregrps", option_remove_ignoregrps_alpm, METH_VARARGS, "remove an ignoregrps."},
  {NULL, NULL, 0, NULL},
};

static struct PyModuleDef pyalpm_options_def = {
  PyModuleDef_HEAD_INIT,
  "_alpmoptions",
  "This module handles options getting and setting for pyalpm",
  -1,
  pyalpm_options_methods
};

PyMODINIT_FUNC PyInit__alpmoptions()
{
  PyObject* m = PyModule_Create(&pyalpm_options_def);

  alpm_error = PyErr_NewException("_alpmoptions.error", NULL, NULL);
  return m;
}

/* vim: set ts=2 sw=2 et: */
