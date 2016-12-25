/**
 * transaction.c : wrapper class around libalpm transactions
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
#include <string.h>
#include <alpm.h>
#include <Python.h>
#include "package.h"
#include "handle.h"
#include "util.h"

/** Transaction callbacks */
extern PyObject *global_py_callbacks[N_CALLBACKS];

void pyalpm_eventcb(alpm_event_t *event) {
  const char *eventstr;
  switch(event->type) {
    case ALPM_EVENT_CHECKDEPS_START:
      eventstr = "Checking dependencies";
      break;
    case ALPM_EVENT_CHECKDEPS_DONE:
      eventstr = "Done checking dependencies";
      break;
    case ALPM_EVENT_FILECONFLICTS_START:
      eventstr = "Checking file conflicts";
      break;
    case ALPM_EVENT_FILECONFLICTS_DONE:
      eventstr = "Done checking file conflicts";
      break;
    case ALPM_EVENT_RESOLVEDEPS_START:
      eventstr = "Resolving dependencies";
      break;
    case ALPM_EVENT_RESOLVEDEPS_DONE:
      eventstr = "Done resolving dependencies";
      break;
    case ALPM_EVENT_INTERCONFLICTS_START:
      eventstr = "Checking inter conflicts";
      break;
    case ALPM_EVENT_INTERCONFLICTS_DONE:
      eventstr = "Done checking inter conflicts";
      break;
    case ALPM_EVENT_PACKAGE_OPERATION_START:
      eventstr = "Operating on a package";
      switch(((alpm_event_package_operation_t*)event)->operation) {
      case ALPM_PACKAGE_INSTALL:
        eventstr = "Adding a package";
        break;
      case ALPM_PACKAGE_UPGRADE:
        eventstr = "Upgrading a package";
        break;
      case ALPM_PACKAGE_REINSTALL:
        eventstr = "Reinstalling a package";
        break;
      case ALPM_PACKAGE_DOWNGRADE:
        eventstr = "Downgrading a package";
        break;
      case ALPM_PACKAGE_REMOVE:
        eventstr = "Removing a package";
        break;
      }
      break;
    case ALPM_EVENT_PACKAGE_OPERATION_DONE:
      eventstr = "Done operating on a package";
      switch(((alpm_event_package_operation_t*)event)->operation) {
      case ALPM_PACKAGE_INSTALL:
        eventstr = "Done adding a package";
        break;
      case ALPM_PACKAGE_UPGRADE:
        eventstr = "Done upgrading a package";
        break;
      case ALPM_PACKAGE_REINSTALL:
        eventstr = "Done reinstalling a package";
        break;
      case ALPM_PACKAGE_DOWNGRADE:
        eventstr = "Done downgrading a package";
        break;
      case ALPM_PACKAGE_REMOVE:
        eventstr = "Done removing a package";
        break;
      }
      break;
    case ALPM_EVENT_INTEGRITY_START:
      eventstr = "Checking integrity";
      break;
    case ALPM_EVENT_INTEGRITY_DONE:
      eventstr = "Done checking integrity";
      break;
    case ALPM_EVENT_LOAD_START:
    case ALPM_EVENT_LOAD_DONE:
    case ALPM_EVENT_DELTA_INTEGRITY_START:
    case ALPM_EVENT_DELTA_INTEGRITY_DONE:
    case ALPM_EVENT_DELTA_PATCHES_START:
    case ALPM_EVENT_DELTA_PATCHES_DONE:
    case ALPM_EVENT_DELTA_PATCH_START:
      /* info here */
    case ALPM_EVENT_DELTA_PATCH_DONE:
    case ALPM_EVENT_DELTA_PATCH_FAILED:
    case ALPM_EVENT_SCRIPTLET_INFO:
      /* info here */
    case ALPM_EVENT_RETRIEVE_START:
    case ALPM_EVENT_RETRIEVE_DONE:
    case ALPM_EVENT_RETRIEVE_FAILED:
      /* info here */
      eventstr = "event not implemented";
      break;
    case ALPM_EVENT_DISKSPACE_START:
      eventstr = "Checking disk space";
      break;
    case ALPM_EVENT_DISKSPACE_DONE:
      eventstr = "Done checking disk space";
      break;
    case ALPM_EVENT_OPTDEP_REMOVAL:
    case ALPM_EVENT_DATABASE_MISSING:
      eventstr = "event not implemented";
      break;
    case ALPM_EVENT_KEYRING_START:
      eventstr = "Checking keys in keyring";
      break;
    case ALPM_EVENT_KEYRING_DONE:
      eventstr = "Done checking keys in keyring";
      break;
    case ALPM_EVENT_KEY_DOWNLOAD_START:
    case ALPM_EVENT_KEY_DOWNLOAD_DONE:
    case ALPM_EVENT_PACNEW_CREATED:
    case ALPM_EVENT_PACSAVE_CREATED:
    case ALPM_EVENT_HOOK_START:
    case ALPM_EVENT_HOOK_DONE:
    case ALPM_EVENT_HOOK_RUN_START:
    case ALPM_EVENT_HOOK_RUN_DONE:
    default:
      eventstr = "unknown event";
  }
  {
    PyObject *result = NULL;
    if (global_py_callbacks[CB_PROGRESS]) {
      result = PyObject_CallFunction(global_py_callbacks[CB_EVENT], "is", event->type, eventstr);
    } else {
      PyErr_SetString(PyExc_RuntimeError, "event callback was called but it's not set!");
    }
    if (PyErr_Occurred()) PyErr_Print();
    Py_CLEAR(result);
  }
}

void pyalpm_questioncb(alpm_question_t question,
        void* data1, void *data2, void* data3, int* retcode) {
}

void pyalpm_progresscb(alpm_progress_t op,
        const char* target_name, int percentage, size_t n_targets, size_t cur_target) {
  PyObject *result = NULL;
  if (global_py_callbacks[CB_PROGRESS]) {
    result = PyObject_CallFunction(global_py_callbacks[CB_PROGRESS], "sinn",
      target_name, percentage, n_targets, cur_target);
  } else {
    PyErr_SetString(PyExc_RuntimeError, "progress callback was called but it's not set!");
  }
  if (PyErr_Occurred()) {
    PyErr_Print();
    /* alpm_trans_interrupt(handle); */
  }
  Py_CLEAR(result);
}

/** Transaction info translation */
static PyObject* pyobject_from_pmdepmissing(void *item) {
  alpm_depmissing_t* miss = (alpm_depmissing_t*)item;
  char* needed = alpm_dep_compute_string(miss->depend);
  PyObject *result = Py_BuildValue("(sss)",
      miss->target,
      needed,
      miss->causingpkg);
  free(needed);
  return result;
}

static PyObject* pyobject_from_pmconflict(void *item) {
  alpm_conflict_t* conflict = (alpm_conflict_t*)item;
  return Py_BuildValue("(sss)",
      conflict->package1,
      conflict->package2,
      conflict->reason);
}

static PyObject* pyobject_from_pmfileconflict(void *item) {
  alpm_fileconflict_t* conflict = (alpm_fileconflict_t*)item;
  const char *target = conflict->target;
  const char *filename = conflict->file;
  switch(conflict->type) {
  case ALPM_FILECONFLICT_TARGET:
    return Py_BuildValue("(sss)", target, filename, conflict->ctarget);
  case ALPM_FILECONFLICT_FILESYSTEM:
    return Py_BuildValue("(ssO)", target, filename, Py_None);
  default:
    PyErr_Format(PyExc_RuntimeError, "invalid type %d for alpm_fileconflict_t object", conflict->type);
    return NULL;
  }
}

/* Standard methods */
const char* flagnames[19] = {
  "nodeps",
  "force",
  "nosave",
  "nodepversion",
  "cascade",
  "recurse",
  "dbonly",
  NULL,
  "alldeps",
  "downloadonly",
  "noscriptlet",
  "noconflicts",
  NULL,
  "needed",
  "allexplicit",
  "unneeded",
  "recurseall",
  "nolock",
  NULL
};

static PyObject *pyalpm_trans_get_flags(PyObject *self, void *closure)
{
  PyObject *result;
  alpm_handle_t *handle = ALPM_HANDLE(self);
  int flags = alpm_trans_get_flags(handle);
  int i;
  if (flags == -1) RET_ERR("no transaction defined", alpm_errno(handle), NULL);
  result = PyDict_New();
  for (i = 0; i < 18; i++) {
    if(flagnames[i])
      PyDict_SetItemString(result, flagnames[i], flags & (1 << i) ? Py_True : Py_False);
  }
  return result;
}

static PyObject *pyalpm_trans_get_add(PyObject *self, void *closure)
{
  alpm_handle_t *handle = ALPM_HANDLE(self);
  alpm_list_t *to_add;
  /* sanity check */
  int flags = alpm_trans_get_flags(handle);
  if (flags == -1) RET_ERR("no transaction defined", alpm_errno(handle), NULL);

  to_add = alpm_trans_get_add(handle);
  return alpmlist_to_pylist(to_add, pyalpm_package_from_pmpkg);
}

static PyObject *pyalpm_trans_get_remove(PyObject *self, void *closure)
{
  alpm_handle_t *handle = ALPM_HANDLE(self);
  alpm_list_t *to_remove;
  /* sanity check */
  int flags = alpm_trans_get_flags(handle);
  if (flags == -1) RET_ERR("no transaction defined", alpm_errno(handle), NULL);

  to_remove = alpm_trans_get_remove(handle);
  return alpmlist_to_pylist(to_remove, pyalpm_package_from_pmpkg);
}

/** Transaction flow */
#define INDEX_FLAGS(array) \
  array[0], array[1], array[2], \
  array[3], array[4], array[5], \
  array[6], array[8], array[9], \
  array[10], array[11], array[13], \
  array[14], array[15], array[16], \
  array[17]

/** Initializes a transaction
 * @param self a Handle object
 * ...
 * @return a Transaction object with the same underlying object
 */
PyObject* pyalpm_trans_init(PyObject *self, PyObject *args, PyObject *kwargs) {
  alpm_handle_t *handle = ALPM_HANDLE(self);
  PyObject *result;
  const char* keywords[] = { INDEX_FLAGS(flagnames), NULL };
  char flags[18] = "\0\0\0\0\0" /* 5 */ "\0\0\0\0\0" /* 10 */ "\0\0\0\0\0" /* 15 */ "\0\0\0";

  /* check all arguments */
  if (!PyArg_ParseTupleAndKeywords(args, kwargs,
        "|bbbbbbbbbbbbbbbbOOO", (char**)keywords,
        INDEX_FLAGS(&flags))) {
    return NULL;
  }

  /* run alpm_trans_init() */
  {
    alpm_transflag_t flag_int = 0;
    int i, ret;
    for (i = 0; i < 18; i++) {
      if (flags[i]) flag_int |= 1U << i;
    }
    ret = alpm_trans_init(handle, flag_int);
    if (ret == -1) {
      RET_ERR("transaction could not be initialized", alpm_errno(handle), NULL);
    }
  }
  result = pyalpm_transaction_from_pmhandle(handle);
  return result;
}

static PyObject* pyalpm_trans_prepare(PyObject *self, PyObject *args) {
  alpm_handle_t *handle = ALPM_HANDLE(self);
  alpm_list_t *data;

  int ret = alpm_trans_prepare(handle, &data);
  if (ret == -1) {
    /* return the list of package conflicts in the exception */
    PyObject *info = alpmlist_to_pylist(data, pyobject_from_pmdepmissing);
    if (!info) return NULL;
    RET_ERR_DATA("transaction preparation failed", alpm_errno(handle), info, NULL);
  }

  Py_RETURN_NONE;
}

static PyObject* pyalpm_trans_commit(PyObject *self, PyObject *args) {
  alpm_handle_t *handle = ALPM_HANDLE(self);
  alpm_list_t *data = NULL;
  int ret;
  enum _alpm_errno_t err;
  PyObject *err_info = NULL;

  ret = alpm_trans_commit(handle, &data);
  if (ret == 0) Py_RETURN_NONE;
  if (ret != -1) {
    PyErr_Format(PyExc_RuntimeError,
        "unexpected return value %d from alpm_trans_commit()", ret);
    return NULL;
  }

  err = alpm_errno(handle);
  switch(err) {
    case ALPM_ERR_FILE_CONFLICTS:
      /* return the list of file conflicts in the exception */
      err_info = alpmlist_to_pylist(data, pyobject_from_pmfileconflict);
      break;
    case ALPM_ERR_PKG_INVALID:
    case ALPM_ERR_PKG_INVALID_CHECKSUM:
    case ALPM_ERR_PKG_INVALID_SIG:
    case ALPM_ERR_DLT_INVALID:
      err_info = alpmlist_to_pylist(data, pyobject_from_string);
      break;
    default:
      break;
  }
  if (err_info)
    RET_ERR_DATA("transaction failed", err, err_info, NULL);
  else
    RET_ERR("transaction failed", err, NULL);
}

static PyObject* pyalpm_trans_interrupt(PyObject *self, PyObject *args) {
  alpm_handle_t *handle = ALPM_HANDLE(self);
  int ret = alpm_trans_interrupt(handle);
  if (ret == -1) RET_ERR("unable to interrupt transaction", alpm_errno(handle), NULL);
  Py_RETURN_NONE;
}

PyObject* pyalpm_trans_release(PyObject *self, PyObject *args) {
  alpm_handle_t *handle = ALPM_HANDLE(self);
  int ret = alpm_trans_release(handle);
  if (ret == -1) RET_ERR("unable to release transaction", alpm_errno(handle), NULL);
  Py_RETURN_NONE;
}

/** Transaction contents */
static PyObject* pyalpm_trans_add_pkg(PyObject *self, PyObject *args) {
  alpm_handle_t *handle = ALPM_HANDLE(self);
  alpm_pkg_t *pmpkg;
  PyObject *pkg;
  int ret;

  if (!PyArg_ParseTuple(args, "O!", &AlpmPackageType, &pkg)) {
    return NULL;
  }

  pmpkg = pmpkg_from_pyalpm_pkg(pkg);
  ret = alpm_add_pkg(handle, pmpkg);
  if (ret == -1) RET_ERR("unable to update transaction", alpm_errno(handle), NULL);
  /* alpm_add_pkg eats the reference to pkg */
  pyalpm_pkg_unref(pkg);
  Py_RETURN_NONE;
}

static PyObject* pyalpm_trans_remove_pkg(PyObject *self, PyObject *args) {
  alpm_handle_t *handle = ALPM_HANDLE(self);
  PyObject *pkg;
  alpm_pkg_t *pmpkg;
  int ret;

  if (!PyArg_ParseTuple(args, "O!", &AlpmPackageType, &pkg)) {
    return NULL;
  }

  pmpkg = pmpkg_from_pyalpm_pkg(pkg);
  ret = alpm_remove_pkg(handle, pmpkg);
  if (ret == -1) RET_ERR("unable to update transaction", alpm_errno(handle), NULL);
  Py_RETURN_NONE;
}

static PyObject* pyalpm_trans_sysupgrade(PyObject *self, PyObject *args, PyObject *kwargs) {
  alpm_handle_t *handle = ALPM_HANDLE(self);
  char* keyword[] = {"downgrade", NULL};
  PyObject *downgrade;
  int do_downgrade, ret;

  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O!", keyword, &PyBool_Type, &downgrade))
    return NULL;

  do_downgrade = (downgrade == Py_True) ? 1 : 0;
  ret = alpm_sync_sysupgrade(handle, do_downgrade);
  if (ret == -1) RET_ERR("unable to update transaction", alpm_errno(handle), NULL);
  Py_RETURN_NONE;
}

/** Properties and methods */

static struct PyGetSetDef pyalpm_trans_getset[] = {
  /** filepaths */
  { "flags", (getter)pyalpm_trans_get_flags, NULL, "Transaction flags", NULL } ,
  { "to_add", (getter)pyalpm_trans_get_add, NULL, "Packages added by the transaction", NULL },
  { "to_remove", (getter)pyalpm_trans_get_remove, NULL, "Packages added by the transaction", NULL },
  { NULL }
};

static struct PyMethodDef pyalpm_trans_methods[] = {
  /* Execution flow */
  {"prepare", pyalpm_trans_prepare,    METH_NOARGS, "prepare" },
  {"commit",  pyalpm_trans_commit,     METH_NOARGS, "commit" },
  {"interrupt", pyalpm_trans_interrupt,METH_NOARGS,  "Interrupt the transaction." },
  {"release", pyalpm_trans_release,    METH_NOARGS,  "Release the transaction." },

  /* Transaction contents */
  {"add_pkg",    pyalpm_trans_add_pkg,    METH_VARARGS,
    "append a package addition to transaction"},
  {"remove_pkg", pyalpm_trans_remove_pkg, METH_VARARGS,
    "append a package removal to transaction"},
  {"sysupgrade", (PyCFunction)pyalpm_trans_sysupgrade, METH_VARARGS | METH_KEYWORDS,
    "set the transaction to perform a system upgrade\n"
    "args:\n"
    "  transaction (boolean) : whether to enable downgrades\n" },
  { NULL }
};

/* The Transaction object have the same underlying C structure
 * as the Handle objects. Only the method table changes.
 */
static PyTypeObject AlpmTransactionType = {
  PyVarObject_HEAD_INIT(NULL, 0)
  "alpm.Transaction",    /*tp_name*/
  sizeof(AlpmHandle),  /*tp_basicsize*/
  .tp_flags = Py_TPFLAGS_DEFAULT,
  .tp_doc = "This class is the main interface to get/set libalpm options",
  .tp_methods = pyalpm_trans_methods,
  .tp_getset = pyalpm_trans_getset,
};

PyObject *pyalpm_transaction_from_pmhandle(void* data) {
  alpm_handle_t *handle = (alpm_handle_t*)data;
  AlpmHandle *self;
  self = (AlpmHandle*)AlpmTransactionType.tp_alloc(&AlpmTransactionType, 0);
  if (self == NULL) {
    PyErr_SetString(PyExc_RuntimeError, "unable to create pyalpm.Transaction object");
    return NULL;
  }

  self->c_data = handle;
  return (PyObject *)self;
}

/* Initialization */
int init_pyalpm_transaction(PyObject *module) {
  if (PyType_Ready(&AlpmTransactionType) < 0)
    return -1;
  Py_INCREF(&AlpmTransactionType);
  PyModule_AddObject(module, "Transaction", (PyObject*)(&AlpmTransactionType));
  return 0;
}

/* vim: set ts=2 sw=2 tw=0 et: */

