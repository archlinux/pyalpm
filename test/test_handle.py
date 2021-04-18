from pytest import raises

import pyalpm

from conftest import assert_string_argument, PKG


def test_cachedirs(handle):
    handle.add_cachedir('/tmp/')
    assert '/tmp/' in handle.cachedirs

    handle.remove_cachedir('/tmp/')
    assert '/tmp/' not in handle.cachedirs

def test_cachedirs_error(handle):
    assert_string_argument(handle.add_cachedir)
    assert_string_argument(handle.remove_cachedir)

def test_set_cachedirs(handle):
    handle.cachedirs = ['/tmp/']
    assert handle.cachedirs == ['/tmp/']

def test_ignoregrps(handle):
    handle.add_ignoregrp('base')
    assert 'base' in handle.ignoregrps

    handle.remove_ignoregrp('base')
    assert 'base' not in handle.ignoregrps

def test_ignoregrps_error(handle):
    assert_string_argument(handle.add_ignoregrp)
    assert_string_argument(handle.remove_ignoregrp)

def test_set_ignoregrps(handle):
    handle.ignoregrps = ['base']
    assert handle.ignoregrps == ['base']

def test_ignorepkg(handle):
    handle.add_ignorepkg(PKG)
    assert PKG in handle.ignorepkgs

    handle.remove_ignorepkg(PKG)
    assert PKG not in handle.ignorepkgs

def test_ignorepkg_error(handle):
    assert_string_argument(handle.add_ignorepkg)
    assert_string_argument(handle.remove_ignorepkg)

def test_set_ignorepkgs(handle):
    handle.ignorepkgs = [PKG]
    assert handle.ignorepkgs == [PKG]

def test_noextracts(handle):
    handle.add_noextract('index.php')
    assert 'index.php' in handle.noextracts

    handle.remove_noextract('index.php')
    assert 'index.php' not in handle.noextracts

def test_noextracts_error(handle):
    assert_string_argument(handle.add_noextract)
    assert_string_argument(handle.remove_noextract)

def test_set_noextracts(handle):
    handle.noextracts = ['index.php']
    assert handle.noextracts == ['index.php']

def test_noupgrade(handle):
    handle.add_noupgrade('linux')
    assert 'linux' in handle.noupgrades

    handle.remove_noupgrade('linux')
    assert 'linux' not in handle.noupgrades

def test_noupgrade_error(handle):
    assert_string_argument(handle.add_noupgrade)
    assert_string_argument(handle.remove_noupgrade)

def test_usesyslog(handle):
    handle.usesyslog = True
    assert handle.usesyslog

def test_usesyslog_error(handle):
    with raises(TypeError) as excinfo:
        handle.usesyslog = "nope"
    assert 'wrong arguments' in str(excinfo.value)

def test_checkspace(handle):
    assert not handle.checkspace
    handle.checkspace = True
    assert handle.checkspace

def test_checkspace_error(handle):
    with raises(TypeError) as excinfo:
        handle.checkspace = "nope"
    assert 'wrong arguments' in str(excinfo.value)

def test_noupgrades(handle):
    assert not handle.noupgrades

def test_set_noupgrades(handle):
    handle.noupgrades = [PKG]
    assert handle.noupgrades == [PKG]

def test_root(handle):
    assert handle.root == '/'

def test_gpgdir(handle):
    handle.gpgdir = '/'
    assert handle.gpgdir == '/'

    handle.gpgdir = b'/'
    assert handle.gpgdir == '/'

def test_invalid_logfile(handle):
    with raises(TypeError) as excinfo:
        handle.logfile = 1
    assert 'logfile path must be a string' in str(excinfo.value)

def test_invalid_logcb(handle):
    with raises(TypeError) as excinfo:
        handle.logcb = 1
    assert 'value must be None or a function' in str(excinfo.value)

def test_logcb(handle):
    handle.logcb = None
    assert handle.logcb is None

def test_load_pkg_invalid(handle):
    with raises(pyalpm.error) as excinfo:
        handle.load_pkg('/tmp/noexistant.txt')
    assert 'loading package failed' in str(excinfo.value)


def test_load_pkg(handle, localpkg):
    pkg = handle.load_pkg(localpkg)
    assert pkg.name == 'empty'

def test_set_pkgreason(handle, package):
    with raises(pyalpm.error) as excinfo:
        handle.set_pkgreason(package, -1)
    assert 'failed setting install reason' in str(excinfo.value)

def test_register_syncdb_invalid(handle):
    with raises(TypeError) as excinfo:
        handle.register_syncdb([], -1)
    assert 'takes a string and an integer' in str(excinfo.value)

    with raises(TypeError) as excinfo:
        handle.register_syncdb("foo", "bar")
    assert 'takes a string and an integer' in str(excinfo.value)

    with raises(pyalpm.error) as excinfo:
        handle.register_syncdb("http://", 99999)
    assert 'unable to register sync database' in str(excinfo.value)

def test_create_failed():
    with raises(pyalpm.error) as excinfo:
        pyalpm.Handle('/non-existant', '/')
    assert 'could not create a libalpm handle' in str(excinfo.value)


# vim: set ts=4 sw=4 et:
