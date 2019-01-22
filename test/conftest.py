import pytest

from pyalpm import Handle


@pytest.fixture()
def handle():
    return Handle('/', '/tmp')


@pytest.fixture()
def localdb(handle):
    return handle.get_localdb()
