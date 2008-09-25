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

#include <alpm.h>
#include <alpm_list.h>

#include <stdio.h>
#include <string.h>

#ifndef PYALPM_H
#define PYALPM_H

/*pyalpm internal variables*/
char *error, *warning, *debug, *function;
unsigned short enable_messages_logcb = 0;
char VERSION[] = "0.1";
unsigned short init = 0;

/*copied from pacman db.h as it can't be included, set as pmdb_t in alpm.h*/
struct __pmdb_t {
	char *path;
	char *treename;
	void *handle;
	alpm_list_t *pkgcache;
	alpm_list_t *grpcache;
	alpm_list_t *servers;
};

/*misc internal functions*/
void add_alpm_list_t(alpm_list_t *prt);
void remove_alpm_list_t(alpm_list_t *prt);
alpm_list_t * tuple_alpm_list_t(PyObject *list);
void clean_pmdb_t(pmdb_t *prt);
unsigned short check_init(void);

/*pyalpm errors*/
PyObject *alpm_error = NULL;

/*miscelaneus functions*/
PyObject * alpmversion_alpm(PyObject *self);
PyObject * version_alpm(PyObject *self);
PyObject * check_init_alpm(PyObject *self);
PyObject * testconverter(PyObject *self, PyObject *args);

/*initialize and release*/
PyObject * initialize_alpm(PyObject *self);
PyObject * release_alpm(PyObject *self);

/*read-only function*/
PyObject * option_get_lockfile_alpm(PyObject *self);

/*write-only function*/
PyObject * option_set_usedelta_alpm(PyObject *self, PyObject *args);

/*_{get,set}_ functions*/
PyObject * option_set_root_alpm(PyObject *self, PyObject *args);
PyObject * option_get_root_alpm(PyObject *self);
PyObject * option_set_dbpath_alpm(PyObject *self, PyObject *args);
PyObject * option_get_dbpath_alpm(PyObject *self);
PyObject * option_set_logfile_alpm(PyObject *self, PyObject *args);
PyObject * option_get_logfile_alpm(PyObject *self);
PyObject * option_set_xfercommand_alpm(PyObject *self, PyObject *args);
PyObject * option_get_xfercommand_alpm(PyObject *self);
PyObject * option_set_nopassiveftp_alpm(PyObject *self, PyObject *args);
PyObject * option_get_nopassiveftp_alpm(PyObject *self);
PyObject * option_set_usesyslog_alpm(PyObject *self, PyObject *args);
PyObject * option_get_usesyslog_alpm(PyObject *self);

/*_{get,set,add,remove}_ functions*/

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

PyObject * option_add_holdpkg_alpm(PyObject *self, PyObject *args);
PyObject * option_remove_holdpkg_alpm(PyObject *self, PyObject *args);

/*callback function*/

PyObject * option_get_logcb_alpm(PyObject *self);
PyObject * option_set_logcb_alpm(PyObject *self, PyObject *args);

#endif /* PYALPM_H */