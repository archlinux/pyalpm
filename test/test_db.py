import pytest

from conftest import handle, localdb, syncdb


def test_empty_getsyncdb(handle):
    assert handle.get_syncdbs() == []

def test_localdb_empty(localdb):
    assert localdb.get_pkg('foo') is None

def test_search_empty(localdb):
    assert localdb.search('bar') == []

def test_read_grp(localdb):
    assert localdb.read_grp('foo') is None

def test_read_grp_error(localdb):
    with pytest.raises(TypeError) as excinfo:
        localdb.read_grp()
    assert 'expected string argument' in str(excinfo.value)

def test_get_pkg(localdb):
    with pytest.raises(TypeError) as excinfo:
        localdb.get_pkg()
    assert 'takes a string argument' in str(excinfo.value)

def test_update(syncdb):
    syncdb.update(False)
    assert not syncdb.search('pacman') is None

# DB properties

def test_db_name(localdb):
    assert localdb.name == 'local'

def test_db_servers(localdb):
    assert localdb.servers == []

def test_db_pkgcache(localdb):
    assert localdb.pkgcache == []

def test_db_grpcache_empty(localdb):
    assert localdb.grpcache == []

# vim: set ts=4 sw=4 et:
