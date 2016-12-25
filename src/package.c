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

#include <pyconfig.h>
#include <string.h>
#include <alpm.h>
#include <Python.h>
#include "db.h"
#include "util.h"
#include "handle.h"
#include "package.h"

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

static PyObject* _get_string_attribute(AlpmPackage *self, const char* getter(alpm_pkg_t*)) {
  const char *attr;
  if (! self->c_data) {
    PyErr_SetString(alpm_error, "data is not initialized");
    return NULL;
  }
  attr = getter(self->c_data);
  if (attr == NULL) Py_RETURN_NONE;
  return Py_BuildValue("s", attr);
}

struct list_getter {
  alpm_list_t *(*getter)(alpm_pkg_t *);
  PyObject *(*item_converter)(void *);
};

/** Returns a Python list attribute */
static PyObject* _get_list_attribute(AlpmPackage *self, const struct list_getter *closure) {
  alpm_list_t *result = NULL;
  CHECK_IF_INITIALIZED();
  result = closure->getter(self->c_data);
  return alpmlist_to_pylist(result, closure->item_converter);
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
  alpm_handle_t *handle = ALPM_HANDLE(self);
  char *filename;
  int check_sig = ALPM_SIG_PACKAGE_OPTIONAL;
  char *kws[] = { "path", "check_sig", NULL };
  alpm_pkg_t *result;
  AlpmPackage *pyresult;
  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "s|i:load_pkg", kws, &filename, &check_sig)) {
    return NULL;
  }

  if ((alpm_pkg_load(handle, filename, 1, check_sig, &result) == -1) || !result) {
    RET_ERR("loading package failed", alpm_errno(handle), NULL);
  }

  pyresult = (AlpmPackage*)pyalpm_package_from_pmpkg(result);
  if (!pyresult) return NULL;
  pyresult->needs_free = 1;
  return (PyObject*)pyresult;
}

static PyObject* pyalpm_package_get_builddate(AlpmPackage *self, void *closure) {
  CHECK_IF_INITIALIZED();
  return PyLong_FromLongLong(alpm_pkg_get_builddate(self->c_data));
}

static PyObject* pyalpm_package_get_installdate(AlpmPackage *self, void *closure) {
  CHECK_IF_INITIALIZED();
  return PyLong_FromLongLong(alpm_pkg_get_installdate(self->c_data));
}

static PyObject* pyalpm_package_get_size(AlpmPackage *self, void *closure) {
  CHECK_IF_INITIALIZED();
  return PyLong_FromLongLong(alpm_pkg_get_size(self->c_data));
}

static PyObject* pyalpm_package_get_isize(AlpmPackage *self, void *closure) {
  CHECK_IF_INITIALIZED();
  return PyLong_FromLongLong(alpm_pkg_get_isize(self->c_data));
}

static PyObject* pyalpm_package_get_reason(AlpmPackage *self, void *closure) {
  CHECK_IF_INITIALIZED();
  return PyLong_FromLong(alpm_pkg_get_reason(self->c_data));
}

static PyObject* pyalpm_package_get_files(AlpmPackage *self, void *closure) {
  const alpm_filelist_t *flist = NULL;
  PyObject *result = NULL;

  CHECK_IF_INITIALIZED();

  flist = alpm_pkg_get_files(self->c_data);
  if (!flist)
    Py_RETURN_NONE;
  else {
    ssize_t i;
    result = PyList_New((Py_ssize_t)flist->count);
    for (i = 0; i < (ssize_t)flist->count; i++) {
      const alpm_file_t *file = flist->files + i;
      PyObject *filename = PyUnicode_DecodeFSDefault(file->name);
      PyObject *filesize = PyLong_FromLongLong(file->size);
      PyObject *filemode = PyLong_FromUnsignedLong(file->mode);
      PyObject *item = PyTuple_New(3);
      if (item && filename && filesize && filemode) {
        PyTuple_SET_ITEM(item, 0, filename);
        PyTuple_SET_ITEM(item, 1, filesize);
        PyTuple_SET_ITEM(item, 2, filemode);
        PyList_SET_ITEM(result, i, item);
      } else {
        Py_CLEAR(item);
        Py_CLEAR(filename);
        Py_CLEAR(filesize);
        Py_CLEAR(filemode);
        return NULL;
      }
    }
  }
  return result;
}

