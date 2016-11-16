/*
    
    Copyright 2008 Imanol Celaya <ilcra1989@gmail.com>

This file is part of pyalpm.

    pyalpm is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    pyalpm is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with pyalpm.  If not, see <http://www.gnu.org/licenses/>.

*/
#include <Python.h>

#ifndef PYALPM_H
#define PYALPM_H

/*pyalpm internal variables*/
char *error, *warning, *debug, *function;
unsigned short enable_messages_logcb = 0;

/* submodules init */
int init_pyalpm_handle(PyObject *module);
void init_pyalpm_db(PyObject *module);
void init_pyalpm_package(PyObject *module);
int init_pyalpm_transaction(PyObject *module);

#endif /* PYALPM_H */
