import unittest
import pyalpm

class TestOptions(unittest.TestCase):
    def setUp(self):
        pyalpm.initialize()

    def tearDown(self):
        pyalpm.release()

    def test_paths(self):
        "Test getter/setters for file path options"
        opts = pyalpm.options
        opts.root = "/root"
        self.assertEqual(opts.root, "/root/")

        opts.dbpath = "/var/lib/pacman"
        self.assertEqual(opts.dbpath, "/var/lib/pacman/")
        self.assertEqual(opts.lockfile, "/var/lib/pacman/db.lck")

        opts.logfile = "/var/log/pacman.log"
        self.assertEqual(opts.logfile, "/var/log/pacman.log")

    def test_paths_wrong_type(self):
        opts = pyalpm.options
        with self.assertRaises(pyalpm.error):
            opts.root = 3
        with self.assertRaises(pyalpm.error):
            opts.dbpath = 3
        with self.assertRaises(pyalpm.error):
            opts.logfile = 3

    def test_string(self):
        "Test getter/setters for string options"
        pyalpm.options.arch = 'i686'
        self.assertEqual(pyalpm.options.arch, 'i686')
        with self.assertRaises(pyalpm.error):
            pyalpm.options.arch = ["i686"]

    def test_booleans(self):
        "Test get/set for boolean options"
        opts = pyalpm.options
        opts.usesyslog = 1
        self.assertEqual(opts.usesyslog, 1)
        opts.usesyslog = 0
        self.assertEqual(opts.usesyslog, 0)
        opts.usedelta = 1
        self.assertEqual(opts.usedelta, 1)
        opts.checkspace = 1
        self.assertEqual(opts.checkspace, 1)

    def test_lists(self):
        "Test get/set for list options"
        opts = pyalpm.options

        opts.noupgrades = ["glibc", "pacman"]
        self.assertEqual(set(opts.noupgrades), set(["glibc", "pacman"]))
        opts.add_noupgrade("kernel26")
        self.assertEqual(set(opts.noupgrades), set(["glibc", "pacman", "kernel26"]))
        opts.remove_noupgrade("glibc")
        self.assertEqual(set(opts.noupgrades), set(["pacman", "kernel26"]))

        opts.ignorepkgs = ["glibc", "pacman"]
        self.assertEqual(set(opts.ignorepkgs), set(["glibc", "pacman"]))

if __name__ == "__main__":
    unittest.main(verbosity = 2)

# vim: set ts=4 sw=4 tw=0 et:
