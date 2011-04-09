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

#include <string.h>
#include <alpm.h>
#include <Python.h>
#include "package.h"
#include "util.h"

/** Transaction callbacks */
static PyObject *event_cb = NULL;
static PyObject *conv_cb = NULL;
static PyObject *progress_cb = NULL;

static void pyalpm_trans_eventcb(pmtransevt_t event, void* data1, void *data2) {
}

static void pyalpm_trans_convcb(pmtransconv_t question,
        void* data1, void *data2, void* data3, int* retcode) {
}

static void pyalpm_trans_progresscb(pmtransprog_t op,
        const char* target_name, int percentage, size_t n_targets, size_t cur_target) {
}

/** Transaction info translation */
static PyObject* pyobject_from_pmdepmissing(void *item) {
  pmdepmissing_t* miss = (pmdepmissing_t*)item;
  char* needed = alpm_dep_compute_string(alpm_miss_get_dep(miss));
  PyObject *result = Py_BuildValue("(sss)",
      alpm_miss_get_target(miss),
      needed,
      alpm_miss_get_causingpkg(miss));
  free(needed);
  return result;
}

static PyObject* pyobject_from_pmconflict(void *item) {
  pmconflict_t* conflict = (pmconflict_t*)item;
  return Py_BuildValue("(sss)",
      alpm_conflict_get_package1(conflict),
      alpm_conflict_get_package2(conflict),
      alpm_conflict_get_reason(conflict));
}

static PyObject* pyobject_from_pmfileconflict(void *item) {
  pmfileconflict_t* conflict = (pmfileconflict_t*)item;
  return Py_BuildValue("(sss)",
      alpm_fileconflict_get_target(conflict),
      alpm_fileconflict_get_file(conflict),
      alpm_fileconflict_get_ctarget(conflict));
}

/* Standard methods */
static PyObject *pyalpm_trans_get_flags(PyObject *self, void *closure)
{
  int flags = alpm_trans_get_flags();
  if (flags == -1) RET_ERR("no transaction defined", NULL);
  return PyLong_FromLong(flags);
}

static PyObject *pyalpm_trans_get_add(PyObject *self, void *closure)
{
  /* sanity check */
  int flags = alpm_trans_get_flags();
  if (flags == -1) RET_ERR("no transaction defined", NULL);

  alpm_list_t *to_add = alpm_trans_get_add();
  return alpmlist_to_pylist(to_add, pyalpm_package_from_pmpkg);
}

static PyObject *pyalpm_trans_get_remove(PyObject *self, void *closure)
{
  /* sanity check */
  int flags = alpm_trans_get_flags();
  if (flags == -1) RET_ERR("no transaction defined", NULL);

  alpm_list_t *to_remove = alpm_trans_get_remove();
  return alpmlist_to_pylist(to_remove, pyalpm_package_from_pmpkg);
}

/** Transaction flow */
PyObject* pyalpm_trans_init(PyObject *self, PyObject *args) {
  /* TODO: pass arguments to alpm_trans_init() */
  int ret = alpm_trans_init(0, NULL, NULL, NULL);
  if (ret == -1) {
    RET_ERR("transaction could not be initialized", NULL);
  }
  Py_RETURN_NONE;
}

static PyObject* pyalpm_trans_prepare(PyObject *self, PyObject *args) {
  alpm_list_t *data;

  int ret = alpm_trans_prepare(&data);
  if (ret == -1) {
    /* return the list of package conflicts in the exception */
    PyObject *info = alpmlist_to_pylist(data, pyobject_from_pmdepmissing);
    if (!info) return NULL;
    RET_ERR_DATA("transaction preparation failed", info, NULL);
  }

  Py_RETURN_NONE;
}

static PyObject* pyalpm_trans_commit(PyObject *self, PyObject *args) {
  alpm_list_t *data = NULL;

  int ret = alpm_trans_commit(&data);
  if (ret == -1) {
    /* return the list of file conflicts in the exception */
    PyObject *info = alpmlist_to_pylist(data, pyobject_from_pmfileconflict);
    if (!info) return NULL;
    RET_ERR_DATA("transaction failed", info, NULL);
  }

  Py_RETURN_NONE;
}

