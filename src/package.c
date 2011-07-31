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

#include <string.h>
#include <alpm.h>
#include <Python.h>
#include "db.h"
#include "util.h"
#include "handle.h"

typedef struct _AlpmPackage {
  PyObject_HEAD
  alpm_pkg_t *c_data;
  int needs_free;
} AlpmPackage;

PyTypeObject AlpmPackageType;
extern PyTypeObject AlpmHandleType;

int PyAlpmPkg_Check(PyObject *object) {
  return PyObject_TypeCheck(object, &AlpmPackageType);
}

static PyObject* pyalpm_pkg_repr(PyObject *rawself) {
  AlpmPackage *self = (AlpmPackage *)rawself;
  return PyUnicode_FromFormat("<alpm.Package(\"%s-%s-%s\") at %p>",
			      alpm_pkg_get_name(self->c_data),
			      alpm_pkg_get_version(self->c_data),
			      alpm_pkg_get_arch(self->c_data),
			      self);
}

static PyObject* pyalpm_pkg_str(PyObject *rawself) {
  AlpmPackage *self = (AlpmPackage *)rawself;
  return PyUnicode_FromFormat("alpm.Package(\"%s-%s-%s\")",
			      alpm_pkg_get_name(self->c_data),
			      alpm_pkg_get_version(self->c_data),
			      alpm_pkg_get_arch(self->c_data));
}

void pyalpm_pkg_unref(PyObject *object) {
  if (PyAlpmPkg_Check(object))
    ((AlpmPackage*)(object))->needs_free = 0;
}

static void pyalpm_package_dealloc(AlpmPackage *self) {
  if (self->needs_free)
    alpm_pkg_free(self->c_data);
  Py_TYPE(self)->tp_free((PyObject*)self);
}

/* Internal utility functions */

static PyObject* _pyobject_from_pmdepend(void* dep) {
  char *depstring = alpm_dep_compute_string((alpm_depend_t*)dep);
  PyObject *item = Py_BuildValue("s", depstring);
  free(depstring);
  return item;
};

PyObject *pyalpm_package_from_pmpkg(void* data) {
  AlpmPackage *self;
  alpm_pkg_t *p = (alpm_pkg_t*)data;
  self = (AlpmPackage*)AlpmPackageType.tp_alloc(&AlpmPackageType, 0);
  if (self == NULL) {
    PyErr_SetString(PyExc_RuntimeError, "unable to create package object");
    return NULL;
  }

  self->c_data = p;
  self->needs_free = 0;
  return (PyObject *)self;
}

#define CHECK_IF_INITIALIZED() if (! self->c_data) { \
  PyErr_SetString(alpm_error, "data is not initialized"); \
  return NULL; \
  }

alpm_pkg_t *pmpkg_from_pyalpm_pkg(PyObject *object) {
  AlpmPackage *self = (AlpmPackage*)object;
  CHECK_IF_INITIALIZED();
  return self->c_data;
}

/** Converts a Python list of packages to an alpm_list_t linked list.
 * return 0 on success, -1 on failure
 */
int pylist_pkg_to_alpmlist(PyObject *list, alpm_list_t **result) {
  alpm_list_t *ret = NULL;
  PyObject *iterator = PyObject_GetIter(list);
  PyObject *item;

  if(iterator == NULL) {
    PyErr_SetString(PyExc_TypeError, "object is not iterable");
    return -1;
  }

  while((item = PyIter_Next(iterator)))
  {
    if (PyObject_TypeCheck(item, &AlpmPackageType)) {
      ret = alpm_list_add(ret, ((AlpmPackage*)item)->c_data);
    } else {
      PyErr_SetString(PyExc_TypeError, "list must contain only Package objects");
      FREELIST(ret);
      return -1;
    }
    Py_DECREF(item);
  }
  Py_DECREF(iterator);

  *result = ret;
  return 0;
}

/* Python bindings */

