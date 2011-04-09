#!/usr/bin/env python3
# 
# pycman-sync - A Python implementation of Pacman
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
A Python implementation of pacman -S

This script displays information about packages available in repositories,
and is also used to install/upgrade/remove them.
"""

import sys

import pyalpm
from pycman import config
from pycman import pkginfo

def show_groups(args):
	"Show groups like pacman -Sg"
	for repo in pyalpm.get_syncdbs():
		if len(args.args) == 0:
			# list all available groups
			[print(name) for name, pkgs in repo.grpcache]
		else:
			# only print chosen groups
			for group in args.args:
				grp = repo.read_grp(group)
				if grp is None:
					continue
				else:
					name, pkgs = grp
				if args.quiet:
					[print(pkg.name) for pkg in pkgs]
				else:
					[print(name, pkg.name) for pkg in pkgs]
	return 0

def show_repo(args):
	"Show repository's list of packages like pacman -Sl"
	repos = pyalpm.get_syncdbs()
	if len(args.args) > 0:
		repo_dict = dict((repo.name, repo) for repo in repos)
		try:
			repos = [repo_dict[name] for name in args.args]
		except KeyError as err:
			print("error: repository '%s' was not found" % err.args)
			return 1

	for repo in repos:
		if args.quiet:
			[print(pkg.name) for pkg in repo.pkgcache]
		else:
			[print(repo.name, pkg.name, pkg.version) for pkg in repo.pkgcache]
	return 0

def show_packages(args):
	"Show information about packages like pacman -Si"
	retcode = 0
	repos = pyalpm.get_syncdbs()
	if len(args.args) == 0:
		for repo in repos:
			for pkg in repo.pkgcache:
				pkginfo.display_pkginfo(pkg, level = args.info, style = 'sync')
	else:
		for pkgname in args.args:
			if '/' in pkgname:
				repo, pkgname = pkgname.split('/', 1)
				pkg = repo.get_pkg(pkgname)
				if pkg is None:
					retcode = 1
					print("error: package '%s' was not found in repository '%s'" % (pkgname, repo.name))
				else:
					pkginfo.display_pkginfo(pkg, level = args.info, style = 'sync')
			else:
				found = False
				for repo in repos:
					pkg = repo.get_pkg(pkgname)
					if pkg is None:
						continue
					found = True
					pkginfo.display_pkginfo(pkg, level = args.info, style = 'sync')
				if not found:
					retcode = 1
					print("error: package '%s' was not found" % pkgname)
	return retcode

def main(rawargs):
	parser = config.make_parser(prog = 'pycman-sync')
	group = parser.add_argument_group("sync options")
	group.add_argument('-i', '--info',
			action = 'count', dest = 'info', default = 0,
			help = 'view package information')
	group.add_argument('-g', '--groups', action = 'store_true', default = False,
			help = 'view list of groups, or all members of a package group')
	group.add_argument('-l', '--list', action = 'store_true', default = False,
			help = 'list the contents of repositories')
	group.add_argument('-q', '--quiet',
			action = 'store_true', dest = 'quiet', default = False,
			help = 'show less information for query and search')
	group.add_argument('args', metavar = 'arg', nargs = '*',
			help = 'arguments (group names for -g, repo names for -l, '
			'package names for -i)')

	args = parser.parse_args(rawargs)
	config.init_with_config_and_options(args)

	if args.verbose:
		print("sync " + " ".join(rawargs), file = sys.stderr)

	if args.groups:
		return show_groups(args)
	elif args.info:
		return show_packages(args)
	elif args.list:
		return show_repo(args)

	return 0

if __name__ == "__main__":
	ret = main(sys.argv[1:])
	sys.exit(ret)

# vim: set ts=4 sw=4 tw=0 noet:
