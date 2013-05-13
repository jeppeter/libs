#! python

import os
import sys
import logging
from optparse import OptionParser
sys.path.append(os.path.dirname(os.path.abspath(__file__)))
sys.path.append(os.path.join(os.path.dirname(os.path.abspath(__file__)),'.'))
import xunit.config
import xunit.suite
import unittest
from xunit.utils import exception
import re

def SplitVariables(v):
	#logging.info('v %s'%(v))
	p = '\[([^]]+)\]\.([^=]+)=(.*)'
	vpat = re.compile(p)
	m = vpat.match(v)
	if not m:
		raise exception.XUnitException('(%s) not match (%s)'%(v,p))
	sec = m.group(1)
	opt = m.group(2)
	val = m.group(3)
	return sec,opt,val
	

def SetOuterVariables(utcfg,variables=[]):
	# first to parse the variable
	for va in variables:
		s,o,v = SplitVariables(va)
		utcfg.SetValue(s,o,v,1)
	return

def Runtest(cfname,variables=[]):
	utcfg = xunit.config.XUnitConfig()
	SetOuterVariables(utcfg,variables)
	# now to add the %(build.topdir)s
	utcfg.SetValue('build','topdir',os.path.dirname(os.path.abspath(__file__)))
	utcfg.LoadFile(cfname)
	# we need to set the variable to be overwrited
	SetOuterVariables(utcfg,variables)

	# now we should get
	units = utcfg.GetUnitTests()
	suites = xunit.suite.XUnitSuiteBase()
	if len(units) > 0:
		for u in units:
			suites.LoadCase(u)

	# now we should set for the case verbose is none we debug our self information
	_res = xunit.result.XUnitResultBase()
	for s in suites:
		s(_res)
		if _res.shouldStop:
			break

	_ret = -1
	if _res.Fails() == 0 and _res.UnexpectFails() ==0 and _res.UnexpectSuccs() == 0:
		_ret = 0

	_res.ResultAll()

	return _ret



def Usage(opt,ec,msg=None):
	fp = sys.stderr
	if ec == 0:
		fp = sys.stdout
	if msg :
		fp.write('%s\n'%(msg))
	opt.print_help(fp)
	sys.exit(ec)

def Parse_Callback(option, opt_str, value, parser):
	#print 'option %s opt_str %s value %s parser %s'%(repr(option), repr(opt_str), repr(value), repr(parser))
	if opt_str == '-D' or opt_str == '--variable':
		if not hasattr(parser.values,'variables'):
			parser.values.variables = []
		if value :
			parser.values.variables.append(value)
	elif opt_str == '-V' or opt_str == '--debug':
		if not hasattr(parser.values,'variables'):
			parser.values.variables = []
		parser.values.variables.append('[global].xmllevel=5')
		
	else:
		Usage(parser,3,'unknown option (%s)'%(option))


if __name__ == '__main__':
	args = OptionParser()
	args.add_option('-v','--verbose',action='count',dest='verbose',help='verbose mode if -vvv debug most')
	args.add_option('-f','--failfast',action="store_true",dest="failfast",help="failfast mode")
	args.add_option('-x','--xmllog',action='store',dest='xmllog',nargs=1,help='set xmllog file defautlt is none')
	args.add_option('-D','--variable',action="callback",callback=Parse_Callback,type="string",nargs=1,help='specify a variable format is [section].option=value')
	args.add_option('-V','--debug',action="callback",callback=Parse_Callback,help='debug mode ')
	

	options ,nargs = args.parse_args(sys.argv[1:])
	if len (nargs) < 1:
		Usage(args,3,"need one config files")

	_vars = []
	utcfg = xunit.config.XUnitConfig()

	
	if options.verbose:
		logging.basicConfig(level=logging.INFO,format="%(levelname)-8s [%(filename)-10s:%(funcName)-20s:%(lineno)-5s] %(message)s")
		_vars.append('[global].debug.mode=y')
		if options.verbose >= 3:
			_vars.append('[.__main__].xmllevel=5')

	if options.failfast:
		_vars.append('[global].failfast=y')
	if options.xmllog:
		_vars.append('[global].xmllog=%s'%(options.xmllog))

	#logging.info('opt %r'%(options))
	if hasattr(options,'variables'):
		_vars.extend(options.variables)
	_ret = Runtest(nargs[0],_vars)
	if _ret != 0:
		sys.exit(3)
	sys.exit(0)
