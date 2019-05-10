Handles
=======

Handles are objects that provide access to pacman databases and transactions.

.. py:class:: Handle(rootpath: string, dbpath: string)

   A handle object is initialized with a root path (i.e.., where do packages
   get installed) and a dbpath (i.e., where is the database information
   located). Generally, these parameters default to root path being '/' and a
   dbpath being '/var/lib/pacman'.


   .. py:method:: get_localdb()

      Return a reference to the local database object

     :returns: an alpm database object for the localdb


   .. py:method:: get_syncdbs()

      Return a list of references to the sync databases currently registered.

     :returns: an alpm database object for the localdb. 


   .. py:method:: register_syncdb(name: string, flags: int)

     Registers the database with the given name.

     :param str name: The name of the database to register (e.g., 'core')
     :param int flags: an integer constant representing the type of access 
      (i.e., an ALPM_SIG_* constant as exported in the parent module)
     :returns: an alpm database object for this syncdb

   .. py:method:: set_pkgreason(package: Package, reason: int)

      Sets the reason for this package installation's (e.g., explicitly or as a
      dependency)

      :param Package package: the package.
      :param int reason: 0 for explicitly requrested by a user or 1 for as dependency of another package
      :returns: Nothing

   .. py:method:: add_cachedir()

      Adds a cachedir.

      :param str path: the path to the cachedir to add.
      :returns: Nothing

   .. py:method:: add_ignoregrp()

      Add an ignoregrp.

      :param str groupname: the groupname to ignore
      :returns: nothing

   .. py:method:: add_ignorepkg()

      Add an ignorepkg.

      :param str pkgname: the package name to ignore
      :returns: nothing

   .. py:method:: add_noextract()

      Add a noextract package.

      :param str pkgname: the package name to noextract
      :returns: nothing

   .. py:method:: add_noupgrade()

      Add a noupgrade package

      :param str pkgname: the package name to noextract
      :returns: nothing

   .. py:method:: init_transaction()

      Initializes a transaction

      :param bool nodeps: skip dependency checks
      :param bool force: overwrite existing packages (deprecated)
      :param bool nosave: do not save .pacsave files
      :param bool nodepversion: undocumented
      :param bool cascade: remove all dependent packages
      :param bool recurse: remove also explicitly installed unneeded dependent packages
      :param bool dbonly: only remove database entry, do not remove files
      :param bool alldeps: mark packages as non-explicitly installed
      :param bool downloadonly: download pakcages but do not install/upgrade anything
      :param bool noscriptlet: do not execute the install scriptlet of one exists
      :param bool noconflicts: ignore conflicts
      :param bool needed: do not reinstall the targets that are already up-to-date.
      :param bool allexplicit: undocmented
      :param bool unneeded: remove also explicitly unneeded deps
      :param bool recurseall: undocumented
      :param bool nolock: do not database
      :returns: a :class:`Transaction` object
