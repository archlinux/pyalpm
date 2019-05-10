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

#include <pyconfig.h>
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
  /* memset(self->py_callbacks, 0, N_CALLBACKS * sizeof(PyObject*)); */
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
    return pyalpm_handle_from_pmhandle((void*)h);
  } else {
    RET_ERR("could not create a libalpm handle", errcode, NULL);
  }
}

/* Database getters/setters */

static PyObject* pyalpm_get_localdb(PyObject *self, PyObject *dummy) {
  alpm_handle_t *handle = ALPM_HANDLE(self);
  return pyalpm_db_from_pmdb(alpm_get_localdb(handle));
}

static PyObject* pyalpm_get_syncdbs(PyObject *self, PyObject *dummy) {
  alpm_handle_t *handle = ALPM_HANDLE(self);
  return alpmlist_to_pylist(alpm_get_syncdbs(handle),
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

  result = alpm_register_syncdb(handle, dbname, pgp_level);
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
  alpm_pkgreason_t reason;
  int ret;
  if (!PyArg_ParseTuple(args, "O!i:set_pkgreason", &AlpmPackageType, &pkg, &reason)) {
    return NULL;
  }
  pmpkg = ALPM_PACKAGE(pkg);
  ret = alpm_pkg_set_reason(pmpkg, reason);

  if (ret == -1) RET_ERR("failed setting install reason", alpm_errno(handle), NULL);
  Py_RETURN_NONE;
}

/* String attributes get/setters */
struct _alpm_str_getset {
  const char *(*getter)(alpm_handle_t *);
  int (*setter)(alpm_handle_t *, const char *);
};

static PyObject *_get_string_attr(PyObject *self, const struct _alpm_str_getset *closure) {
  alpm_handle_t *handle = ALPM_HANDLE(self);
  const char *str = closure->getter(handle);
  if(str == NULL)
    RET_ERR("failed getting option value", alpm_errno(handle), NULL);
  return Py_BuildValue("s", str);
}

static int _set_string_attr(PyObject *self, PyObject *value, const struct _alpm_str_getset *closure) {
  alpm_handle_t *handle = ALPM_HANDLE(self);
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

  ret = closure->setter(handle, path);
  free(path);
  if (ret == -1) RET_ERR("failed setting option value", alpm_errno(handle), -1);
  return 0;
}

static struct _alpm_str_getset root_getset    = { alpm_option_get_root, NULL };
static struct _alpm_str_getset dbpath_getset  = { alpm_option_get_dbpath, NULL };
static struct _alpm_str_getset lockfile_getset  = { alpm_option_get_lockfile, NULL };
static struct _alpm_str_getset logfile_getset = { alpm_option_get_logfile, alpm_option_set_logfile };
static struct _alpm_str_getset gpgdir_getset = { alpm_option_get_gpgdir, alpm_option_set_gpgdir };
static struct _alpm_str_getset arch_getset = { alpm_option_get_arch, alpm_option_set_arch };

/* Callback attributes get/setters */
typedef int (*alpm_cb_setter)(alpm_handle_t*, void*);
struct _alpm_cb_getset {
  alpm_cb_setter setter;
  void *cb_wrapper;
  pyalpm_callback_id id;
};

void pyalpm_eventcb(alpm_event_t event, void* data1, void *data2);
void pyalpm_questioncb(alpm_question_t question,
    void* data1, void *data2, void* data3, int* retcode);
void pyalpm_progresscb(alpm_progress_t op,
    const char* target_name, int percentage, size_t n_targets, size_t cur_target);

static struct _alpm_cb_getset cb_getsets[N_CALLBACKS] = {
  { (alpm_cb_setter)alpm_option_set_logcb, pyalpm_logcb, CB_LOG },
  { (alpm_cb_setter)alpm_option_set_dlcb, pyalpm_dlcb, CB_DOWNLOAD },
  { (alpm_cb_setter)alpm_option_set_fetchcb, pyalpm_fetchcb, CB_FETCH },
  { (alpm_cb_setter)alpm_option_set_totaldlcb, pyalpm_totaldlcb, CB_TOTALDL },
  { (alpm_cb_setter)alpm_option_set_eventcb, pyalpm_eventcb, CB_EVENT },
  { (alpm_cb_setter)alpm_option_set_questioncb, pyalpm_questioncb, CB_QUESTION },
  { (alpm_cb_setter)alpm_option_set_progresscb, pyalpm_progresscb, CB_PROGRESS },
};

/** Callback options
 * We use Python callable objects as callbacks: they are
 * stored in static variables, and the reference count is
 * increased accordingly.
 *
 * These Python functions are wrapped into C functions 
 * that are passed to libalpm.
 */
PyObject *global_py_callbacks[N_CALLBACKS];

static PyObject* _get_cb_attr(PyObject *self, const struct _alpm_cb_getset *closure) {
  /* AlpmHandle *it = self; */
  PyObject *pycb = global_py_callbacks[closure->id];
  if (pycb == NULL) Py_RETURN_NONE;
  Py_INCREF(pycb);
  return pycb;
}

static int _set_cb_attr(PyObject *self, PyObject *value, const struct _alpm_cb_getset *closure) {
  AlpmHandle *it = (AlpmHandle *)self;
  if (value == Py_None) {
    Py_CLEAR(global_py_callbacks[closure->id]);
    closure->setter(it->c_data, NULL);
  } else if (PyCallable_Check(value)) {
    Py_CLEAR(global_py_callbacks[closure->id]);
    Py_INCREF(value);
    global_py_callbacks[closure->id] = value;
    closure->setter(it->c_data, closure->cb_wrapper);
  } else {
    PyErr_SetString(PyExc_TypeError, "value must be None or a function");
    return -1;
  }

  return 0;
}

struct PyGetSetDef pyalpm_handle_getset[] = {
  /** filepaths */
  { "root",
    (getter)_get_string_attr,
    NULL,
    "system root directory", &root_getset } ,
  { "dbpath",
    (getter)_get_string_attr,
    NULL,
    "alpm database directory", &dbpath_getset } ,
  { "logfile",
    (getter)_get_string_attr,
    (setter)_set_string_attr,
    "alpm logfile path", &logfile_getset } ,
  { "lockfile",
    (getter)_get_string_attr,
    NULL,
    "alpm lockfile path", &lockfile_getset } ,
  { "gpgdir",
    (getter)_get_string_attr,
    (setter)_set_string_attr,
    "alpm GnuPG home directory", &gpgdir_getset } ,

  /** strings */
  { "arch",
    (getter)_get_string_attr,
    (setter)_set_string_attr,
    "Target archichecture", &arch_getset } ,

  /** booleans */
  { "usesyslog",
    (getter)option_get_usesyslog_alpm,
    (setter)option_set_usesyslog_alpm,
    "use syslog (an integer, 0 = False, 1 = True)", NULL } ,
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
    (getter)_get_cb_attr, (setter)_set_cb_attr,
    "logging callback, with arguments (loglevel, format string, tuple)",
    &cb_getsets[CB_LOG] },
  { "dlcb",
    (getter)_get_cb_attr, (setter)_set_cb_attr,
    "download status callback (a function)\n"
    "args: filename    :: str\n"
    "      transferred :: int\n"
    "      total       :: int\n",
    &cb_getsets[CB_DOWNLOAD] },
  { "totaldlcb",
    (getter)_get_cb_attr, (setter)_set_cb_attr,
    "total download size callback: totaldlcb(total_size)",
    &cb_getsets[CB_TOTALDL] },
  { "fetchcb",
    (getter)_get_cb_attr, (setter)_set_cb_attr,
    "download function\n"
    "args: url              :: string\n"
    "      destination path :: string\n"
    "      overwrite        :: bool\n"
    "returns: 0 on success, 1 if file exists, -1 on error",
    &cb_getsets[CB_FETCH] },
  { "eventcb",
    (getter)_get_cb_attr, (setter)_set_cb_attr,
    "  a function called when an event occurs\n"
    "    -- args: (event ID, event string, (object 1, object 2))\n",
    &cb_getsets[CB_EVENT] },
  { "questioncb",
    (getter)_get_cb_attr, (setter)_set_cb_attr,
    "  a function called to get user input\n",
    &cb_getsets[CB_QUESTION] },
  { "progresscb",
    (getter)_get_cb_attr, (setter)_set_cb_attr,
    "  -- a function called to indicate progress\n"
    "    -- args: (target name, percentage, number of targets, target number)\n",
    &cb_getsets[CB_PROGRESS] },

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
    "  needed, allexplicit, unneeded, recurseall, nolock\n"
    "    -- the transaction options (booleans)\n"
  },

  /* Package load */
  {"load_pkg", (PyCFunction)pyalpm_package_load, METH_VARARGS | METH_KEYWORDS,
    "loads package information from a tarball"},

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

static void pyalpm_dealloc(PyObject* self) {
  alpm_handle_t *handle = ALPM_HANDLE(self);
  int ret = alpm_release(handle);
  if (ret == -1) {
    PyErr_Format(alpm_error, "unable to release alpm handle");
  }
  handle = NULL;
  Py_TYPE(self)->tp_free((PyObject *)self);
}

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
  .tp_dealloc = (destructor) pyalpm_dealloc,
};

/** Initializes Handle class in module */
int init_pyalpm_handle(PyObject *module) {
  PyObject *type;
  if (PyType_Ready(&AlpmHandleType) < 0)
    return -1;
  type = (PyObject*)&AlpmHandleType;
  Py_INCREF(type);
  PyModule_AddObject(module, "Handle", type);

  PyModule_AddIntConstant(module, "LOG_ERROR", ALPM_LOG_ERROR);
  PyModule_AddIntConstant(module, "LOG_WARNING", ALPM_LOG_WARNING);
  PyModule_AddIntConstant(module, "LOG_DEBUG", ALPM_LOG_DEBUG);
  PyModule_AddIntConstant(module, "LOG_FUNCTION", ALPM_LOG_FUNCTION);

  return 0;
}

/* vim: set ts=2 sw=2 et: */