/** Convert alpm_backup_t to Python tuples
 * The resulting tuple is (filename, hexadecimal hash)
 */
static PyObject* pyobject_from_alpm_backup(void* data) {
  alpm_backup_t* item = (alpm_backup_t*)data;
  PyObject* tuple = Py_BuildValue("(ss)", item->name, item->hash);
  return tuple;
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
  return PyLong_FromLongLong(alpm_pkg_download_size(self->c_data));
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

struct list_getter get_licenses = { alpm_pkg_get_licenses, pyobject_from_string };
struct list_getter get_groups   = { alpm_pkg_get_groups, pyobject_from_string };
struct list_getter get_backup   = { alpm_pkg_get_backup, pyobject_from_alpm_backup };
struct list_getter get_deltas   = { alpm_pkg_get_deltas, pyobject_from_string };
struct list_getter get_depends  = { alpm_pkg_get_depends, _pyobject_from_pmdepend };
struct list_getter get_optdepends = { alpm_pkg_get_optdepends, _pyobject_from_pmdepend };
struct list_getter get_replaces   = { alpm_pkg_get_replaces, _pyobject_from_pmdepend };
struct list_getter get_provides   = { alpm_pkg_get_provides, _pyobject_from_pmdepend };
struct list_getter get_conflicts  = { alpm_pkg_get_conflicts, _pyobject_from_pmdepend };

static struct PyGetSetDef AlpmPackageGetSet[] = {
  { "db", (getter)pyalpm_package_get_db, 0, "the database from which the package comes from, or None", NULL } ,
  /* description properties */
  { "name",    (getter)_get_string_attribute, 0, "package name",    alpm_pkg_get_name } ,
  { "version", (getter)_get_string_attribute, 0, "package version", alpm_pkg_get_version } ,
  { "desc",    (getter)_get_string_attribute, 0, "package desc",    alpm_pkg_get_desc } ,
  { "url",     (getter)_get_string_attribute, 0, "package URL",     alpm_pkg_get_url } ,
  { "arch",    (getter)_get_string_attribute, 0, "target architecture", alpm_pkg_get_arch } ,
  { "licenses", (getter)_get_list_attribute, 0, "list of licenses", &get_licenses } ,
  { "groups",   (getter)_get_list_attribute, 0, "list of package groups", &get_groups } ,
  /* package properties */
  { "packager",   (getter)_get_string_attribute, 0, "packager name", alpm_pkg_get_packager } ,
  { "md5sum",     (getter)_get_string_attribute, 0, "package md5sum", alpm_pkg_get_md5sum } ,
  { "sha256sum",  (getter)_get_string_attribute, 0, "package sha256sum as hexadecimal digits", alpm_pkg_get_sha256sum } ,
  { "base64_sig", (getter)_get_string_attribute, 0, "GPG signature encoded as base64", alpm_pkg_get_base64_sig } ,
  { "filename",   (getter)_get_string_attribute, 0, "package filename", alpm_pkg_get_filename } ,
  { "size", (getter)pyalpm_package_get_size, 0, "package size", NULL } ,
  { "isize", (getter)pyalpm_package_get_isize, 0, "installed size", NULL } ,
  { "reason", (getter)pyalpm_package_get_reason, 0, "install reason (0 = explicit, 1 = depend)", NULL } ,
  { "builddate", (getter)pyalpm_package_get_builddate, 0, "building time", NULL } ,
  { "installdate", (getter)pyalpm_package_get_installdate, 0, "install time", NULL } ,
  { "files",  (getter)pyalpm_package_get_files, 0, "list of installed files", NULL } ,
  { "backup", (getter)_get_list_attribute, 0, "list of tuples (filename, md5sum)", &get_backup } ,
  { "deltas", (getter)_get_list_attribute, 0, "list of available deltas", &get_deltas } ,
  /* dependency information */
  { "depends",    (getter)_get_list_attribute, 0, "list of dependencies", &get_depends } ,
  { "optdepends", (getter)_get_list_attribute, 0, "list of optional dependencies", &get_optdepends } ,
  { "conflicts",  (getter)_get_list_attribute, 0, "list of conflicts", &get_conflicts } ,
  { "provides",   (getter)_get_list_attribute, 0, "list of provided package names", &get_provides } ,
  { "replaces",   (getter)_get_list_attribute, 0, "list of replaced packages", &get_replaces } ,
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
