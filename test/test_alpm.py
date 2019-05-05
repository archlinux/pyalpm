import pytest
import pyalpm

from conftest import package, syncdb, PKG


def test_alpm_version():
    assert pyalpm.alpmversion() != ''

def test_version():
    assert pyalpm.version() != ''

def test_vercmp_smaller():
    assert pyalpm.vercmp('1', '2') == -1

def test_vercmp_greater():
    assert pyalpm.vercmp('2', '1') == 1
    assert pyalpm.vercmp('2.0-1', '1.7-6') == 1

def test_vercmp_equal():
    assert pyalpm.vercmp('1', '1') == 0
    assert pyalpm.vercmp('1.0', '1.0-10') == 0

def test_vercmp_epoch():
    assert pyalpm.vercmp('4.34', '1:001') == -1

def test_find_satisfier(package):
    assert pyalpm.find_satisfier([package], PKG).name == package.name
    assert pyalpm.find_satisfier([package], 'bar') is None

def test_find_satisfier_error():
    with pytest.raises(TypeError) as excinfo:
        pyalpm.find_satisfier()
    assert 'takes a Package list and a string' in str(excinfo.value)

    with pytest.raises(TypeError) as excinfo:
        pyalpm.find_satisfier(["foo"], PKG)
    assert 'list must contain only Package objects' in str(excinfo.value)

def test_find_grp_pkgs(syncdb):
    assert pyalpm.find_grp_pkgs([syncdb], 'test') == []

def test_find_grp_pkgs_error():
    with pytest.raises(TypeError) as excinfo:
        pyalpm.find_grp_pkgs()
    assert 'expected arguments' in str(excinfo.value)

    with pytest.raises(TypeError) as excinfo:
        pyalpm.find_grp_pkgs([None], 'test')
    assert 'list must contain only Database objects' in str(excinfo.value)

def test_sync_newversion(syncdb, package):
    assert pyalpm.sync_newversion(package, [syncdb]) is None

def test_sync_newversion_error():
    with pytest.raises(TypeError) as excinfo:
        pyalpm.sync_newversion()
    assert 'takes a Package and a list of DBs' in str(excinfo.value)

# vim: set ts=4 sw=4 et:
