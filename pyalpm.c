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

#include "pyalpm.h"
#include <stdio.h>
/*pyalpm functions*/
PyObject * initialize_alpm(PyObject *self)
{
  if(alpm_initialize() == -1)
  {
    PyErr_SetString(alpm_error, "failed to initialize alpm");
    return NULL;
  }
  else
  {
    init = 1;
    addresses = (alpm_list_t*) malloc(sizeof(alpm_list_t));
    return Py_None;
  }
}

PyObject * release_alpm(PyObject *self)
{
  /*clean_memory(addresses);*/
  if(alpm_release() == -1)
  {
    PyErr_SetString(alpm_error, "failed to release alpm");
    return NULL;
  }
  else
  {
    init = 0;
    return Py_None;
  }
}

PyObject * option_get_logcb_alpm(PyObject *self)
{
  const char *str = NULL;
  if(alpm_option_get_logcb() == NULL)
  {
    PyErr_SetString(alpm_error, "failed getting logcb");
    return NULL;
  }
  else
  {
    str = alpm_option_get_logcb();
    
    return Py_BuildValue("s", str);
  }
}

void test_cb(pmloglevel_t level, char *fmt, va_list args)
{
  if(strlen(fmt))
  {
    switch(level)
    {
      case PM_LOG_ERROR: printf("%s", error); break;
      case PM_LOG_WARNING: printf("%s", warning); break;
      case PM_LOG_DEBUG: printf("%s", debug); break;
      case PM_LOG_FUNCTION: printf("%s", function); break;
      default: return;
    }
    vprintf(fmt, args);
  }
}

PyObject * option_set_logcb_alpm(PyObject *self, PyObject *args)
{
  if(!PyArg_ParseTuple(args, "ssss", &error, &warning, &debug, &function))
  {
    PyErr_SetString(alpm_error, "incorrect arguments");
    return NULL;
  }
  
  else
  {
    alpm_option_set_logcb(test_cb);
    
    return Py_None;
  }
}

/*
The following functions take a string as argument(*_set_*)
while other (*_get_*) return a string
*/
PyObject * option_get_root_alpm(PyObject *self)
{
  const char *str = alpm_option_get_root();
  
  if(str == NULL)
  {
    PyErr_SetString(alpm_error, "failed getting root path");
    return NULL;
  }
  else
  {
    return Py_BuildValue("s", str);
  }
}

PyObject * option_set_root_alpm(PyObject *self, PyObject *args)
{
  const char *path;

  if(!PyArg_ParseTuple(args, "s", &path))
  {
    PyErr_SetString(alpm_error, "error in the args");
    return NULL;
  }
  else
  {
    if(alpm_option_set_root(path) == -1)
    {
      PyErr_SetString(alpm_error, "failed setting root path");
      return NULL;
    }
    else
    {
      return Py_None;
    }
  }
}

PyObject * option_set_dbpath_alpm(PyObject *self, PyObject *args)
{
  const char *path;
  if(!PyArg_ParseTuple(args, "s", &path))
  {
    PyErr_SetString(alpm_error, "error in the args");
    return NULL;
  }
  else
  {
    if(alpm_option_set_dbpath(path) == -1)
    {
      PyErr_SetString(alpm_error, "failed setting dbpath");
      return NULL;
    }
    else
    {
      return Py_None;
    }
  }
}

PyObject * option_get_dbpath_alpm(PyObject *self)
{
  const char *str = alpm_option_get_dbpath();
  
  if(str == NULL)
  {
    PyErr_SetString(alpm_error, "failed getting dbpath.");
    return NULL;
  }
  else
  {
    return Py_BuildValue("s", str);
  }
}

PyObject * option_set_logfile_alpm(PyObject *self, PyObject *args)
{
  const char *path;
  if(!PyArg_ParseTuple(args, "s", &path))
  {
    PyErr_SetString(alpm_error, "error in the args");
    return NULL;
  }
  else
  {
    if(alpm_option_set_logfile(path) == -1)
    {
      PyErr_SetString(alpm_error, "failed setting logfile");
      return NULL;
    }
    else
    {
      return Py_None;
    }
  }
}

PyObject * option_get_logfile_alpm(PyObject *self)
{
  const char *str = alpm_option_get_logfile();
  
  if(str == NULL)
  {
    PyErr_SetString(alpm_error, "failed getting logfile.");
    return NULL;
  }
  else
  {
    return Py_BuildValue("s", str);
  }
}

PyObject * option_set_xfercommand_alpm(PyObject *self, PyObject *args)
{
  const char *cmd;
  if(!PyArg_ParseTuple(args, "s", &cmd))
  {
    PyErr_SetString(alpm_error, "error in the args");
    return NULL;
  }
  else
  {
    alpm_option_set_xfercommand(cmd);
    return Py_None;
  }
}

PyObject * option_get_xfercommand_alpm(PyObject *self)
{
  const char *str = alpm_option_get_xfercommand();
  
  if(str == NULL)
  {
    PyErr_SetString(alpm_error, "failed getting xfercommand.");
    return NULL;
  }
  else
  {
    return Py_BuildValue("s", str);
  }
}

/*
receives and returns an int type
1 = enabled
0 = disabled
*/
PyObject * option_get_usesyslog_alpm(PyObject *self)
{
  unsigned short str = alpm_option_get_usesyslog();
  
  if(str == -1)
  {
    PyErr_SetString(alpm_error, "failed getting usesyslog");
    return NULL;
  }
  else
  {
    return Py_BuildValue("i", str);
  }
}

PyObject * option_set_usesyslog_alpm(PyObject *self, PyObject *args)
{
  const unsigned short *str;
  if(!PyArg_ParseTuple(args, "i", &str))
  {
    PyErr_SetString(alpm_error, "wrong arguments");
    return NULL;
  }
  else
  {
    alpm_option_set_usesyslog(str);
    return Py_None;
  }
}

PyObject * option_get_nopassiveftp_alpm(PyObject *self)
{
  unsigned short str = alpm_option_get_nopassiveftp();
  
  if(str == -1)
  {
    PyErr_SetString(alpm_error, "failed getting nopassiveftp");
    return NULL;
  }
  else
  {
    return Py_BuildValue("i", str);
  }
}

PyObject * option_set_nopassiveftp_alpm(PyObject *self, PyObject *args)
{
  const unsigned short *str;
  if(!PyArg_ParseTuple(args, "i", &str))
  {
    PyErr_SetString(alpm_error, "wrong arguments");
    return NULL;
  }
  else
  {
    alpm_option_set_usesyslog(str);
    return Py_None;
  }
}

/*write only function*/

PyObject * option_set_usedelta_alpm(PyObject *self, PyObject *args)
{
  const unsigned short *str;
  if(!PyArg_ParseTuple(args, "i", &str))
  {
    PyErr_SetString(alpm_error, "wrong arguments");
    return NULL;
  }
  else
  {
    alpm_option_set_usedelta(str);
    return Py_None;
  }
}

/*read-only functions*/

PyObject * option_get_lockfile_alpm(PyObject *self)
{
  const char *str = NULL;
  str = alpm_option_get_lockfile();
  
  if(str == NULL)
  {
    PyErr_SetString(alpm_error, "failed getting lockfile");
    return NULL;
  }
  else
  {
    return Py_BuildValue("s", str);
  }
}

PyObject * option_set_noupgrades_alpm(PyObject *self, PyObject *args)
{
  alpm_list_t *target, *adtmp;
  PyObject *tmp;
  
  if(!PyArg_ParseTuple(args, "O", &tmp))
  {
    PyErr_SetString(alpm_error, "error in the args.");
    return NULL;
  }
  else
  {
    target = tuple_alpm_list_t(tmp);
    adtmp = addresses;
    addresses->data = target;
    add_alpm_list_t(addresses);
    addresses->prev = adtmp;
    alpm_option_set_noupgrades(target);
    return Py_None;
  }
}

