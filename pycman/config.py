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

import io
import os
import sys
import argparse
import configparser

import pyalpm

def read_config(path):
	f = open(path)
	preprocessed = io.StringIO()
	for line in f:
		if '=' in line:
			# Process Include statements
			key, value = line.split('=', 1)
			key = key.strip()
			value = value.strip()
			if key == 'Include':
				with open(value) as g:
					preprocessed.write(g.read())
				continue
		preprocessed.write(line)

	f.close()

	parser = configparser.ConfigParser(
			allow_no_value = True,
			delimiters = ['='],
			comment_prefixes = ['#'],
			empty_lines_in_values = False)
	parser.read_string(preprocessed.getvalue(), source=path)
	# parser.write(sys.stderr)
	return parser

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

def init_with_config(configpath):
	"Reads configuration from given path and apply it to libalpm"
	config = read_config(configpath)
	arch = os.uname()[-1]
	try:
		config_options = config["options"]
	except KeyError:
		config_options = {}

	pyalpm.options.root = config_options.get("rootdir", "/")
	pyalpm.options.dbpath = config_options.get("dbpath", "/var/lib/pacman")
	pyalpm.options.arch = config_options.get("architecture", arch)
	pyalpm.options.logfile = config_options.get("logfile", "/var/log/pacman.log")
	# set sync databases
	for repo in config.sections():
		if repo == "options":
			continue
		pyalpm.register_syncdb(repo)

def init_with_config_and_options(options):
	"Reads configuration from file and commandline options, and apply it to libalpm"
	pyalpm.initialize()

	# read config file
	if options.config is not None:
		config_file = options.config
	else:
		config_file = "/etc/pacman.conf"
	config = read_config(config_file)

	try:
		config_options = config["options"]
	except KeyError:
		config_options = {}

	# set libalpm options
	if options.root is not None:
		pyalpm.options.root = options.root
	else:
		pyalpm.options.root = config_options.get("rootdir", "/")

	if options.dbpath is not None:
		pyalpm.options.dbpath = options.dbpath
	else:
		pyalpm.options.dbpath = config_options.get("dbpath", "/var/lib/pacman")

	if options.arch is not None:
		pyalpm.options.arch = options.arch
	else:
		arch = os.uname()[-1]
		pyalpm.options.arch = config_options.get("architecture", arch)

	if options.logfile is not None:
		pyalpm.options.logfile = options.logfile
	else:
		pyalpm.options.logfile = config_options.get("logfile", "/var/log/pacman.log")

	# set sync databases
	for repo in config.sections():
		if repo == "options":
			continue
		pyalpm.register_syncdb(repo)

# vim: set ts=4 sw=4 tw=0 noet:
