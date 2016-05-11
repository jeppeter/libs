#!/usr/bin/python
import re
import os
import sys
import argparse
import logging
sys.path.append(os.path.abspath(os.path.dirname(__file__)))
import dbgexp
import cmdpack


class DpkgDependBase(object):
	def __init__(self):
		self.__depends = []
		self.__needmore = False
		self.__moreexpr = re.compile('<([^>]+)>')
		self.__depexpr = re.compile('\s*depends:\s+([^\s]+)\s*',re.I)
		return

	def __add_inner(self,pkg):
		if pkg is None:
			return
		if pkg not in self.__depends:
			self.__depends.append(pkg)
		return

	def add_depend(self,l):
		dependpkg = None
		l = l.rstrip('\r\n')
		if self.__needmore :
			sarr = re.split(':',l)
			if len(sarr) < 2:
				dependpkg = l.strip(' \t')
				dependpkg = dependpkg.rstrip(' \t')
				self.__add_inner(dependpkg)
				return
		self.__needmore = False
		m = self.__depexpr.findall(l)
		if m :
			dependpkg = m[0]
			if self.__moreexpr.match(dependpkg):
				self.__needmore = True
				dependpkg = None
		self.__add_inner(dependpkg)
		return

	def add_depend_direct(self,pkg):
		self.__add_inner(pkg)
		return

	def get_depend(self):
		return self.__depends

class DpkgRDependBase(object):
	def __init__(self):
		self.__rdepends = []
		self.__started = False
		self.__rdepexpr = re.compile('\s*reverse depends:\s*',re.I)
		return

	def __add_inner(self,pkg):
		if pkg is None:
			return
		if pkg not in self.__rdepends:
			self.__rdepends.append(pkg)
		return

	def get_rdepends(self):
		return self.__rdepends

	def add_depend(self,l):
		l = l.rstrip('\r\n')
		l = l.strip(' \t')
		l = l.rstrip(' \t')
		l = l.replace('|','')
		if not self.__started:
			logging.info('%s'%(l))
			if self.__rdepexpr.match(l):
				self.__started = True
			return
		self.__add_inner(l)
		return
	def reset_start(self):
		self.__started = False


def filter_depends(instr,context):
	context.add_depend(instr)
	return


class DpkgDepends(DpkgDependBase):
	def __init__(self,aptcache='apt-cache',sudoprefix='sudo'):
		super(DpkgDepends, self).__init__()
		self.__aptcache = aptcache
		self.__sudoprefix = sudoprefix
		return

	def get_depend_command(self,pkgname):
		cmds = '%s "%s" depends "%s"'%(self.__sudoprefix,self.__aptcache,pkgname)
		retval = cmdpack.run_command_callback(cmds,filter_depends,self)
		self.add_depend_direct(pkgname)
		if retval != 0 :
			raise Exception('run (%s) error'%(cmds))
		return self.get_rdepends()

class DpkgRDepends(DpkgRDependBase):
	def __init__(self,aptcache='apt-cache',sudoprefix='sudo'):
		super(DpkgRDepends, self).__init__()
		self.__aptcache = aptcache
		self.__sudoprefix = sudoprefix
		return

	def get_depend_command(self,pkgname):
		cmds = '%s "%s" rdepends "%s"'%(self.__sudoprefix,self.__aptcache,pkgname)
		self.reset_start()
		logging.info('run (%s)'%(cmds))
		retval = cmdpack.run_command_callback(cmds,filter_depends,self)
		if retval != 0 :
			raise Exception('run (%s) error'%(cmds))
		return self.get_rdepends()


def Usage(ec,fmt,parser):
	fp = sys.stderr
	if ec == 0 :
		fp = sys.stdout

	if len(fmt) > 0:
		fp.write('%s\n'%(fmt))
	parser.print_help(fp)
	sys.exit(ec)

def format_output(pkgs,getpkgs,outstr):
	s = ''
	i = 0
	j = 0
	for p in pkgs:
		if (i % 5)==0:			
			if i != 0:
				j += 1
			i = 0
			s += '\n'
		if i != 0 :
			s += ','
		s += '%s'%(p)
		i += 1
	s += '\n'
	i = 0
	j = 0
	s += '%s'%(outstr)
	for p in getpkgs:
		if (i % 5)==0:			
			if i != 0:
				j += 1
			i = 0
			s += '\n'
		if i != 0 :
			s += ','
		s += '%s'%(p)
		i += 1
	s += '\n'
	sys.stdout.write(s)
	return

def get_all_deps(pkgs,aptcache):
	dpkgdeps = DpkgDepends(aptcache)
	for p in pkgs:
		dpkgdeps.get_depend_command(p)
	return dpkgdeps.get_depend()

def get_all_rdeps(pkgs,aptcache):
	dpkgrdeps = DpkgRDepends(aptcache)
	for p in pkgs:
		dpkgrdeps.get_depend_command(p)
	return dpkgrdeps.get_rdepends()

def main():
	parser = argparse.ArgumentParser(description='apt-cache set',usage='%s [options] {commands} pkgs...'%(sys.argv[0]))	
	parser.add_argument('-v','--verbose',default=0,action='count')
	parser.add_argument('-a','--aptcache',default='apt-cache',action='store')
	sub_parser = parser.add_subparsers(help='',dest='command')
	dep_parser = sub_parser.add_parser('dep',help='get depends')
	dep_parser.add_argument('pkgs',metavar='N',type=str,nargs='+',help='package to get depend')
	rdep_parser = sub_parser.add_parser('rdep',help='get rdepends')
	rdep_parser.add_argument('pkgs',metavar='N',type=str,nargs='+',help='package to get rdepend')

	args = parser.parse_args()	

	loglvl= logging.ERROR
	if args.verbose >= 3:
		loglvl = logging.DEBUG
	elif args.verbose >= 2:
		loglvl = logging.INFO
	logging.basicConfig(level=loglvl,format='%(asctime)s:%(filename)s:%(funcName)s:%(lineno)d\t%(message)s')
	if len(args.pkgs) < 1:
		Usage(3,'packages need',parser)
	if args.command == 'dep':
		getpkgs = get_all_deps(args.pkgs,args.aptcache)
	elif args.command == 'rdep':
		getpkgs = get_all_rdeps(args.pkgs,args.aptcache)
	else:
		Usage(3,'can not get %s'%(args.command),parser)
	format_output(args.pkgs,getpkgs,args.command)
	return

if __name__ == '__main__':
	main()

