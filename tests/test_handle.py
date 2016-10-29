import unittest
import pyalpm


class handle(unittest.TestCase):

    def setUp(self):
        self.handle = pyalpm.Handle('/', '/var/lib/pacman')

    def test_cachedirs(self):
        self.handle.add_cachedir('/tmp/')
        self.assertIn('/tmp/', self.handle.cachedirs)

        self.handle.remove_cachedir('/tmp/')
        self.assertNotIn('/tmp/', self.handle.cachedirs)

    def test_ignoregrps(self):
        self.handle.add_ignoregrp('base')
        self.assertIn('base', self.handle.ignoregrps)

        self.handle.remove_ignoregrp('base')
        self.assertNotIn('base', self.handle.ignoregrps)

    def test_ignorepkg(self):
        self.handle.add_ignorepkg('pacman')
        self.assertIn('pacman', self.handle.ignorepkgs)

        self.handle.remove_ignorepkg('pacman')
        self.assertNotIn('pacman', self.handle.ignorepkgs)

    def test_noextracts(self):
        self.handle.add_noextract('index.php')
        self.assertIn('index.php', self.handle.noextracts)

        self.handle.remove_noextract('index.php')
        self.assertNotIn('index.php', self.handle.noextracts)

    def test_noupgrade(self):
        self.handle.add_noupgrade('linux')
        self.assertIn('linux', self.handle.noupgrades)

        self.handle.remove_noupgrade('linux')
        self.assertNotIn('linux', self.handle.noupgrades)

# vim: set ts=4 sw=4 et:
