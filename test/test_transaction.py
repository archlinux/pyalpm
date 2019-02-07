from unittest import mock
from pytest import raises

from conftest import real_handle as handle, transaction, package, PKG

from pyalpm import error


def test_cb_download(handle):
    cb_dl = mock.Mock()
    handle.dlcb = cb_dl
    db = handle.get_syncdbs()[0]
    db.update(True)
    cb_dl.assert_called()

def test_add_pkg_not_satisfied(handle, transaction, package):
    transaction.add_pkg(package)
    with raises(error) as excinfo:
        transaction.prepare()
    assert 'could not satisfy dependencies' in str(excinfo)

def test_add_pkg_error(transaction):
    with raises(TypeError) as excinfo:
        transaction.add_pkg(PKG)
    assert 'must be alpm.Package' in str(excinfo.value)

def test_remove_pkg_error(transaction):
    with raises(TypeError) as excinfo:
        transaction.remove_pkg(PKG)
    assert 'must be alpm.Package' in str(excinfo.value)
