#!/usr/bin/python

import os
import sys
import subprocess
import logging
sys.path.append(os.path.abspath(os.path.dirname(__file__)))
import dbgexp

def run_cmd_wait(cmd,mustsucc=1):
	logging.debug('run (%s)'%(cmd))
	ret = subprocess.call(cmd,shell=True)
	if mustsucc and ret != 0:
		raise dbgexp.DebugException(dbgexp.ERROR_RUN_CMD,'run cmd (%s) error'%(cmd))
	return ret

def run_read_cmd(cmd):
	logging.debug('run (%s)'%(cmd))
	p = subprocess.Popen(cmd,stdout=subprocess.PIPE,stderr=subprocess.PIPE,shell=True)
	return p

def read_line(pin,ch='\r'):
	s = ''
	while True:
		b = pin.read(1)
		if b is None or len(b) == 0:
			if len(s) == 0:
				return None
			return s
		if b != ch and b != '\n':
			s += b
			continue
		return s
	return s


def run_command_callback(cmd,callback,ctx):
	p = run_read_cmd(cmd)
	exited = 0
	exitcode = -1
	while exited == 0:
		pret = p.poll()
		logging.debug('pret %s'%(repr(pret)))
		if pret is not None:
			exitcode = pret
			exited = 1
			logging.debug('exitcode (%d)'%(exitcode))
			while True:
				rl = read_line(p.stdout)
				logging.debug('read (%s)'%(rl))
				if rl is None:
					break
				callback(rl,ctx)
			break
		else:
			rl = read_line(p.stdout)
			logging.debug('read (%s)'%(rl))
			if rl :
				callback(rl,ctx)
	return exitcode
