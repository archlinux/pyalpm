/**
 * options.h
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

#ifndef _PY_ALPM_OPTIONS_METHODS_H
#define _PY_ALPM_OPTIONS_METHODS_H

#include <Python.h>

/** Path options */
int option_set_root_alpm(PyObject *self, PyObject *args, void* closure);
PyObject * option_get_root_alpm(PyObject *self, void* closure);

int option_set_dbpath_alpm(PyObject *self, PyObject *args, void* closure);
PyObject * option_get_dbpath_alpm(PyObject *self, void* closure);

int option_set_logfile_alpm(PyObject *self, PyObject *value, void* closure);
PyObject * option_get_logfile_alpm(PyObject *self, void* closure);

/* read-only */
PyObject* option_get_lockfile_alpm(PyObject *self, void* closure);

/** string options */
int option_set_arch_alpm(PyObject *self, PyObject *value, void* closure);
PyObject* option_get_arch_alpm(PyObject *self, void* closure);

/** boolean options */

PyObject* option_get_usesyslog_alpm(PyObject *self, void* closure);
int option_set_usesyslog_alpm(PyObject *self, PyObject *value, void* closure);

PyObject* option_get_usedelta_alpm(PyObject *self, void* closure);
int option_set_usedelta_alpm(PyObject *self, PyObject *value, void* closure);

PyObject* option_get_checkspace_alpm(PyObject *self, void* closure);
int option_set_checkspace_alpm(PyObject *self, PyObject *value, void* closure);

/** list options */
PyObject* option_get_cachedirs_alpm(PyObject *self, void* closure);
PyObject* option_get_noupgrades_alpm(PyObject *self, void* closure);
PyObject* option_get_noextracts_alpm(PyObject *self, void* closure);
PyObject* option_get_ignorepkgs_alpm(PyObject *self, void* closure);
PyObject* option_get_ignoregrps_alpm(PyObject *self, void* closure);

PyObject * option_set_noupgrades_alpm(PyObject *self, PyObject *args);
PyObject * option_add_noupgrade_alpm(PyObject *self, PyObject *args);
PyObject * option_remove_noupgrade_alpm(PyObject *self, PyObject *args);

PyObject * option_add_cachedir_alpm(PyObject *self, PyObject *args);
PyObject * option_remove_cachedir_alpm(PyObject *self, PyObject *args);

PyObject * option_add_noextract_alpm(PyObject *self, PyObject *args);
PyObject * option_remove_noextract_alpm(PyObject *self, PyObject *args);

PyObject * option_add_ignorepkg_alpm(PyObject *self, PyObject *args);
PyObject * option_remove_ignorepkg_alpm(PyObject *self, PyObject *args);

PyObject * option_add_ignoregrps_alpm(PyObject *self, PyObject *args);
PyObject * option_remove_ignoregrps_alpm(PyObject *self, PyObject *args);

#endif // _PY_ALPM_OPTIONS_H

/* vim: set ts=2 sw=2 et: */
