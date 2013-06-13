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
	fp.write('%s [OPTIONS]\n'%(os.path.basename(__file__)))
	fp.write('\t-H host    : specify the hostname and port host:port\n')
	fp.write('\t-p  process : specify the process to handle\n')
	sys.exit(ec)
	return

def main():
	args = OptionParser()
	args.add_option

if __name__ == '__main__':
	main()
