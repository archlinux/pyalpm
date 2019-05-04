Databases
=========

A database object represents a collection of packages. A database can be
accessed by means of a Handle object, and can be used to query for package
information.

.. py:class:: Database


   .. py:attribute:: name (string)
        
      The name of this database (for example, "core")

   .. py:attribute:: servers (list)

      A list of servers from where this database was fetched 

   .. py:attribute:: pkgcache (list)
      
      A list of references to the packages in this database

   .. py:attribute:: grpcache (list)

      A list of tuples of the form: 
      ::

         [
            ( 'GROUPNAME', [package-list] ),
            ( 'GROUPNAME2', [...]),
            ...
         ]



   .. py:method:: get_pkg(name: string)

      Retrieves a package instance with the name 'name'

     :param str name: The name of the package (e.g., 'coreutils')
     :returns: a reference to the Package object with the name 'name' or None
      if it doesn't exist

   .. py:method:: update(force: boolean)

      Attempts to update the sync database (i.e., alpm_db_update).

      :param bool force: If the database should be updated even if it's up-to-date
      :returns: True if the update was successful, or an error if that's not
       the case.

   .. py:method:: search(query: string) 

      Search this database for a package with the name matching the query.

      :param str query: a regexp representing the package searched for.
      :returns: a list of package objects matching the query by name


   .. py:method:: read_grp(group: string) 

      Retrieves the list of packages that belong to the group by the name passed

      :param str name: The name of the group (e.g., 'base')
      :returns: a list of  :class:`Package` objects that correspond to the packages that
       belong to that group or None if the group is not found.
