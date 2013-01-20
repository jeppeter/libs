#! python

import os
#os.path.append(".")

import PyQQ
import sys
from optparse import OptionParser

def Usage(opt,exitcode,msg=None):
	fp = sys.stderr
	if exitcode == 0:
		fp = sys.stdout
	if msg is not None:
		fp.write(msg)
	opt.print_usage(fp)
	sys.exit(exitcode)
	

if __name__ == '__main__':
	oparse = OptionParser()
	oparse.add_option('-q','--qq',action="append",dest="qqs",help="qq number set")
	oparse.add_option('-p','--password',action="append",dest="pwds",help="qq password to set,please append it immediate after the -q or --qq")

	(options,args)=oparse.parse_args()
	if options['qqs'] is None or len(options['qqs']) < 2:
		Usage(oparse,3,"Must specify two qqs")
	if options['pwds'] is None or len(options['pwds']) != len(options['pwds']):
		Usage(oparse,3,"passwords must according to qqs")
	try:
		qq1 = PyQQ()
		qq2 = PyQQ()
		qq1.login(options['qqs'][0],options['pwds'][0])
		qq2.login(options['qqs'][1],options['pwds'][1])
	except PyQQException as e:
		sys.stderr.write("Error %s\n"%(str(e)))
		sys.exit(3)

	