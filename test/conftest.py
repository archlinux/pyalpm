import os.path
from os import mkdir
from os.path import basename
from shutil import copyfile

import json

import pytest

from pyalpm import Handle


PKG = 'linux'
REPO_1 = 'core'


@pytest.fixture(scope="module")
def db_data():
    curpath = os.path.dirname(os.path.realpath(__file__))
    return json.load(open(f'{curpath}/db.json'))

@pytest.fixture()
def handle():
    return Handle('/', '/tmp')


@pytest.fixture()
def localdb(real_handle):
    return real_handle.get_localdb()


@pytest.fixture(scope="module")
def real_handle(tmpdir_factory, generate_syncdb, generate_localdb, db_data):
    dbpath = str(tmpdir_factory.mktemp('dbpath'))
    syncdb = f"{dbpath}/sync"
    localdb = f"{dbpath}/local"
    # TODO: use a different location for clarity
    mirrorpath = f'{dbpath}/{REPO_1}'
    syncdbfile = f'{syncdb}/{REPO_1}.db'

    mkdir(syncdb)
    mkdir(mirrorpath)

    generate_syncdb(db_data, basename(syncdbfile), syncdb)
    generate_localdb(db_data, dbpath)

    # Generate localdb content after the db is initialised
    handle = Handle('/', dbpath)

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


@pytest.fixture(scope="module")
def localpkg(generate_package):
    pkg_data = {
        "pkgname": "empty",
        "pkgbase": "empty",
        "pkgver": "1-1",
        "pkgdesc": "empty",
        "url": "https://archlinux.org",
        "builddate": 1599085821,
        "packager": "Test <test@archlinux.org>",
        "arch": "x86_64",
        "license": "GPL",
        "depend": [
        ]
    }
    return generate_package(pkg_data)
