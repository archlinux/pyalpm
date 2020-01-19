from gc import collect

from pyalpm import Handle
from conftest import REPO_1


ARCH = 'x86_64'
TEST_MIRROR = 'https://mirror.pkgbuild.com/{repo}/os/{arch}'


def get_localdb():
    handle = Handle('/', '/tmp/')
    return handle.get_localdb()


def register_syncdb():
    handle = Handle('/', '/tmp/')
    repo = handle.register_syncdb(REPO_1, 0)
    repo.servers = [TEST_MIRROR.format(repo=REPO_1, arch=ARCH)]
    return repo


def get_syncdb():
    # Return syncdb, so handle should go out of scope
    handle = Handle('/', '/tmp/')
    repo = handle.register_syncdb(REPO_1, 0)
    repo.servers = [TEST_MIRROR.format(repo=REPO_1, arch=ARCH)]
    return handle.get_syncdbs()[0]


def get_pkg_search():
    db = get_syncdb()
    db.update(False)
    return db.search('pacman')[0]


def get_pkg_db():
    db = get_syncdb()
    db.update(False)
    return db.get_pkg('pacman')


def get_grpcache_pkg():
    syncdb = get_syncdb()
    syncdb.update(False)
    # Returns a tuple
    return syncdb.grpcache[0][1][0]


def get_read_grp():
    syncdb = get_syncdb()
    syncdb.update(False)
    # return pkg
    return syncdb.read_grp('base-devel')[1][0]


def get_db_pkgcache():
    syncdb = get_syncdb()
    syncdb.update(False)
    return syncdb.pkgcache[0]


# Database tests, where the Handle goes out of scope


def test_localdb_segfault():
    localdb = get_localdb()
    collect()
    x = localdb.search('yay')
    assert x == []


def test_syncdb_segfault():
    syncdb = get_syncdb()
    collect()
    assert syncdb.search('yay') == []


def test_register_syncdb_segfault():
    syncdb = register_syncdb()
    collect()
    assert syncdb.search('yay') == []

# Package tests, where the DB goes out of scope


def test_pkg_search_segfault():
    pkg = get_pkg_search()
    collect()
    assert pkg.name


def test_db_get_pkg_segfault():
    pkg = get_pkg_db()
    collect()
    assert pkg.name


def test_db_grpcache_pkg_segfault():
    pkg = get_grpcache_pkg()
    collect()
    assert pkg.name


def test_db_read_grp():
    pkg = get_read_grp()
    collect()
    assert pkg.name


def test_db_pkgcache():
    pkg = get_db_pkgcache()
    collect()
    assert pkg.name