PyObject * option_add_noupgrade_alpm(PyObject *self, PyObject *args)
{
  const char *str;
  
  if(!PyArg_ParseTuple(args, "s", &str))
  {
    PyErr_SetString(alpm_error, "wrong arguments");
    return NULL;
  }
  else
  {
    alpm_option_add_noupgrade(str);
    return Py_None;
  }
}

PyObject * option_remove_noupgrade_alpm(PyObject *self, PyObject *args)
{
  const char *str;
  
  if(!PyArg_ParseTuple(args, "s", &str))
  {
    PyErr_SetString(alpm_error, "wrong arguments");
    return NULL;
  }
  else
  {
    alpm_option_remove_noupgrade(str);
    return Py_None;
  }
}

PyObject * option_add_cachedir_alpm(PyObject *self, PyObject *args)
{
  const char *str;
  
  if(!PyArg_ParseTuple(args, "s", &str))
  {
    PyErr_SetString(alpm_error, "wrong arguments");
    return NULL;
  }
  else
  {
    alpm_option_add_cachedir(str);
    return Py_None;
  }
}

PyObject * option_remove_cachedir_alpm(PyObject *self, PyObject *args)
{
  const char *str;
  
  if(!PyArg_ParseTuple(args, "s", &str))
  {
    PyErr_SetString(alpm_error, "wrong arguments");
    return NULL;
  }
  else
  {
    alpm_option_remove_cachedir(str);
    return Py_None;
  }
}

PyObject * option_add_noextract_alpm(PyObject *self, PyObject *args)
{
  const char *str;
  
  if(!PyArg_ParseTuple(args, "s", &str))
  {
    PyErr_SetString(alpm_error, "wrong arguments");
    return NULL;
  }
  else
  {
    alpm_option_add_noextract(str);
    return Py_None;
  }
}

PyObject * option_remove_noextract_alpm(PyObject *self, PyObject *args)
{
  const char *str;
  
  if(!PyArg_ParseTuple(args, "s", &str))
  {
    PyErr_SetString(alpm_error, "wrong arguments");
    return NULL;
  }
  else
  {
    alpm_option_remove_noextract(str);
    return Py_None;
  }
}

PyObject * option_add_ignorepkg_alpm(PyObject *self, PyObject *args)
{
  const char *str;
  
  if(!PyArg_ParseTuple(args, "s", &str))
  {
    PyErr_SetString(alpm_error, "wrong arguments");
    return NULL;
  }
  else
  {
    alpm_option_add_ignorepkg(str);
    return Py_None;
  }
}

PyObject * option_remove_ignorepkg_alpm(PyObject *self, PyObject *args)
{
  const char *str;
  if(!PyArg_ParseTuple(args, "s", &str))
  {
    PyErr_SetString(alpm_error, "wrong arguments");
    return NULL;
  }
  else
  {
    alpm_option_remove_ignorepkg(str);
    return Py_None;
  }
}

PyObject * alpmversion_alpm(PyObject *self)
{
  const char *str;
  str = alpm_version();
  
  return Py_BuildValue("s", str);
}

PyObject * version_alpm(PyObject *self)
{
  return Py_BuildValue("s", VERSION);
}

PyObject * check_init_alpm(PyObject *self)
{
  if(init == 0)
  {
    return Py_False;
  }
  if(init == 1)
  {
    return Py_True;
  }
  else
  {
    PyErr_SetString(alpm_error, "internal error");
    return NULL;
  }
}

/*internal data type converters*/
pmdb_t * tuple_pmdb_t(char *dbpath, char *dbtreename, alpm_list_t *pkgcache,
			    alpm_list_t *grpcache, alpm_list_t *servers)
{
  pmdb_t *result;
  
  result = (pmdb_t*) malloc(sizeof(pmdb_t));
  
  result->path = dbpath;
  result->treename = dbtreename;
  result->pkgcache = pkgcache;
  result->grpcache = grpcache;
  result->servers = servers;
  
  return result;
}

void clean_pmdb_t(pmdb_t *ptr)
{
  clean_alpm_list_t(ptr->pkgcache);
  clean_alpm_list_t(ptr->grpcache);
  clean_alpm_list_t(ptr->servers);
  free(ptr->treename);
  free(ptr->path);
  free(ptr);
}

PyObject * testconverter(PyObject *self, PyObject *args)
{
  const char *path, *dbtreename;
  alpm_list_t *pkgcache, *grpcache, *servers;
  pmdb_t *test;
  PyObject *pkgtmp, *grptmp, *srvtmp;
  
  if(!PyArg_ParseTuple(args, "ssOOO", &path, &dbtreename, &pkgtmp, &grptmp, &srvtmp))
  {
    PyErr_SetString(alpm_error, "bad arguments");
    return NULL;
  }
  else
  {
    pkgcache = tuple_alpm_list_t(pkgtmp);
    grpcache = tuple_alpm_list_t(grptmp);
    servers = tuple_alpm_list_t(srvtmp);
    test = tuple_pmdb_t(path, dbtreename, pkgcache, grpcache, servers);
    clean_alpm_list_t(pkgcache);
    clean_alpm_list_t(grpcache);
    clean_alpm_list_t(servers);
    return Py_BuildValue("s", test->path);
  }
}
/*converts a C array to alpm_list_t linked list, returns a pointer to first node*/
alpm_list_t * tuple_alpm_list_t(PyObject *list)
{
  alpm_list_t *nodetmp;
  char *tmp;
  /*int i, num;*/
  PyObject *iterator = PyObject_GetIter(list);
  PyObject *item;
  
  if(iterator == NULL)
  {
    nodetmp = NULL;
    return nodetmp;
  }
/*  
  num = PySequence_Lenght(list);
  if(num < 0)
  {
    nodetemp = NULL;
    return nodetmp;
  }
*/  
  nodetmp = (alpm_list_t*) malloc(sizeof(alpm_list_t));
  
  while(item = PyIter_Next(iterator))
  {
    tmp = (char*) malloc(sizeof(item));
    nodetmp->data = tmp;
    strcpy(nodetmp->data, item);
    /*printf("%s\n", (char*) *nodetmp->data);*/
    add_alpm_list_t(nodetmp);
    Py_DECREF(item);
  }
  Py_DECREF(iterator);
  
/*  for(i=0;i<num;i++)
  {
    item = PySequence_GetItem(list, i);
    tmp = NULL;
    tmp = (char*) malloc(sizeof(item));
    if(tmp == NULL)
    {
      nodetmp = NULL;
      return nodetmp;
      Py_DECREF(item);
    }
    else
    {
      nodetmp->data = tmp;
      strcpy(nodetmp->data, item);
      if(i != num-1)
      {
      add_alpm_list_t(nodetmp);
      nodetmp = nodetmp->next;
      }
      Py_DECREF(item);
    }
  }
*/  
  return nodetmp;
}

PyObject * alpm_list_t_tuple(alpm_list_t *prt)
{
  PyObject * output;
  
  while(prt != NULL)
  {
    
    prt = prt->next;
  }
}

/*alpm_list_t related functions*/
void add_alpm_list_t(alpm_list_t *prt)
{
  alpm_list_t *new;
  new = (alpm_list_t*) malloc(sizeof(alpm_list_t));
  
  prt->next = new;
  new->prev = prt;
}

void remove_alpm_list_t(alpm_list_t *prt)
{
  alpm_list_t *old;
  
  old = prt->prev;
  old->next = NULL;
  
  free(prt->data);
  free(prt);
}

void clean_alpm_list_t(alpm_list_t *prt)
{
  alpm_list_t *tmp, *tmp2;
  
  tmp = prt;
  tmp2 = prt->next;
  do
  {
    remove_alpm_list_t(tmp);
    tmp = tmp2;
    tmp2 = tmp->next;
  } while(tmp2 != NULL);
}

void clean_memory(alpm_list_t *ptr)
{
  alpm_list_t *tmp;
  
  while(ptr != NULL)
  {
    free(ptr->data);
    tmp = ptr;
    ptr = ptr->prev;
    free(tmp);
  }
  free(ptr);
}

PyMethodDef methods[] = {
  {"testconv", testconverter, METH_VARARGS, "test type converter."},
  {"initialize", initialize_alpm, METH_VARARGS, "initialize alpm."},
  {"release", release_alpm, METH_VARARGS, "release alpm."},
  {"getlogcb", option_get_logcb_alpm, METH_VARARGS, "call back function for logging."},
  {"setlogcb", option_set_logcb_alpm, METH_VARARGS, "sets logging config."},
  {"getroot", option_get_root_alpm, METH_VARARGS, "gets the root path."},
  {"setroot", option_set_root_alpm, METH_VARARGS, "sets the root path."},
  {"getdbpath", option_get_dbpath_alpm, METH_VARARGS, "gets the dbpath."},
  {"setdbpath", option_set_dbpath_alpm, METH_VARARGS, "sets the dbpath."},
  {"getlogfile", option_get_logfile_alpm, METH_VARARGS, "gets the logfile."},
  {"setlogfile", option_set_logfile_alpm, METH_VARARGS, "sets the logfile."},
  {"getlockfile", option_get_lockfile_alpm, METH_VARARGS, "gets the lockfile."},
  {"getxfercommand", option_get_xfercommand_alpm, METH_VARARGS, "gets the xfercommand value."},
  {"setxfercommand", option_set_xfercommand_alpm, METH_VARARGS, "sets the xfercommand value."},
  {"getusesyslog", option_get_usesyslog_alpm, METH_VARARGS, "gets usesyslog value."},
  {"setusesyslog", option_set_usesyslog_alpm, METH_VARARGS, "sets usesyslog value."},
  {"getnopassiveftp", option_get_nopassiveftp_alpm, METH_VARARGS, "gets nopassiveftp value."},
  {"setnopassiveftp", option_set_nopassiveftp_alpm, METH_VARARGS, "sets nopassiveftp value."},
  {"setusedelta", option_set_usedelta_alpm, METH_VARARGS, "sets usedelta value."},
  {"setnoupgrades", option_set_noupgrades_alpm, METH_VARARGS, "sets noupgrades."},
  {"addnoupgrade", option_add_noupgrade_alpm, METH_VARARGS, "add a noupgrade package."},
  {"removenoupgrade", option_remove_noupgrade_alpm, METH_VARARGS, "removes a noupgrade package."},
  {"addcachedir", option_add_cachedir_alpm, METH_VARARGS, "adds a cachedir."},
  {"removecachedir", option_remove_cachedir_alpm, METH_VARARGS, "removes a cachedir."},
  {"addnoextract", option_add_noextract_alpm, METH_VARARGS, "add a noextract package."},
  {"removenoextract", option_remove_noextract_alpm, METH_VARARGS, "remove a noextract package."},
  {"addignorepkg", option_add_ignorepkg_alpm, METH_VARARGS, "add an ignorepkg."},
  {"removeignorepkg", option_remove_ignorepkg_alpm, METH_VARARGS, "remove an ignorepkg."},
  {"version", version_alpm, METH_VARARGS, "returns pyalpm version."},
  {"alpmversion", alpmversion_alpm, METH_VARARGS, "returns alpm version."},
  {"checkinit", check_init_alpm, METH_VARARGS, "checks if the library was initialized."},
  {NULL, NULL, 0, NULL}
};


PyMODINIT_FUNC initpyalpm()
{
  (void) Py_InitModule("pyalpm", methods);
  
  alpm_error = PyErr_NewException("alpm.error", NULL, NULL);
}