PyObject *pyalpm_package_load(PyObject *self, PyObject *args, PyObject *kwargs) {
  char *filename;
  int check_sig = ALPM_SIG_PACKAGE_OPTIONAL;
  char *kws[] = { "handle", "path", "check_sig", NULL };
  PyObject *pyhandle = NULL;
  alpm_handle_t *handle;
  alpm_pkg_t *result;
  AlpmPackage *pyresult;
  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O!s|i:load_pkg", kws,
       &AlpmHandleType, &pyhandle, &filename, &check_sig)) {
    return NULL;
  }

  handle = ALPM_HANDLE(pyhandle);

  if ((alpm_pkg_load(handle, filename, 1, check_sig, &result) == -1) || !result) {
    RET_ERR("loading package failed", alpm_errno(handle), NULL);
  }

  pyresult = (AlpmPackage*)pyalpm_package_from_pmpkg(result);
  if (!pyresult) return NULL;
  pyresult->needs_free = 1;
  return (PyObject*)pyresult;
}

static PyObject *pyalpm_package_get_filename(AlpmPackage *self, void *closure) {
  const char *filename;

  CHECK_IF_INITIALIZED();

  filename = alpm_pkg_get_filename(self->c_data);
  if (filename == NULL) {
    Py_RETURN_NONE;
  }
  return Py_BuildValue("s", filename);
}

static PyObject* pyalpm_package_get_name(AlpmPackage *self, void *closure) {
  const char *name;

  CHECK_IF_INITIALIZED();

  name = alpm_pkg_get_name(self->c_data);
  if (name == NULL) {
    PyErr_SetString(PyExc_RuntimeError, "unable to get name");
    return NULL;
  }
  return Py_BuildValue("s", name);
}

static PyObject* pyalpm_package_get_version(AlpmPackage *self, void *closure) {
  const char *version;

  CHECK_IF_INITIALIZED();

  version = alpm_pkg_get_version(self->c_data);
  if (version == NULL) {
    PyErr_SetString(PyExc_RuntimeError, "unable to get version");
    return NULL;
  }
  return Py_BuildValue("s", version);
}

static PyObject* pyalpm_package_get_desc(AlpmPackage *self, void *closure) {
  const char *desc;

  CHECK_IF_INITIALIZED();

  desc = alpm_pkg_get_desc(self->c_data);
  if (desc == NULL) {
    PyErr_SetString(PyExc_RuntimeError, "unable to get desc");
    return NULL;
  }
  return Py_BuildValue("s", desc);
}

static PyObject* pyalpm_package_get_url(AlpmPackage *self, void *closure) {
  const char *url;

  CHECK_IF_INITIALIZED();

  url = alpm_pkg_get_url(self->c_data);
  if (url == NULL) {
    PyErr_SetString(PyExc_RuntimeError, "unable to get url");
    return NULL;
  }
  return Py_BuildValue("s", url);
}


static PyObject* pyalpm_package_get_builddate(AlpmPackage *self, void *closure) {
  CHECK_IF_INITIALIZED();
  return PyLong_FromLong(alpm_pkg_get_builddate(self->c_data));
}

static PyObject* pyalpm_package_get_installdate(AlpmPackage *self, void *closure) {
  CHECK_IF_INITIALIZED();
  return PyLong_FromLong(alpm_pkg_get_installdate(self->c_data));
}

static PyObject* pyalpm_package_get_packager(AlpmPackage *self, void *closure) {
  const char *packager;

  CHECK_IF_INITIALIZED();

  packager = alpm_pkg_get_packager(self->c_data);
  if (packager == NULL) {
    PyErr_SetString(PyExc_RuntimeError, "unable to get packager");
    return NULL;
  }
  return Py_BuildValue("s", packager);
}

static PyObject* pyalpm_package_get_md5sum(AlpmPackage *self, void *closure) {
  const char *md5sum;

  CHECK_IF_INITIALIZED();

  md5sum = alpm_pkg_get_md5sum(self->c_data);
  if (md5sum == NULL)
    Py_RETURN_NONE;
  return Py_BuildValue("s", md5sum);
}

