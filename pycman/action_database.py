#!/usr/bin/env python3
# 
# pycman-deptest - A Python implementation of Pacman
# Copyright (C) 2011 Rémy Oudompheng <remy@archlinux.org>
# 
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program; if not, write to the Free Software
#   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#

"""
A Python implementation of pacman -D

This script allows to modify install reasons for packages in local
database.
"""

import sys
import pyalpm
from . import config

def commit(pkgs, mode):
	db = pyalpm.get_localdb()
	for pkg in pkgs:
		db.set_pkgreason(pkg, mode)

def main(rawargs):
	parser = config.make_parser()
	mode = parser.add_mutually_exclusive_group(required = True)
	mode.add_argument('--asdeps', dest = 'mode',
			action = "store_const",
			const = pyalpm.PKG_REASON_DEPEND)
	mode.add_argument('--asexplicit', dest = 'mode',
			action = "store_const",
			const = pyalpm.PKG_REASON_EXPLICIT)
	parser.add_argument('pkgs', metavar = 'pkg', nargs='*',
			help = "a dependency string, e.g. 'pacman>=3.4.0'")
	args = parser.parse_args(rawargs)
	config.init_with_config_and_options(args)

	if args.verbose:
		print("database " + " ".join(rawargs), file = sys.stderr)

	commit(args.pkgs, args.mode)
	return 0

if __name__ == "__main__":
	ret = main(sys.argv[1:])
	sys.exit(ret)

# vim: set ts=4 sw=4 tw=0 noet:
