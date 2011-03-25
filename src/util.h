/**
 * util.h : utility functions for pyalpm
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

#ifndef _PY_ALPM_UTIL_H
#define _PY_ALPM_UTIL_H

#include <Python.h>
#include <alpm.h>
#include <alpm_list.h>

/*misc internal functions*/
void add_alpm_list_t(alpm_list_t *prt);
void remove_alpm_list_t(alpm_list_t *prt);
alpm_list_t * tuple_alpm_list_t(PyObject *list);

PyObject* string_alpmlist_to_pylist(alpm_list_t *prt);

unsigned short check_init(void);

#endif // _PY_ALPM_UTIL_H
