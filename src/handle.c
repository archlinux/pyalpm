/**
 * handle.c : wrapper class around alpm_handle_t
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
#include "handle.h"
#include "package.h"
#include "db.h"
#include "options.h"
#include "util.h"

PyTypeObject AlpmHandleType;

static PyObject *pyalpm_handle_from_pmhandle(void* data) {
  alpm_handle_t *handle = (alpm_handle_t*)data;
  AlpmHandle *self;
  self = (AlpmHandle*)AlpmHandleType.tp_alloc(&AlpmHandleType, 0);
  if (self == NULL) {
    PyErr_SetString(PyExc_RuntimeError, "unable to create pyalpm.Handle object");
    return NULL;
  }

  self->c_data = handle;
  return (PyObject *)self;
}

/*pyalpm functions*/
PyObject* pyalpm_initialize(PyTypeObject *subtype, PyObject *args, PyObject *kwargs)
{
  const char *root;
  const char *dbpath;
  alpm_handle_t *h;
  enum _alpm_errno_t errcode = 0;
  if(!PyArg_ParseTuple(args, "ss", &root, &dbpath)) {
    return NULL;
  }

  h = alpm_initialize(root, dbpath, &errcode);
  if (h) {
    set_init(1);
    return pyalpm_handle_from_pmhandle((void*)h);
  } else {
    RET_ERR("could not create a libalpm handle", errcode, NULL);
  }
}

PyObject* pyalpm_release(PyObject *self, PyObject *args)
{
  AlpmHandle *pyhandle;
  if(!PyArg_ParseTuple(args, "O!", &AlpmHandleType, &pyhandle))
    return NULL;

  alpm_release(pyhandle->c_data);
  pyhandle->c_data = NULL;
  Py_RETURN_NONE;
}

/* Database getters/setters */

static PyObject* pyalpm_get_localdb(PyObject *self, PyObject *dummy) {
  alpm_handle_t *handle = ALPM_HANDLE(self);
  return pyalpm_db_from_pmdb(alpm_option_get_localdb(handle));
}

static PyObject* pyalpm_get_syncdbs(PyObject *self, PyObject *dummy) {
  alpm_handle_t *handle = ALPM_HANDLE(self);
  return alpmlist_to_pylist(alpm_option_get_syncdbs(handle),
			    pyalpm_db_from_pmdb);
}

static PyObject* pyalpm_register_syncdb(PyObject *self, PyObject *args) {
  alpm_handle_t *handle = ALPM_HANDLE(self);
  const char *dbname;
  alpm_db_t *result;
  int pgp_level;

  if (!PyArg_ParseTuple(args, "si", &dbname, &pgp_level)) {
    PyErr_Format(PyExc_TypeError, "%s() takes a string and an integer", __func__);
    return NULL;
  }

  result = alpm_db_register_sync(handle, dbname, pgp_level);
  if (! result) {
    PyErr_Format(alpm_error, "unable to register sync database %s", dbname);
    return NULL;
  }

  return pyalpm_db_from_pmdb(result);
}

static PyObject* pyalpm_set_pkgreason(PyObject* self, PyObject* args) {
  alpm_handle_t *handle = ALPM_HANDLE(self);
  alpm_pkg_t *pmpkg = NULL;
  PyObject *pkg = NULL;
  int reason, ret;
  if (!PyArg_ParseTuple(args, "sO!", &AlpmPackageType, &pkg, &reason)) {
    PyErr_SetString(PyExc_TypeError, "expected arguments (pkg, int)");
    return NULL;
  }
  pmpkg = ALPM_PACKAGE(pkg);
  ret = alpm_db_set_pkgreason(handle, pmpkg, reason);

  if (ret == -1) RET_ERR("failed setting install reason", alpm_errno(handle), NULL);
  Py_RETURN_NONE;
}

struct PyGetSetDef pyalpm_handle_getset[] = {
  /** filepaths */
  { "root",
    (getter)option_get_root_alpm,
    NULL,
    "system root directory", NULL } ,
  { "dbpath",
    (getter)option_get_dbpath_alpm,
    NULL,
    "alpm database directory", NULL } ,
  { "logfile",
    (getter)option_get_logfile_alpm,
    (setter)option_set_logfile_alpm,
    "alpm logfile path", NULL } ,
  { "lockfile",
    (getter)option_get_lockfile_alpm,
    NULL,
    "alpm lockfile path", NULL } ,

  /** strings */
  { "arch",
    (getter)option_get_arch_alpm,
    (setter)option_set_arch_alpm,
    "Target archichecture", NULL } ,

  /** booleans */
  { "usesyslog",
    (getter)option_get_usesyslog_alpm,
    (setter)option_set_usesyslog_alpm,
    "use syslog (an integer, 0 = False, 1 = True)", NULL } ,
  { "usedelta",
    (getter)option_get_usedelta_alpm,
    (setter)option_set_usedelta_alpm,
    "use deltas (an integer, 0 = False, 1 = True)", NULL } ,
  { "checkspace",
    (getter)option_get_checkspace_alpm,
    (setter)option_set_checkspace_alpm,
    "check disk space before transactions (an integer, 0 = False, 1 = True)", NULL } ,

