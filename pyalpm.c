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

unsigned short check_init(void)
{
  switch(init)
  {
    case 0:
      return 0;
    case 1:
      return 1;
   default:
      return -1;
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
    return Py_BuildValue("s", test->path);
  }
}
/*converts a Python array to alpm_list_t linked list, returns a pointer to first node*/
alpm_list_t * tuple_alpm_list_t(PyObject *list)
{
  char *tmp, *pystring;
  alpm_list_t *nodetmp, *ret;
  PyObject *iterator = PyObject_GetIter(list);
  PyObject *item;
  
  
  if(iterator == NULL)
  {
    return NULL;
  }
  
  nodetmp = (alpm_list_t*) malloc(sizeof(alpm_list_t));
  ret = nodetmp;
  
  while((item = PyIter_Next(iterator)))
  {
    if(PyString_Check(item))
    {
      tmp = (char*) malloc(sizeof(*PyString_AsString(item)));
      
      strcpy(tmp, PyString_AsString(item));
      
      nodetmp->data=tmp;
      /*nodetmp->data=PyString_AsString(item);*/
      printf("%s\n", nodetmp->data);
    }
    else
    {
      return NULL;
    }
    add_alpm_list_t(nodetmp);
    Py_DECREF(item);
  }
  Py_DECREF(iterator);
    
  return ret;
}

PyObject * alpm_list_t_tuple(alpm_list_t *prt)
{
  PyObject *output, *strtmp;
  alpm_list_t *tmp;
  
  tmp = prt;
  
  if(tmp != NULL)
  {
    output = PyList_New(0);
    if(output != NULL)
    {
      strtmp = Py_BuildValue("s", tmp->data);
      
      
      tmp = tmp->next;
      
      PyList_Append(output, strtmp);
    }
  
    while(tmp != NULL)
    {
      
        strtmp = Py_BuildValue("s", tmp->data);
        
        PyList_Append(output, strcmp);
        
        tmp = tmp->next;
    }
    
  }
  else
  {
    output = NULL;
  }
  
  return output;
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

PyMethodDef methods[] = {
  {"testconv", testconverter, METH_VARARGS, "test type converter."},
  {"initialize", initialize_alpm, METH_VARARGS, "initialize alpm."},
  {"release", release_alpm, METH_VARARGS, "release alpm."},
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
