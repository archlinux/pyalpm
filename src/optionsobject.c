/**
 * package.c : wrapper class around pmpkg_t
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
#include "options.h"

struct PyGetSetDef pyalpm_options_getset[] = {
  /** filepaths */
  { "root",
    (getter)option_get_root_alpm,
    (setter)option_set_root_alpm,
    "system root directory", NULL } ,
  { "dbpath",
    (getter)option_get_dbpath_alpm,
    (setter)option_set_dbpath_alpm,
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
    NULL,
    "list of package cache directories", NULL },
  { "noupgrades",
    (getter)option_get_noupgrades_alpm,
    NULL,
    "list of ...", NULL },
  { "noextracts",
    (getter)option_get_noextracts_alpm,
    NULL,
    "list of ...", NULL },
  { "ignorepkgs",
    (getter)option_get_ignorepkgs_alpm,
    NULL,
    "list of ignored packages", NULL },
  { "ignoregrps",
    (getter)option_get_ignoregrps_alpm,
    NULL,
    "list of ignored groups", NULL },

  /** terminator */
  { NULL }
};

PyTypeObject AlpmOptionSetType = {
  PyVarObject_HEAD_INIT(NULL, 0)
  "alpm.Options",    /*tp_name*/
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
  0,                  /* tp_methods */
  0,                  /* tp_members */
  pyalpm_options_getset,  /* tp_getset */
};

/** Initializes Options class in module */
void init_pyalpm_options(PyObject *module) {
  // the OptionSet type
  PyObject *type;
  if (PyType_Ready(&AlpmOptionSetType) < 0)
    return;
  type = (PyObject*)&AlpmOptionSetType;
  Py_INCREF(type);
  PyModule_AddObject(module, "Options", type);

  // the static instance
  PyObject* options = (PyObject*)AlpmOptionSetType.tp_alloc(&AlpmOptionSetType, 0);
  PyModule_AddObject(module, "options", options);
}

/* vim: set ts=2 et: */
