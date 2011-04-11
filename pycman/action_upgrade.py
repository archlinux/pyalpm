#!/usr/bin/env python3
# 
# pycman-upgrade - A Python implementation of Pacman
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
A Python implementation of pacman -U

This script installs packages from tarballs. Various options control
the effect of the transaction.
"""

import sys
import traceback
import pyalpm
from . import config

def cb_event(*args):
	print("event", args)
def cb_conv(*args):
	print("conversation", args)
def cb_progress(*args):
	print("progress", args)

def upgrade(pkgs, options):
	# prepare target list
	db = pyalpm.get_localdb()
	targets = []
	for name in pkgs:
		pkg = pyalpm.load_pkg(name)
		targets.append(pkg)

	t = pyalpm.transaction
	t.init(
			nodeps = options.nodeps,
			dbonly = options.dbonly,
			alldeps = (options.mode == pyalpm.PKG_REASON_DEPEND),
			allexplicit = (options.mode == pyalpm.PKG_REASON_EXPLICIT),
			event_callback = cb_event,
			conv_callback = cb_conv,
			progress_callback = cb_progress)

	for pkg in targets:
		t.add_pkg(pkg)

	try:
		t.prepare()
		t.commit()
	except pyalpm.error:
		traceback.print_exc()

	t.release()
	return 0

def main(rawargs):
	parser = config.make_parser()
	group = parser.add_argument_group("upgrade options")
	group.add_argument('-d', '--nodeps',
			action = 'store_true', default = False,
			help = 'skip dependency checks')
	group.add_argument('-k', '--dbonly',
			action = 'store_true', default = False,
			help = 'only modify database entries, not package files')
	group.add_argument('--asdeps', dest = 'mode',
			action = "store_const",
			const = pyalpm.PKG_REASON_DEPEND)
	group.add_argument('--asexplicit', dest = 'mode',
			action = "store_const",
			const = pyalpm.PKG_REASON_EXPLICIT)
	group.add_argument('pkgs', metavar = 'pkg', nargs='*',
			help = "a list of package URLs, e.g. package-1.0-1-i686.tar.xz")

	args = parser.parse_args(rawargs)
	config.init_with_config_and_options(args)

	if args.verbose:
		print("upgrade " + " ".join(rawargs), file = sys.stderr)

	return upgrade(args.pkgs, args)

if __name__ == "__main__":
	ret = main(sys.argv[1:])
	sys.exit(ret)

# vim: set ts=4 sw=4 tw=0 noet:
