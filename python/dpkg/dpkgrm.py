#!/usr/bin/python
import os
import sys
import argparse
import logging
sys.path.append(os.path.abspath(os.path.dirname(__file__)))
import dpkgdep

class DpkgRmBase(object):
	def __init__(self,dpkg='dpkg',aptcache='apt-cache'):
		self.__dpkg = dpkg
		self.__aptcache= aptcache
		return

	def __remove_recursive(self,pkg):
		rdeps = dpkgdep.get_all_rdeps(pkg,self.__aptcache,self.__dpkg)
		retpkgs = []
		if len(rdeps) > 0:
			for p in rdeps:
				pkgs = self.__remove_recursive(p)
				for p in pkgs:
					if p not in retpkgs:
						retpkgs.append(p)
		logging.info('remove %s'%(pkg))
		retpkgs.append(pkg)		
		return retpkgs


	def remove_not_dep(self,pkgs):
		allinsts = dpkgdep.get_all_inst(self.__dpkg)
		alldeps = dpkgdep.get_all_deps(pkgs,self.__aptcache)
		rmpkgs = []
		for p in allinsts:
			if p not in alldeps:
				rmpkgs.append(p)
		while len(rmpkgs):
			retpkgs = self.__remove_recursive(rmpkgs[0])
			newallrdeps = []
			for p in allrdeps:
				if p not in retpkgs:
					newallrdeps.append(p)
			allrdeps = newallrdeps
		return

