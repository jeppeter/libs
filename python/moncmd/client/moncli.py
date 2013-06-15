#! python

import sys
from optparse import OptionParser
import os
def Usage(ec,opt,msg=None):
	fp = sys.stderr
	if ec == 0 :
		fp = sys.stdout
	if msg is not None:
		fp.write('%s\n'%(msg))
	opt.print_help(fp)	
	sys.exit(ec)
	return

def Parse_Callback(option, opt_str, value, parser):
	if hasattr(parser.values,'process'):
		parser.values.process.append(value)
	else:
		parser.values.process = []
		parser.values.process.append(value)
	return
def main():
	args = OptionParser()
	args.add_option('-v','--verbose',action='store_true',dest="verbose",help='verbose mode')
	args.add_option('-H','--host',action="store",type="string",dest='host',default="",help='specify host port host:port')
	args.add_option('-p','--process',action='callback',callback=Parse_Callback,help='to append process search')
	opt,nargs = args.parse_args(sys.argv[1:])

	

if __name__ == '__main__':
	main()
