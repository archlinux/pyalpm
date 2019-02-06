from shutil import rmtree
from tempfile import mkdtemp

import pytest

from pyalpm import Handle


REPO_1 = 'core'
TEST_MIRROR = 'http://mirror.rackspace.com/archlinux/{}/os/x86_64'


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
    repo.servers = [TEST_MIRROR.format(REPO_1)]
    yield handle
    rmtree(dbpath)


@pytest.fixture(scope="module")
def syncdb(real_handle, name=REPO_1):
    for repo in real_handle.get_syncdbs():
        if repo.name == name:
            return repo


@pytest.fixture(scope="module")
def package(syncdb, name='pacman'):
    syncdb.update(False)
    return syncdb.get_pkg(name)
