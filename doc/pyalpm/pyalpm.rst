Pyalpm
=======

Pyalpm is a Python module that provides native bindings libalpm to interact
with Arch Linux package databases.

.. toctree::
   :maxdepth: 2
   :caption: Contents:

   Handle
   Database
   Package



Basic Usage
-----------

The easiest way to get access to a pacman database is by means of the handle
object. To obtain a handle object, you need to initialize it with a rootdir
parameter (i.e., the --root path parameter in pacman) and a database location
(i.e., a --dbpath parameter):

.. code-block:: python

   from pyalpm import Handle
   handle = Handle(".", "/var/lib/pacman")
   localdb = handle.get_localdb()


The above code sample will provide you with a localdb object, that you can use
to query, for example, package information:

.. code-block:: python

   coreutils = localdb.get_pkg("coreutils")

The call to get_pkg will provide you with the package passed as the name (in
this case, "coreutils"), you can query information about the package using this
reference.

.. code-block:: python

   print(coreutils.packager)
   # 'John Doe <johndoe@archlinux.org>'

   print(coreutils.licenses)
   # ['GPL3']

   print(coreutils.version)
   # '8.30-1'

You can see what other properties belong to the package object by looking at
the Package page.

A Handle object can also provide a sync database to you:

.. code-block:: python

   import pyalpm
   from pyalpm import Handle
   handle = Handle('.', '/var/lib/pacman/')
   core = handle.register_syncdb("core", pyalpm.SIG_DATABASE)

The parameters required in this case are a database name ("core") and a flag to
indicate the type of PGP level of verification that should be done on the
database. You can read more about the second flag in the SIG section of this
manual.

A syncdb is similar to a localdb (they are, in fact, both DB objects), but some
minor differences such as the installed size being 0 (as the packages are not
installed yet) and the download size of packages not being necessarilly 0 (as
the packages may have not been downloaded yet).

You can use the syncdb to, for example, search for packages:

.. code-block:: python

   core.search("linux.*")
   # a bunch of packages with linux in their name

   linux = core.get_pkg("linux")
   print(linux.download_size)
   # around 70 megabytes
