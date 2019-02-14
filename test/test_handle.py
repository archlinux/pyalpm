from pytest import raises

import pyalpm

from conftest import assert_string_argument, handle, PKG


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
    assert PKG not in  handle.ignorepkgs

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
    assert 'wrong arguments' in str(excinfo)

def test_deltaratio(handle):
    assert handle.deltaratio == 0.0
    handle.deltaratio = 1
    assert handle.deltaratio == 1.0

def test_checkspace(handle):
    assert not handle.checkspace
    handle.checkspace = True
    assert handle.checkspace

def test_checkspace_error(handle):
    with raises(TypeError) as excinfo:
        handle.checkspace = "nope"
    assert 'wrong arguments' in str(excinfo)

def test_noupgrades(handle):
    assert not handle.noupgrades

def test_set_noupgrades(handle):
    handle.noupgrades = [PKG]
    assert handle.noupgrades == [PKG]

def test_root(handle):
    assert handle.root == '/'

def test_gpgdir(handle):
    handle.gpgdir = '/'

def test_load_pkg(handle):
    with raises(pyalpm.error) as excinfo:
        handle.load_pkg('/tmp/noexistant.txt')
    assert 'loading package failed' in str(excinfo)



# vim: set ts=4 sw=4 et:
