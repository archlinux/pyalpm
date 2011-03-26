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

#ifndef _PYALPM_PACKAGE_H
#define _PYALPM_PACKAGE_H

#include <Python.h>

void init_pyalpm_package(PyObject *module);

PyObject *pyalpm_package_from_pmpkg(void* data);
PyObject *pyalpm_package_load(PyObject *self, PyObject *args);

#endif // _PYALPM_PACKAGE_H
