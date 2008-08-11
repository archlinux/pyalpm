#include <Python.h>

#include <alpm.h>
#include <alpm_list.h>

static PyObject *alpm_error = NULL;

static PyObject * initialize_alpm(PyObject *self)
{
  if(alpm_initialize() == -1)
  {
    PyErr_SetString(alpm_error, "failed to initialize alpm");
    return NULL;
  }
  else
  {
    return Py_None;
  }
}

static PyObject * release_alpm(PyObject *self)
{
  if(alpm_release() == -1)
  {
    PyErr_SetString(alpm_error, "failed to release alpm");
    return NULL;
  }
  else
  {
    return Py_None;
  }
}

static PyObject * option_get_logcb_alpm(PyObject *self)
{
  if(alpm_option_get_logcb() == NULL)
  {
    PyErr_SetString(alpm_error, "failed getting logcb");
    return NULL;
  }
  else{
    return Py_None;
  }
}

static PyObject * option_get_root_alpm(PyObject *self)
{
  if(alpm_option_get_root() == NULL)
  {
    PyErr_SetString(alpm_error, "failed getting root path");
    return NULL;
  }
  else
  {
    return Py_None;
  }
}

static PyObject * option_set_root_alpm(PyObject *self, PyObject *args)
{
  const char *path;

  if(!PyArg_ParseTuple(args, "s", &path))
  {
    return NULL;
  }
  
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

PyMethodDef methods[] = {
  {"initialize", initialize_alpm, METH_VARARGS, "initialize alpm."},
  {"release", release_alpm, METH_VARARGS, "release alpm."},
  {"getlogcb", option_get_logcb_alpm, METH_VARARGS, "call back function for logging."},
  {"getroot", option_get_root_alpm, METH_VARARGS, "gets the root path."},
  {"setroot", option_set_root_alpm, METH_VARARGS, "sets the root path."},
  {NULL, NULL, 0, NULL}
};


PyMODINIT_FUNC initpyalpm()
{
  (void) Py_InitModule("pyalpm", methods);
  
  alpm_error = PyErr_NewException("alpm.error", NULL, NULL);
}