Packages
========

.. py:class:: Package

   .. py:attribute:: name (str)

      The name of the package

   .. py:attribute:: builddate (Long long)

      The date on which this package was built

   .. py:attribute:: installdate (Long Long)

      The date in which this package was installed (only in localdb)

   .. py:attribute:: size (Long Long)
      
      A list of references to the packages in this database

   .. py:attribute:: isize (Long Long)

      The installed size

   .. py:attribute:: files (list)

      A list of files in this package

   .. py:attribute:: db (Database)

      A reference to the database this package belongs to

   .. py:attribute:: has_scriptlet (boolean)
      
      Whether this package has a scriptlet

   .. py:attribute:: licenses (list)

      A list of licenses for this package.

   .. py:attribute:: desc (string)

      Package description.

   .. py:attribute:: depends (list)

      A list of dependencies for this package

   .. py:attribute:: optdepends (list)

      A list of the optional dependencies for this package

   .. py:attribute:: checkdepends (list)

      A list of the check dependencies for this package (syncdb only)

   .. py:attribute:: makedepends (list)

      A list of the make dependencies for this package (syncdb only)

   .. py:attribute:: replaces (list)

      A list of packages this package replaces

   .. py:attribute:: provides (list)

      A list of strings of what this package provides

   .. py:attribute:: conflicts (list)

      A list of packages this package conflicts with

   .. py:attribute:: backup (list)

      A list of backup tuples (filename, md5sum)

   .. py:attribute:: groups (list)

      The groups this package belongs to

   .. py:attribute:: arch (string)

      The CPU architecture for this package

   .. py:attribute:: packager (string)

      The packager for this package

   .. py:attribute:: md5sum (string)

      The package md5sum as hexadecimal digits

   .. py:attribute:: sha256sum (string)

      The package sha256sum as hexadecimal digits

   .. py:attribute:: base64_sig (string)

      The package signature encoded as base64

   .. py:attribute:: filename (string)

      The package filename

   .. py:attribute:: url (string)

      The package URL

   .. py:method:: compute_requiredby()

      Computes a list of the packages this package is required by

     :returns: the packages who require this package

   .. py:method:: compute_optionalfor()

      Computes a list of the packages optionally requiring this package

     :returns: the packages who optionally require this package
