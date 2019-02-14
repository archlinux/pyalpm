from shutil import rmtree
from tempfile import mkdtemp

import pytest

from pyalpm import Handle


ARCH = 'x86_64'
PKG = 'pacman'
REPO_1 = 'core'
TEST_MIRROR = 'http://mirror.rackspace.com/archlinux/{}/os/{}'


@pytest.fixture()
def handle():
    return Handle('/', '/tmp')


@pytest.fixture()
def localdb(handle):
    return handle.get_localdb()


@pytest.fixture(scope="module")
def real_handle():
    dbpath = mkdtemp(dir='/tmp')
    handle = Handle('/', dbpath)
    repo = handle.register_syncdb(REPO_1, 0)
    repo.servers = [TEST_MIRROR.format(REPO_1, ARCH)]
    yield handle
    rmtree(dbpath)


@pytest.fixture(scope="module")
def syncdb(real_handle, name=REPO_1):
    for repo in real_handle.get_syncdbs():
        if repo.name == name:
            return repo


@pytest.fixture(scope="module")
def package(syncdb, name=PKG):
    syncdb.update(False)
    return syncdb.get_pkg(name)


@pytest.fixture()
def transaction(real_handle):
    transaction = real_handle.init_transaction()
    yield transaction
    transaction.release()


def assert_string_argument(func):
    with pytest.raises(TypeError) as excinfo:
        func(1)
    assert 'expecting a string argument' in str(excinfo)
