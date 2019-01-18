/**
 * db.c : wrapper class around alpm_db_t
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

#ifndef _PYALPM_DB_H
#define _PYALPM_DB_H

#include <Python.h>

PyObject *pyalpm_db_from_pmdb(void* data);
int pylist_db_to_alpmlist(PyObject *list, alpm_list_t **result);

PyObject* pyalpm_find_grp_pkgs(PyObject* self, PyObject* args);
PyObject* pyalpm_sync_get_new_version(PyObject *self, PyObject* args);

#endif