  /** lists */
  { "cachedirs",
    (getter)option_get_cachedirs_alpm,
    (setter)option_set_cachedirs_alpm,
    "list of package cache directories", NULL },
  { "noupgrades",
    (getter)option_get_noupgrades_alpm,
    (setter)option_set_noupgrades_alpm,
    "list of ...", NULL },
  { "noextracts",
    (getter)option_get_noextracts_alpm,
    (setter)option_set_noextracts_alpm,
    "list of ...", NULL },
  { "ignorepkgs",
    (getter)option_get_ignorepkgs_alpm,
    (setter)option_set_ignorepkgs_alpm,
    "list of ignored packages", NULL },
  { "ignoregrps",
    (getter)option_get_ignoregrps_alpm,
    (setter)option_set_ignoregrps_alpm,
    "list of ignored groups", NULL },

  /** callbacks */
  { "logcb",
    (getter)pyalpm_option_get_logcb,
    (setter)pyalpm_option_set_logcb,
    "logging callback, with arguments (loglevel, format string, tuple)", NULL },
  { "dlcb",
    (getter)pyalpm_option_get_dlcb,
    (setter)pyalpm_option_set_dlcb,
    "download status callback (a function)\n"
    "args: filename    :: str\n"
    "      transferred :: int\n"
    "      total       :: int\n", NULL },
  { "totaldlcb",
    (getter)pyalpm_option_get_totaldlcb,
    (setter)pyalpm_option_set_totaldlcb,
    "total download size callback: totaldlcb(total_size)", NULL },
  { "fetchcb",
    (getter)pyalpm_option_get_fetchcb,
    (setter)pyalpm_option_set_fetchcb,
    "download function\n"
    "args: url              :: string\n"
    "      destination path :: string\n"
    "      overwrite        :: bool\n"
    "returns: 0 on success, 1 if file exists, -1 on error", NULL },

  /** terminator */
  { NULL }
};

static PyMethodDef pyalpm_handle_methods[] = {
  /* Transaction initialization */
  {"init_transaction",    (PyCFunction)pyalpm_trans_init, METH_VARARGS | METH_KEYWORDS,
    "Initializes a transaction.\n"
    "Arguments:\n"
    "  nodeps, force, nosave, nodepversion, cascade, recurse,\n"
    "  dbonly, alldeps, downloadonly, noscriptlet, noconflicts,\n"
    "  needed, allexplicit, inneeded, recurseall, nolock\n"
    "    -- the transaction options (booleans)\n"
    "  event_callback -- a function called when an event occurs\n"
    "    -- args: (event ID, event string, (object 1, object 2))\n"
    "  conv_callback -- a function called to get user input\n"
    "  progress_callback -- a function called to indicate progress\n"
    "    -- args: (target name, percentage, number of targets, target number)\n"
  },

  /* Database members */
  {"register_syncdb", pyalpm_register_syncdb, METH_VARARGS,
   "registers the database with the given name\n"
   "returns the new database on success"},
  {"get_localdb", pyalpm_get_localdb, METH_NOARGS, "returns an object representing the local DB"},
  {"get_syncdbs", pyalpm_get_syncdbs, METH_NOARGS, "returns a list of sync DBs"},
  {"set_pkgreason", pyalpm_set_pkgreason, METH_VARARGS,
    "set install reason for a package (PKG_REASON_DEPEND, PKG_REASON_EXPLICIT)\n"},

  /* Option modifiers */
  {"add_noupgrade", option_add_noupgrade_alpm, METH_VARARGS, "add a noupgrade package."},
  {"remove_noupgrade", option_remove_noupgrade_alpm, METH_VARARGS, "removes a noupgrade package."},

  {"add_cachedir", option_add_cachedir_alpm, METH_VARARGS, "adds a cachedir."},
  {"remove_cachedir", option_remove_cachedir_alpm, METH_VARARGS, "removes a cachedir."},

  {"add_noextract", option_add_noextract_alpm, METH_VARARGS, "add a noextract package."},
  {"remove_noextract", option_remove_noextract_alpm, METH_VARARGS, "remove a noextract package."},

  {"add_ignorepkg", option_add_ignorepkg_alpm, METH_VARARGS, "add an ignorepkg."},
  {"remove_ignorepkg", option_remove_ignorepkg_alpm, METH_VARARGS, "remove an ignorepkg."},

  {"add_ignoregrp", option_add_ignoregrp_alpm, METH_VARARGS, "add an ignoregrp."},
  {"remove_ignoregrp", option_remove_ignoregrp_alpm, METH_VARARGS, "remove an ignoregrp."},
  {NULL, NULL, 0, NULL},
};

PyTypeObject AlpmHandleType = {
  PyVarObject_HEAD_INIT(NULL, 0)
  "alpm.Handle",       /*tp_name*/
  sizeof(AlpmHandle),  /*tp_basicsize*/
  0,                   /*tp_itemsize*/
  .tp_flags = Py_TPFLAGS_DEFAULT,
  .tp_doc = "An object wrapping a libalpm handle. Arguments: root path, DB path.",
  .tp_methods = pyalpm_handle_methods,
  .tp_getset = pyalpm_handle_getset,
  .tp_new = pyalpm_initialize,
};

/** Initializes Handle class in module */
int init_pyalpm_handle(PyObject *module) {
  PyObject *type;
  if (PyType_Ready(&AlpmHandleType) < 0)
    return -1;
  type = (PyObject*)&AlpmHandleType;
  Py_INCREF(type);
  PyModule_AddObject(module, "Handle", type);
  return 0;
}

/* vim: set ts=2 sw=2 et: */
