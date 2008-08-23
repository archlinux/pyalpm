#ifndef PYALPM_H
#define PYALPM_H

/*pyalpm internal variables*/
char *error, *warning, *debug, *function;
unsigned short enable_messages_logcb = 0;
char VERSION[] = "0.1";
unsigned short init = 0;

/*we reuse pyalpm_list_t for freeing memory at alpm release*/
alpm_list_t *addresses;

/*copied from pacman db.h as it can't be included, set as pmdb_t in alpm.h*/
struct __pmdb_t {
	char *path;
	char *treename;
	void *handle;
	alpm_list_t *pkgcache;
	alpm_list_t *grpcache;
	alpm_list_t *servers;
};

/*pyalpm errors*/
PyObject *alpm_error = NULL;

/*miscelaneus functions*/
PyObject * alpmversion_alpm(PyObject *self);
PyObject * version_alpm(PyObject *self);

#endif /* PYALPM_H */