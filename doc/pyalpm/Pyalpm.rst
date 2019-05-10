pyalpm
======

The pyalpm module contains a a number of convenience functions for general usage.

.. py:module:: pyalpm


.. py:method:: version

      Returns pyaplm version

     :returns: the string contraining the pyalpm version


.. py:method:: alpmversion

      Returns alpm version

     :returns: the string contraining the aplm version


.. py:method:: vercmp(string: version1, string: version2)

      Returns compares version strings. See `man vercmp` for details

     :returns: 


.. py:method:: find_satisfier(list: packages, string: pkgname)

      Returns the satisfing package for a given string from a list of packages.

     :returns: returns a :class:`Package` or none.


.. py:method:: sync_newversion(package, list: databases)

      Finds an available upgrade for a package in a list ofdatabases.

     :returns: returns an upgrade :class:`Package` candidate or none.


.. py:method:: find_grp_pkgs(list: databases, string: group)

      Find packages from a given group across databases..

     :returns: returns a list of :class:`Package` objects.

.. py:data:: SIG_DATABASE

      Undocumented

.. py:data:: SIG_DATABASE_OPTIONAL

      Database singing optional, absence of signature is not an error

.. py:data:: SIG_DATABASE_MARGINAL_OK

      Signatures are required, marginal trust

.. py:data:: SIG_DATABASE_UNKNOWN_OK

      Signatures are required, unknown trust
