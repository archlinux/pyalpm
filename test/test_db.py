from conftest import handle, localdb


def test_empty_getsyncdb(handle):
    assert handle.get_syncdbs() == []

def test_localdb_empty(localdb):
    assert localdb.get_pkg('foo') is None

def test_search_empty(localdb):
    assert localdb.search('bar') == []

def test_read_grp_empty(localdb):
    assert localdb.read_grp('foo') is None

# DB properties

def test_db_name(localdb):
    assert localdb.name == 'local'

def test_db_servers(localdb):
    assert localdb.servers == []

def test_db_pkgcache(localdb):
    assert localdb.pkgcache == []

def test_db_grpcache(localdb):
    assert localdb.grpcache == []

# vim: set ts=4 sw=4 et:
