#! /usr/bin/python 

import os
import re
import random
import sys
import time
from optparse import OptionParser
import logging
import subprocess
import time
import traceback

def OptsHasName(opts,name):
	if not hasattr(opts,name):
		return 0
	if getattr(opts,name) is None:
		return 0

	return 1

def SetArgsInt(args,name):
	args.add_option('--%s'%(name),action='store',nargs=1,type='int',dest='%s'%(name),default=None,help='to set %s'%(name))
	return

def SetArgsStr(args,name):
	args.add_option('--%s'%(name),action='store',nargs=1,type='string',dest='%s'%(name),default=None,help='to set %s'%(name))
	return
def SetArgsCallback(args,name,callfn):
	args.add_option('--%s'%(name),action='callback',type="string",callback=callfn,dest='%s'%(name),default=None,help='to set %s'%(name))
	return



def Usage(opt,ec,msg=None):
	fp = sys.stderr
	if ec == 0:
		fp = sys.stdout
	if msg :
		fp.write('%s\n'%(msg))
	opt.print_help(fp)
	sys.exit(ec)

def VerifyOptions(opts,args):
	hasvalue = 0
	if not OptsHasName(opts,'host'):
		Usage(args,3,'please specify host -H or --host')


	if not OptsHasName(opts,'user'):
		Usage(args,3,'please specify user by -u or --user')
	if not OptsHasName(opts,'passfile'):
		Usage(args,3,'please specify passfile by -P or --passfile')
	

	return 

def LoginUser(host,port,user,passfile,timeout,cmds=[]):
	cmd = 'ssh -i %s -p %s %s@%s '%(passfile,port,user,host)
	for c in cmds:
		cmd += ' %s'%(c)
	pssub = subprocess.Popen(cmd,shell=True,stdin=subprocess.PIPE,stdout=subprocess.PIPE,stderr=subprocess.PIPE)
	pssub.stdin.write('exit\n')
	pssub.stdout.readlines()
	return
		

def LogUser(opts,cmds):
	i = 0
	while opts.times == 0 or i < opts.times:		
		LoginUser(opts.host,opts.port,opts.user,opts.passfile,opts.timeout,cmds)
		if (i%100)==0:
			logging.info('%d times'%(i))
		i +=1
	return


def main():
	args = OptionParser()
	args.add_option('-v','--verbose',action='store_true',dest='verbose',help='verbose mode')
	args.add_option('-H','--host',action='store',dest='host',nargs=1,help='specify the host')
	args.add_option('-p','--port',action='store',dest='port',nargs=1,default=22,help='specify port default is 22')
	args.add_option('-u','--user',action='store',dest='user',nargs=1,help='specify username')
	args.add_option('-P','--passfile',action='store',dest='passfile',nargs=1,default=None,help='specify security private key')
	args.add_option('-T','--times',action='store',type='int',dest='times',nargs=1,default=0,help='specify times to login default is 0 for no-limited')
	args.add_option('-t','--timeout',action='store',type='float',dest='timeout',nargs=1,default=0.0,help='specify time to log default is 0.0')
	
	options ,nargs = args.parse_args(sys.argv[1:])

	VerifyOptions(options,args)

	if options.verbose :
		logging.basicConfig(level=logging.INFO,format="%(asctime)s %(levelname)-8s [%(filename)-10s:%(funcName)-20s:%(lineno)-5s] %(message)s")

	LogUser(options,nargs)

	return

if __name__ == '__main__':
	main()


