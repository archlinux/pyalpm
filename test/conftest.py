from os import mkdir
from shutil import copyfile

import pytest

from pyalpm import Handle

from generate_pacman_db import generate_syncdb, generate_localdb

PKG = 'linux'
REPO_1 = 'core'


@pytest.fixture()
def handle():
    return Handle('/', '/tmp')


@pytest.fixture()
def localdb(real_handle):
    return real_handle.get_localdb()


@pytest.fixture(scope="module")
def real_handle(tmpdir_factory):
    dbpath = str(tmpdir_factory.mktemp('dbpath'))
    syncdb = f"{dbpath}/sync"
    localdb = f"{dbpath}/local"
    mirrorpath = f'{dbpath}/{REPO_1}'
    syncdbfile = f'{syncdb}/{REPO_1}.db'

    mkdir(syncdb)
    mkdir(localdb)
    mkdir(mirrorpath)

    generate_syncdb(syncdbfile)

    handle = Handle('/', dbpath)

    # Generate localdb content after the db is initialised
    generate_localdb(localdb)

    # Generate local repo by copying the sync core.db
    # TODO: use a different sync db with a more packages
    copyfile(syncdbfile, f'{mirrorpath}/{REPO_1}.db')

    repo = handle.register_syncdb(REPO_1, 0)
    repo.servers = [f'file:///{mirrorpath}']

    yield handle


@pytest.fixture(scope="module")
def syncdb(real_handle, name=REPO_1):
    for repo in real_handle.get_syncdbs():
        if repo.name == name:
            return repo


@pytest.fixture(scope="module")
def package(syncdb, name=PKG):
    return syncdb.get_pkg(name)


@pytest.fixture()
def localpackage(localdb, name=PKG):
    return localdb.get_pkg(name)


@pytest.fixture()
def transaction(real_handle):
    transaction = real_handle.init_transaction()
    yield transaction
    transaction.release()


def assert_string_argument(func):
    with pytest.raises(TypeError) as excinfo:
        func(1)
    assert 'expecting a string argument' in str(excinfo.value)
