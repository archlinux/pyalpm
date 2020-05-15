/**
 * package.c : wrapper class around alpm_pkg_t
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

#ifndef _PYALPM_PACKAGE_H
#define _PYALPM_PACKAGE_H

#include <Python.h>

typedef struct _AlpmPackage {
  PyObject_HEAD
  alpm_pkg_t *c_data;
  int needs_free;
} AlpmPackage;

#define ALPM_PACKAGE(self) (((AlpmPackage*)(self))->c_data)

extern PyTypeObject AlpmPackageType;
int PyAlpmPkg_Check(PyObject *object);

void pyalpm_pkg_unref(PyObject *object);

PyObject *pyalpm_package_from_pmpkg(void* data);
alpm_pkg_t *pmpkg_from_pyalpm_pkg(PyObject *object);

int pylist_pkg_to_alpmlist(PyObject *list, alpm_list_t **result);

PyObject *pyalpm_package_load(PyObject *self, PyObject *args, PyObject *kwargs);

#endif
