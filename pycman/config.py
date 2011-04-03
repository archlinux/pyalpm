#!/usr/bin/env python3
# 
# pycman - A Python implementation of Pacman
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
pycman configuration handling

This module handles pacman.conf files as well as pycman options that
are common to all action modes.
"""

import argparse
import pyalpm

def read_config(path):
	pass

def make_parser(*args, **kwargs):
	parser = argparse.ArgumentParser(*args, **kwargs)
	common = parser.add_argument_group('Common options')
	common.add_argument('-b', '--dbpath', metavar = '<path>',
			action = 'store', dest = 'dbpath', type = str,
			help = 'set an alternate database location')
	common.add_argument('-r', '--root', metavar = '<path>',
			action = 'store', dest = 'root', type = str,
			help = 'set an alternate installation root')
	common.add_argument('-v', '--verbose',
			action = 'store_true', dest = 'verbose', default = False,
			help = 'be verbose')
	common.add_argument('--arch', metavar = '<arch>',
			action = 'store', dest = 'arch', type = str,
			help = 'set an alternate architecture')
	common.add_argument('--config', metavar = '<file>',
			action = 'store', dest = 'config', type = str,
			help = 'set an alternate configuration file')
	common.add_argument('--logfile', metavar = '<file>',
			action = 'store', dest = 'logfile', type = str,
			help = 'set an alternate log file')
	return parser

def init_with_config(options):
	pyalpm.initialize()

	# read config file
	if options.config is not None:
		config_file = options.config
	else:
		config_file = "/etc/pacman.conf"
	config = read_config(config_file)

	# set libalpm options
	if options.root is not None:
		pyalpm.options.root = options.root
	else:
		pyalpm.options.root = "/"

	if options.dbpath is not None:
		pyalpm.options.dbpath = options.dbpath
	else:
		pyalpm.options.dbpath = "/var/lib/pacman"

	if options.arch is not None:
		pyalpm.options.arch = options.arch

# vim: set ts=4 sw=4 tw=0 noet:
