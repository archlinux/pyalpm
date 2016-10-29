import unittest
import pyalpm


class vercmp(unittest.TestCase):

    def test_smaller(self):
        self.assertEqual(pyalpm.vercmp('1', '2'), -1)

    def test_greater(self):
        self.assertEqual(pyalpm.vercmp('2', '1'), 1)
        self.assertEqual(pyalpm.vercmp('2.0-1', '1.7-6'), 1)

    def test_equal(self):
        self.assertEqual(pyalpm.vercmp('1', '1'), 0)
        self.assertEqual(pyalpm.vercmp('1.0', '1.0-10'), 0)

    def test_epoch(self):
        self.assertEqual(pyalpm.vercmp('4.34', '1:001'), -1)

# vim: set ts=4 sw=4 et:
