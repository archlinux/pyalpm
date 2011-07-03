/**
 * handle.h : wrapper class around alpm_handle_t
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

#ifndef PYALPM_HANDLE_H
#define PYALPM_HANDLE_H

#include <Python.h>

typedef struct _AlpmHandle {
  PyObject_HEAD
  alpm_handle_t *c_data;
} AlpmHandle;

#define ALPM_HANDLE(self) (((AlpmHandle*)(self))->c_data)

PyObject *pyalpm_release(PyObject* self, PyObject* args);

/* from transaction.c */
PyObject *pyalpm_transaction_from_pmhandle(void* data);
PyObject* pyalpm_trans_init(PyObject *self, PyObject *args, PyObject *kwargs);

#endif
