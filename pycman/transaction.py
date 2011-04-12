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

import pyalpm

# Callbacks
def cb_event(*args):
	print("event", args)

def cb_conv(*args):
	print("conversation", args)

def cb_progress(*args):
	print("progress", args)

# Transaction initialization
def init_from_options(options):
	t = pyalpm.transaction
	t.init(
			cascade = getattr(options, "cascade", False),
			nodeps = getattr(options, "nodeps", False),
			force = getattr(options, 'force', False),
			dbonly = getattr(options, 'dbonly', False),
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

# vim: set ts=4 sw=4 tw=0 noet:
