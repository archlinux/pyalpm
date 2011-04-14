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
pycman transaction management

This module defines convenient wrappers around pyalpm functions
to initialize transactions according to options
"""

import sys
import traceback
import pyalpm

# Callbacks
def cb_event(*args):
	print("event", args)

def cb_conv(*args):
	print("conversation", args)

last_target = None
last_percent = 100
last_i = -1
def cb_progress(target, percent, n, i):
	"Display progress percentage for target i/n"
	global last_target, last_percent, last_i
	if len(target) == 0:
		# abstract progress
		if percent < last_percent or i < last_i:
			sys.stdout.write("progress (%d targets)" % n)
			last_i = 0
		sys.stdout.write((i - last_i) * '.')
		sys.stdout.flush()
		last_i = i
	else:
		# progress for some target (write 25 dots for 100%)
		if target != last_target or percent < last_percent:
			last_target = target
			last_percent = 0
			sys.stdout.write("progress for %s (%d/%d)" % (target, i, n))
		old_dots = last_percent // 4
		new_dots = percent // 4
		sys.stdout.write((new_dots - old_dots) * '.')
		sys.stdout.flush()

	# final newline
	if percent == 100 and last_percent < 100:
		sys.stdout.write('\n')
		sys.stdout.flush()
	last_percent = percent

def cb_dl(filename, tx, total):
	print("download %s: %d/%d" % (filename, tx, total))

def init_from_options(options):
	"Transaction initialization"
	pyalpm.options.dlcb = cb_dl
	t = pyalpm.transaction
	t.init(
			cascade = getattr(options, "cascade", False),
			nodeps = getattr(options, "nodeps", False),
			force = getattr(options, 'force', False),
			dbonly = getattr(options, 'dbonly', False),
			downloadonly = getattr(options, 'downloadonly', False),
			nosave = getattr(options, 'nosave', False),
			recurse = (getattr(options, 'recursive', 0) > 0),
			recurseall = (getattr(options, 'recursive', 0) > 1),
			unneeded = getattr(options, 'unneeded', False),
			alldeps = (getattr(options, 'mode', None) == pyalpm.PKG_REASON_DEPEND),
			allexplicit = (getattr(options, 'mode', None) == pyalpm.PKG_REASON_EXPLICIT),
			event_callback = cb_event,
			conv_callback = cb_conv,
			progress_callback = cb_progress)
	return t

def finalize(t):
	"Commit a transaction"
	try:
		t.prepare()
		t.commit()
	except pyalpm.error:
		traceback.print_exc()
		t.release()
		return False
	t.release()
	return True

# vim: set ts=4 sw=4 tw=0 noet:
