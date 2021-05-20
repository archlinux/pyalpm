from unittest import mock
from pytest import raises

from conftest import real_handle as handle, PKG

from pyalpm import error


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