static PyObject* pyalpm_package_get_arch(AlpmPackage *self, void *closure) {
  const char *arch;

  CHECK_IF_INITIALIZED();

  arch = alpm_pkg_get_arch(self->c_data);
  if (arch == NULL) {
    PyErr_SetString(PyExc_RuntimeError, "unable to get arch");
    return NULL;
  }
  return Py_BuildValue("s", arch);
}

static PyObject* pyalpm_package_get_size(AlpmPackage *self, void *closure) {
  CHECK_IF_INITIALIZED();
  return PyLong_FromLong(alpm_pkg_get_size(self->c_data));
}

static PyObject* pyalpm_package_get_isize(AlpmPackage *self, void *closure) {
  CHECK_IF_INITIALIZED();
  return PyLong_FromLong(alpm_pkg_get_isize(self->c_data));
}

static PyObject* pyalpm_package_get_reason(AlpmPackage *self, void *closure) {
  CHECK_IF_INITIALIZED();
  return PyLong_FromLong(alpm_pkg_get_reason(self->c_data));
}

static PyObject* pyalpm_package_get_licenses(AlpmPackage *self, void *closure) {
  alpm_list_t *licenses = NULL;

  CHECK_IF_INITIALIZED();

  licenses = alpm_pkg_get_licenses(self->c_data);
  return alpmlist_to_pylist(licenses, pyobject_from_string);
}

static PyObject* pyalpm_package_get_groups(AlpmPackage *self, void *closure) {
  alpm_list_t *groups = NULL;

  CHECK_IF_INITIALIZED();

  groups = alpm_pkg_get_groups(self->c_data);
  return alpmlist_to_pylist(groups, pyobject_from_string);
}

static PyObject* pyalpm_package_get_depends(AlpmPackage *self, void *closure) {
  alpm_list_t *depends = NULL;

  CHECK_IF_INITIALIZED();

  depends = alpm_pkg_get_depends(self->c_data);
  return alpmlist_to_pylist(depends, _pyobject_from_pmdepend);
}

static PyObject* pyalpm_package_get_optdepends(AlpmPackage *self, void *closure) {
  alpm_list_t *optdepends = NULL;

  CHECK_IF_INITIALIZED();

  optdepends = alpm_pkg_get_optdepends(self->c_data);
  return alpmlist_to_pylist(optdepends, pyobject_from_string);
}

static PyObject* pyalpm_package_get_conflicts(AlpmPackage *self, void *closure) {
  alpm_list_t *conflicts = NULL;

  CHECK_IF_INITIALIZED();

  conflicts = alpm_pkg_get_conflicts(self->c_data);
  return alpmlist_to_pylist(conflicts, pyobject_from_string);
}

static PyObject* pyalpm_package_get_provides(AlpmPackage *self, void *closure) {
  alpm_list_t *provides = NULL;

  CHECK_IF_INITIALIZED();

  provides = alpm_pkg_get_provides(self->c_data);
  return alpmlist_to_pylist(provides, pyobject_from_string);
}

static PyObject* pyalpm_package_get_deltas(AlpmPackage *self, void *closure) {
  CHECK_IF_INITIALIZED();
  return alpmlist_to_pylist(alpm_pkg_get_deltas(self->c_data), pyobject_from_string);
}

static PyObject* pyalpm_package_get_replaces(AlpmPackage *self, void *closure) {
  alpm_list_t *replaces = NULL;

  CHECK_IF_INITIALIZED();

  replaces = alpm_pkg_get_replaces(self->c_data);
  return alpmlist_to_pylist(replaces, pyobject_from_string);
}

static PyObject* pyalpm_package_get_files(AlpmPackage *self, void *closure) {
  const alpm_filelist_t *flist = NULL;
  PyObject *result = NULL;

  CHECK_IF_INITIALIZED();

  flist = alpm_pkg_get_files(self->c_data);
  if (!flist)
    Py_RETURN_NONE;
  else {
    int i;
    result = PyList_New(flist->count);
    for (i = 0; i < flist->count; i++) {
      const alpm_file_t *file = flist->files + i;
      PyObject *item = Py_BuildValue("(sii)", file->name, file->size, file->mode);
      if (!item) {
        Py_CLEAR(result);
        return NULL;
      }
      PyList_SET_ITEM(result, i, item);
    }
  }
  return result;
}

/** Convert backup file strings to Python tuples
 * Strings "file\tmd5sum" are converted to ("file", "md5sum")
 * Strings "file" are converted to ("file", None)
 */
static PyObject* _pytuple_from_tab_separated(void* data) {
  char* s = (char *)data;
  char* sep = strchr(s, '\t');
  PyObject* tuple;
  if (!sep) sep = s + strlen(s);
  {
    PyObject* fst;
    PyObject* snd;
    fst = PyUnicode_FromStringAndSize(s, sep - s);
    if (*sep != '\0')
      snd = PyUnicode_FromString(sep + 1);
    else {
      Py_INCREF(Py_None);
      snd = Py_None;
    }
    tuple = PyTuple_Pack(2, fst, snd);
    Py_DECREF(fst);
    Py_DECREF(snd);
  }
  return tuple;
}

static PyObject* pyalpm_package_get_backup(AlpmPackage *self, void *closure) {
  CHECK_IF_INITIALIZED();
  return alpmlist_to_pylist(alpm_pkg_get_backup(self->c_data),
			    _pytuple_from_tab_separated);
}

static PyObject* pyalpm_package_get_db(AlpmPackage *self, void *closure) {
  alpm_db_t* db;
  CHECK_IF_INITIALIZED();
  db = alpm_pkg_get_db(self->c_data);
  if (db)
    return pyalpm_db_from_pmdb(db);
  else
    Py_RETURN_NONE;
}

static PyObject* pyalpm_pkg_has_scriptlet(AlpmPackage *self, void *closure) {
  CHECK_IF_INITIALIZED();
  return PyBool_FromLong(alpm_pkg_has_scriptlet(self->c_data));
}

static PyObject* pyalpm_pkg_download_size(AlpmPackage *self, void *closure) {
  CHECK_IF_INITIALIZED();
  return PyLong_FromLong(alpm_pkg_download_size(self->c_data));
}

static PyObject* pyalpm_pkg_compute_requiredby(PyObject *rawself, PyObject *args) {
  AlpmPackage *self = (AlpmPackage*)rawself;
  PyObject *pyresult;
  CHECK_IF_INITIALIZED();
  {
    alpm_list_t *result = alpm_pkg_compute_requiredby(self->c_data);
    pyresult = alpmlist_to_pylist(result, pyobject_from_string);
    FREELIST(result);
  }
  return pyresult;
}

