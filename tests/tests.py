import os
import pyalpm

h = pyalpm.Handle("/", "/var/lib/pacman")

localdb = h.get_localdb()
print("Local database information")
print("  DB name:", localdb.name, "url:", localdb.url)
print("  Packages:", len(localdb.pkgcache))

for name, pkgs in localdb.grpcache:
    print("  Group:", name, [pkg.name for pkg in pkgs])
print("")

print("Registering [core], [extra], [community]...")
core = pyalpm.register_syncdb("core")
extra = pyalpm.register_syncdb("extra")
community = pyalpm.register_syncdb("community")
print("")

print("Available sync DBs")
for db in pyalpm.get_syncdbs():
    print("  DB:", db.name, "url:", db.url)
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