static PyObject* pyalpm_trans_interrupt(PyObject *self, PyObject *args) {
  int ret = alpm_trans_interrupt();
  if (ret == -1) RET_ERR("unable to interrupt transaction", NULL);
  Py_RETURN_NONE;
}

PyObject* pyalpm_trans_release(PyObject *self, PyObject *args) {
  int ret = alpm_trans_release();
  if (ret == -1) RET_ERR("unable to release transaction", NULL);
  Py_RETURN_NONE;
}

/** Transaction contents */
static PyObject* pyalpm_trans_add_pkg(PyObject *self, PyObject *args) {
  PyObject *pkg;
  if (!PyArg_ParseTuple(args, "O!", &AlpmPackageType, &pkg)) {
    return NULL;
  }

  pmpkg_t *pmpkg = pmpkg_from_pyalpm_pkg(pkg);
  int ret = alpm_add_pkg(pmpkg);
  if (ret == -1) RET_ERR("unable to update transaction", NULL);
  Py_RETURN_NONE;
}

static PyObject* pyalpm_trans_remove_pkg(PyObject *self, PyObject *args) {
  PyObject *pkg;
  if (!PyArg_ParseTuple(args, "O!", &AlpmPackageType, &pkg)) {
    return NULL;
  }

  pmpkg_t *pmpkg = pmpkg_from_pyalpm_pkg(pkg);
  int ret = alpm_remove_pkg(pmpkg);
  if (ret == -1) RET_ERR("unable to update transaction", NULL);
  Py_RETURN_NONE;
}

static PyObject* pyalpm_trans_sysupgrade(PyObject *self, PyObject *args, PyObject *kwargs) {
  char* keyword[] = {"downgrade", NULL};
  PyObject *downgrade;
  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O!", keyword, &PyBool_Type, &downgrade))
    return NULL;

  int do_downgrade = (downgrade == Py_True) ? 1 : 0;
  int ret = alpm_sync_sysupgrade(do_downgrade);
  if (ret == -1) RET_ERR("unable to update transaction", NULL);
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
  {"init",    pyalpm_trans_init,       METH_VARARGS, "init" },
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
    "set the transaction to perform a system upgrade"},
  { NULL }
};

static PyTypeObject AlpmTransactionType = {
  PyVarObject_HEAD_INIT(NULL, 0)
  "alpm.TransactionClass",    /*tp_name*/
  0,                   /*tp_basicsize*/
  0,                   /*tp_itemsize*/
  0,                   /*tp_dealloc*/
  0,                   /*tp_print*/
  0,                   /*tp_getattr*/
  0,                   /*tp_setattr*/
  NULL,                /*tp_reserved*/
  0,                   /*tp_repr*/
  0,                   /*tp_as_number*/
  0,                   /*tp_as_sequence*/
  0,                   /*tp_as_mapping*/
  0,                   /*tp_hash */
  0,                   /*tp_call*/
  0,                   /*tp_str*/
  0,                   /*tp_getattro*/
  0,                   /*tp_setattro*/
  0,                   /*tp_as_buffer*/
  Py_TPFLAGS_DEFAULT, /*tp_flags*/
  "This class is the main interface to get/set libalpm options",
                      /* tp_doc */
  0,                  /* tp_traverse */
  0,                  /* tp_clear */
  0,                  /* tp_richcompare */
  0,                  /* tp_weaklistoffset */
  0,                  /* tp_iter */
  0,                  /* tp_iternext */
  pyalpm_trans_methods, /* tp_methods */
  0,                    /* tp_members */
  pyalpm_trans_getset,  /* tp_getset */
};

/* Initialization */
int init_pyalpm_transaction(PyObject *module) {
  if (PyType_Ready(&AlpmTransactionType) < 0)
    return -1;
  Py_INCREF(&AlpmTransactionType);
  PyModule_AddObject(module, "TransactionClass", (PyObject*)(&AlpmTransactionType));

  // the static instance
  PyObject *the_transaction = (PyObject*)AlpmTransactionType.tp_alloc(&AlpmTransactionType, 0);
  PyModule_AddObject(module, "transaction", the_transaction);
  Py_INCREF(the_transaction);
  return 0;
}

/* vim: set ts=2 sw=2 tw=0 et: */