static struct PyGetSetDef AlpmPackageGetSet[] = {
  { "db", (getter)pyalpm_package_get_db, 0, "the database from which the package comes from, or None", NULL } ,
  /* description properties */
  { "name", (getter)pyalpm_package_get_name, 0, "package name", NULL } ,
  { "version", (getter)pyalpm_package_get_version, 0, "package version", NULL } ,
  { "desc", (getter)pyalpm_package_get_desc, 0, "package desc", NULL } ,
  { "url", (getter)pyalpm_package_get_url, 0, "package URL", NULL } ,
  { "arch", (getter)pyalpm_package_get_arch, 0, "target architecture", NULL } ,
  { "licenses", (getter)pyalpm_package_get_licenses, 0, "list of licenses", NULL } ,
  { "groups", (getter)pyalpm_package_get_groups, 0, "list of package groups", NULL } ,
  /* package properties */
  { "packager", (getter)pyalpm_package_get_packager, 0, "packager name", NULL } ,
  { "md5sum", (getter)pyalpm_package_get_md5sum, 0, "package md5sum", NULL } ,
  { "filename", (getter)pyalpm_package_get_filename, 0, "package filename", NULL } ,
  { "size", (getter)pyalpm_package_get_size, 0, "package size", NULL } ,
  { "isize", (getter)pyalpm_package_get_isize, 0, "installed size", NULL } ,
  { "reason", (getter)pyalpm_package_get_reason, 0, "install reason (0 = explicit, 1 = depend)", NULL } ,
  { "builddate", (getter)pyalpm_package_get_builddate, 0, "building time", NULL } ,
  { "installdate", (getter)pyalpm_package_get_installdate, 0, "install time", NULL } ,
  { "files", (getter)pyalpm_package_get_files, 0, "list of installed files", NULL } ,
  { "backup", (getter)pyalpm_package_get_backup, 0, "list of tuples (filename, md5sum)", NULL } ,
  { "deltas", (getter)pyalpm_package_get_deltas, 0, "list of available deltas", NULL } ,
  /* dependency information */
  { "depends", (getter)pyalpm_package_get_depends, 0, "list of dependencies", NULL } ,
  { "optdepends", (getter)pyalpm_package_get_optdepends, 0, "list of optional dependencies", NULL } ,
  { "conflicts", (getter)pyalpm_package_get_conflicts, 0, "list of conflicts", NULL } ,
  { "provides", (getter)pyalpm_package_get_provides, 0, "list of provided package names", NULL } ,
  { "replaces", (getter)pyalpm_package_get_replaces, 0, "list of replaced packages", NULL } ,
  /* miscellaneous information */
  { "has_scriptlet", (getter)pyalpm_pkg_has_scriptlet, 0, "True if the package has an install script", NULL },
  { "download_size", (getter)pyalpm_pkg_download_size, 0, "predicted download size for this package", NULL },
  { NULL }
};

static struct PyMethodDef pyalpm_pkg_methods[] = {
  { "compute_requiredby", pyalpm_pkg_compute_requiredby, METH_NOARGS,
      "computes the list of packages requiring this package" },
  { NULL }
};

PyTypeObject AlpmPackageType = {
  PyVarObject_HEAD_INIT(NULL, 0)
  "alpm.Package",             /*tp_name*/
  sizeof(AlpmPackage),        /*tp_basicsize*/
  0,                          /*tp_itemsize*/
  .tp_dealloc = (destructor)pyalpm_package_dealloc,
  .tp_repr = pyalpm_pkg_repr,
  .tp_str = pyalpm_pkg_str,
  .tp_flags = Py_TPFLAGS_DEFAULT,
  .tp_doc = "Package object",
  .tp_methods = pyalpm_pkg_methods,
  .tp_getset = AlpmPackageGetSet,
};

/** Initializes Package class in module */
void init_pyalpm_package(PyObject *module) {
  PyObject *type;

  if (PyType_Ready(&AlpmPackageType) < 0)
    return;
  type = (PyObject*)&AlpmPackageType;
  Py_INCREF(type);
  PyModule_AddObject(module, "Package", type);

  /* package reasons */
  PyModule_AddIntConstant(module, "PKG_REASON_EXPLICIT", ALPM_PKG_REASON_EXPLICIT);
  PyModule_AddIntConstant(module, "PKG_REASON_DEPEND", ALPM_PKG_REASON_DEPEND);

  /* signature check levels */
  PyModule_AddIntConstant(module, "SIG_PACKAGE", ALPM_SIG_PACKAGE);
  PyModule_AddIntConstant(module, "SIG_PACKAGE_OPTIONAL", ALPM_SIG_PACKAGE_OPTIONAL);
  PyModule_AddIntConstant(module, "SIG_PACKAGE_MARGINAL_OK", ALPM_SIG_PACKAGE_MARGINAL_OK);
  PyModule_AddIntConstant(module, "SIG_PACKAGE_UNKNOWN_OK", ALPM_SIG_PACKAGE_UNKNOWN_OK);
}

/* vim: set ts=2 sw=2 et: */
