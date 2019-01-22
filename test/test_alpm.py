import pyalpm


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

# vim: set ts=4 sw=4 et:
