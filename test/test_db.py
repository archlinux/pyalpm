import pytest

from pyalpm import error


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
    assert syncdb.search('pacman') is not None

def test_update_error(handle, syncdb):
    servers = syncdb.servers
    syncdb.servers = ['nonexistant']
    with pytest.raises(error) as excinfo:
        syncdb.update(False)
    assert 'unable to update database' in str(excinfo.value)
    # TODO(jelle): remove when handle is no longer scope="module"
    syncdb.servers = servers

# DB properties

def test_db_name(localdb):
    assert localdb.name == 'local'

def test_db_servers(localdb):
    assert localdb.servers == []

    with pytest.raises(TypeError) as excinfo:
        localdb.servers = [1]
    assert 'list must contain only strings' in str(excinfo.value)

    localdb.servers = [b'server']
    assert localdb.servers == ['server']

def test_db_pkgcache(localdb):
    assert localdb.pkgcache != []

def test_db_grpcache_empty(localdb):
    assert localdb.grpcache != []

def test_db_grpcache_not_empty(syncdb):
    assert syncdb.grpcache != []

def test_db_repr(localdb):
    assert 'local' in repr(localdb)

def test_db_str(localdb):
    assert 'local' in str(localdb)

# vim: set ts=4 sw=4 et:
