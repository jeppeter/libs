#!/usr/bin/python

import sys
import os
import logging
import argparse
import re

def replace_string_file(fromstr,tostr,infile):
	totals = ''
	with open(infile,'r+b') as f:
		for l in f:
			s = l.replace(fromstr,tostr)
			totals += s
	return totals

def write_string_file(str,outfile):
	with open(outfile,'w+b') as f:
		f.write(str)
	return

def replace_string_files(fromstr,tostr,indir,filterdirs=[]):	
	seperator = '\\%c'%(os.path.sep)
	for root,dirs,files in os.walk(indir,topdown=True):
		rootelems = []
		if root != '':
			rootelems = os.path.split(root)
			rootelems = re.split(seperator,root)
		filterout = 0
		#logging.debug('rootelems %s filterdirs %s'%(rootelems,filterdirs))

		for f in filterdirs:
			if f in rootelems:
				filterout = 1
		if filterout == 0 :
			logging.debug('root %s dirs %s files %s'%(root,dirs,files))
			for f in files:
				curf = '%s%c%s'%(root,os.path.sep,f)
				s = replace_string_file(fromstr,tostr,curf) 
				write_string_file(s,curf)


def Usage(ec,fmt,args):
	fp = sys.stderr
	if ec == 0:
		fp = sys.stdout
	if fmt is not None:
		fp.write(fmt)
		fp.write('\n')
	args.print_help(fp)
	sys.exit(ec)

def main():
	parser = argparse.ArgumentParser(description='replace string in a directory',usage='%s [options]'%(sys.argv[0]))	
	parser.add_argument('-v','--verbose',default=0,action='count',help='to make verbose mode')
	parser.add_argument('-f','--filter',default=['.git'],action='append_const',const=str,help='to add filters not to handle')
	parser.add_argument('-d','--directory',default='.',type=str,help='to specify the directory to do default is .')
	parser.add_argument('strhandle',default=[],nargs='+',help='fromstr tostr')
	args = parser.parse_args()

	loglvl= logging.ERROR
	if args.verbose >= 3:
		loglvl = logging.DEBUG
	elif args.verbose >= 2:
		loglvl = logging.INFO
	logging.basicConfig(level=loglvl,format='%(asctime)s:%(filename)s:%(funcName)s:%(lineno)d\t%(message)s')

	if len(args.strhandle) < 2:
		Usage(3,"must specify fromstr tostr",parser)
	logging.debug('args %s'%(args))
	replace_string_files(args.strhandle[0],args.strhandle[1],args.directory,args.filter)


if __name__ == '__main__':
	main()