import os
from unittest import mock

import pytest
from pytest import raises

from conftest import real_handle as handle, PKG

from pyalpm import Handle, error


def test_cb_download(handle):
    cb_dl = mock.Mock()
    handle.dlcb = cb_dl
    db = handle.get_syncdbs()[0]
    db.update(True)
    cb_dl.assert_called()


def test_logcb(handle):
    cb_log = mock.Mock()
    handle.logcb = cb_log
    db = handle.get_syncdbs()[0]
    db.update(False)
    cb_log.assert_called()
    assert handle.logcb == cb_log


def test_add_pkg_not_satisfied(handle, transaction, package):
    transaction.add_pkg(package)
    with raises(error) as excinfo:
        transaction.prepare()
    assert 'could not satisfy dependencies' in str(excinfo.value)
    # (target, needed, causingpkg)
    assert excinfo.value.args[2] == [('linux', 'coreutils', None)]

def test_prepare_released(handle):
    transaction = handle.init_transaction()
    transaction.release()
    with raises(error) as excinfo:
        transaction.prepare()
    assert 'transaction preparation failed' in str(excinfo.value)
    assert excinfo.value.args[2] == []

def test_prepare_invalid_arch(real_handle, transaction, package):
    # HACK: the real_handle is module scoped so revert any changes made.
    old_arch = real_handle.arch
    real_handle.arch = ['aarch64']
    try:
        transaction.add_pkg(package)
        with raises(error) as excinfo:
            transaction.prepare()
        assert 'transaction preparation failed' in str(excinfo.value)
        assert excinfo.value.args[2] == [f'{package.name}-{package.version}-{package.arch}']
    finally:
        real_handle.arch = old_arch

@pytest.fixture()
def conflict_handle(tmp_path, generate_localdb, generate_syncdb):
    installed = {'name': 'foo', 'base': 'foo', 'version': '1-1', 'arch': 'any',
                 'files': []}
    target = {'name': 'bar', 'base': 'bar', 'version': '1-1', 'arch': 'any',
              'conflicts': ['foo']}
    dbpath = str(tmp_path)
    generate_localdb([installed], dbpath)
    os.mkdir(f'{dbpath}/sync')
    generate_syncdb([target], 'test.db', f'{dbpath}/sync')
    handle = Handle('/', dbpath)
    handle.register_syncdb('test', 0)
    return handle

def test_prepare_conflicting_deps(conflict_handle):
    syncdb = conflict_handle.get_syncdbs()[0]
    transaction = conflict_handle.init_transaction()
    try:
        transaction.add_pkg(syncdb.get_pkg('bar'))
        with raises(error) as excinfo:
            transaction.prepare()
        assert 'transaction preparation failed' in str(excinfo.value)
        # (package1, package2, reason)
        assert excinfo.value.args[2] == [('bar', 'foo', 'foo')]
    finally:
        transaction.release()

def test_add_pkg_error(transaction):
    with raises(TypeError) as excinfo:
        transaction.add_pkg(PKG)
    assert 'must be alpm.Package' in str(excinfo.value)

def test_remove_pkg_error(transaction):
    with raises(TypeError) as excinfo:
        transaction.remove_pkg(PKG)
    assert 'must be alpm.Package' in str(excinfo.value)

def test_flags(transaction):
    assert isinstance(transaction.flags, dict)

def test_to_add(transaction):
    assert transaction.to_add == []

def test_to_remove(transaction):
    assert transaction.to_remove == []

def test_sysupgrade(transaction):
    # No packages installed
    transaction.sysupgrade(False)
    assert transaction.to_add == []
    assert transaction.to_remove == []

def test_interrupt_error(transaction):
    with raises(error) as excinfo:
        transaction.interrupt()
    assert 'unable to interrupt transaction' in str(excinfo.value)

def test_commit_error(transaction):
    with raises(error) as excinfo:
        transaction.commit()
    assert 'transaction failed' in str(excinfo.value)
