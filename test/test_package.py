from conftest import PKG

from pyalpm import Package


def test_db(package):
    assert package.db is not None

def test_name(package):
    assert package.name == PKG

def test_version(package):
    assert package.version != ''

def test_arch(package):
    assert package.arch == 'x86_64'

def test_size(package):
    assert isinstance(package.size, int)

def test_isize(package):
    assert isinstance(package.isize, int)

def test_reason(package):
    assert package.reason == 0

def test_builddate(package):
    assert isinstance(package.builddate, int)

def test_installdate(package):
    assert package.installdate == 0

def test_files(localpackage):
    assert localpackage.files != []

def test_backup(localpackage):
    assert localpackage.backup != []

def test_depends(package):
    assert package.depends != []

def test_has_scriptlet(package):
    assert isinstance(package.has_scriptlet, bool)

def test_download_size(package):
    assert package.download_size == 0

def test_compute_requiredby(package):
    assert package.compute_requiredby() == ['base']

def test_compute_optionalfor(package):
    assert package.compute_optionalfor() == []

def test_repr(package):
    assert repr(Package) == "<class 'alpm.Package'>"
    assert PKG in repr(package)

def test_str(package):
    assert PKG in str(package)
