#!/usr/bin/python

import re
import sys
import argparse
import logging

def match(restr,instr):
	expr = re.compile(restr)	
	if expr.match(instr):
		print '(%s) match (%s)'%(instr,restr)
	else:
		print '(%s) not match (%s)'%(instr,restr)
	return

def findall(restr,instr):
	expr = re.compile(restr)
	m =  expr.findall(instr)
	if m :
		s = '(%s) match (%s)\n'%(instr,restr)
		i = 0
		for cm in m:
			s += '\t[%d] %s\n'%(i,cm)
			i += 1
		print '%s'%(s)
	else:
		print '(%s) no more for (%s)'%(instr,restr)
	return

def imatch(restr,instr):
	expr = re.compile(restr,re.I)
	if expr.match(instr):
		print '(%s) ignore match (%s)'%(instr,restr)
	else:
		print '(%s) not ignore match (%s)'%(instr,restr)
	return

def ifindall(restr,instr):
	expr = re.compile(restr,re.I)
	m =  expr.findall(instr)
	if m :
		s = '(%s) match (%s)\n'%(instr,restr)
		i = 0
		for cm in m:
			s += '\t[%d] (%s)\n'%(i,cm)
			i += 1
		print '%s'%(s)
	else:
		print '(%s) no more for (%s)'%(instr,restr)
	return


def Usage(ec,fmt,parser):
	fp = sys.stderr
	if ec == 0 :
		fp = sys.stdout

	if len(fmt) > 0:
		fp.write('%s\n'%(fmt))
	parser.print_help(fp)
	sys.exit(ec)

def main():
	parser = argparse.ArgumentParser(description='re test',usage='%s [options]'%(sys.argv[0]))	
	#parser.add_argument('-r','--restr',default=None,help='re str to set')
	#parser.add_argument('-i','--instr',default=None,help='instr to set')
	parser.add_argument('-v','--verbose',default=0,action='count')
	sub_parser = parser.add_subparsers(help='',dest='command')
	match_parser = sub_parser.add_parser('match',help='re.match')
	ignore_parser = sub_parser.add_parser('imatch',help='re.match with re.I')
	match_parser.add_argument('strs',metavar='N',type=str,nargs='+',help='restr instr')
	ignore_parser.add_argument('strs',metavar='N',type=str,nargs='+',help='restr instr')
	findall_parser = sub_parser.add_parser('findall',help='re.findall')
	ifindall_parser = sub_parser.add_parser('ifindall',help='re.findall with re.I')
	findall_parser.add_argument('strs',metavar='N',type=str,nargs='+',help='restr instr')
	ifindall_parser.add_argument('strs',metavar='N',type=str,nargs='+',help='restr instr')
	args = parser.parse_args()
	#if args.restr is None or args.instr is None:
	#	Usage(3,'need instr and restr',parser)

	loglvl= logging.ERROR
	if args.verbose >= 3:
		loglvl = logging.DEBUG
	elif args.verbose >= 2:
		loglvl = logging.INFO
	logging.basicConfig(level=loglvl,format='%(asctime)s:%(filename)s:%(funcName)s:%(lineno)d\t%(message)s')
	if len(args.strs) < 2:
		Usage(3,'restr instr need',parser)
	if args.command == 'match':
		match(args.strs[0],args.strs[1])
	elif args.command == 'findall':
		findall(args.strs[0],args.strs[1])
	elif args.command == 'ifindall':
		ifindall(args.strs[0],args.strs[1])
	elif args.command == 'imatch':
		imatch(args.strs[0],args.strs[1])
	else:
		Usage(3,'unrecognize %s'%(args.command),parser)
	return


if __name__ == '__main__':
	main()