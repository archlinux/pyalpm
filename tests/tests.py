import os
import pyalpm

h = pyalpm.Handle("/", "/var/lib/pacman")

localdb = h.get_localdb()
print("Local database information")
print("  DB name:", localdb.name, "servers:", localdb.servers)
print("  Packages:", len(localdb.pkgcache))

for name, pkgs in localdb.grpcache:
    print("  Group:", name, [pkg.name for pkg in pkgs])
print("")

print("Registering [core], [extra], [community]...")
core = h.register_syncdb("core", pyalpm.SIG_DATABASE_OPTIONAL)
extra = h.register_syncdb("extra", pyalpm.SIG_DATABASE_OPTIONAL)
community = h.register_syncdb("community", pyalpm.SIG_DATABASE_OPTIONAL)
print("")

print("Available sync DBs")
for db in h.get_syncdbs():
    print("  DB:", db.name, "servers:", db.servers)
print("")

print("Package information about glibc")
pkg = localdb.get_pkg("glibc")
for attr in dir(pkg):
    if attr.startswith('_'):
        continue
    if attr == "files":
        print("  ", len(pkg.files), "files")
    else:
        print("  ", attr, ":", getattr(pkg, attr))
print("  Required by:", ' '.join(pkg.compute_requiredby()))
print("")

print("Package information about a tarball")
for i in os.listdir("/var/cache/pacman/pkg"):
    filename = os.path.join("/var/cache/pacman/pkg", i)
    pkg = h.load_pkg(filename)
    print("Loaded", filename)
    break
for attr in dir(pkg):
    if attr.startswith('_'):
        continue
    if attr == "files":
        print("  ", len(pkg.files), "files")
    else:
        print("  ", attr, ":", getattr(pkg, attr))
print("")

print("Information about group gnome")
l = pyalpm.find_grp_pkgs([core, extra, community], "gnome")
for pkg in l:
    print("  ", pkg.name, "from", pkg.db.name)
print("")

# vim: set ts=4 sw=4 et:
