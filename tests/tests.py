import os
import pyalpm

pyalpm.initialize()
pyalpm.options.root = "/"
pyalpm.options.dbpath = "/var/lib/pacman"

localdb = pyalpm.get_localdb()
print("Local database information")
print("  DB name:", localdb.name, "url:", localdb.url)
print("  Packages:", len(localdb.pkgcache))

for name, pkgs in localdb.grpcache:
    print("  Group:", name, [pkg.name for pkg in pkgs])

print("Registering [core], [extra], [community]...")
pyalpm.register_syncdb("core")
pyalpm.register_syncdb("extra")
pyalpm.register_syncdb("community")

print("Available sync DBs")
for db in pyalpm.get_syncdbs():
    print("  DB:", db.name, "url:", db.url)

print("Package information about glibc")
pkg = localdb.get_pkg("glibc")
for attr in dir(pkg):
    if not attr.startswith('_'):
        print("  ", attr, ":", getattr(pkg, attr))

