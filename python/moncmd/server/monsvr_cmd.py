#! python

'''
	this file to handle the client connect 
	in command line 
'''

import sys
import os
import monsvrimpl

svrport_const = 9123
svrport=svrport_const
svrtmout_const = 60
svrtmout = svrtmout_const
svrprompt = 'svr>'
svrthreadimpl = None

def Prompt()
	sys.stdout.write(svrprompt)
	sys.stdout.flush()
	return
def GetCommand()
	l = sys.stdin.readline()
	l = l.rstrip('\r\n')
	return l

def ListCommand():
	pass
def RunCommand():
	pass

def HelpCommand():
	pass

def command_ui():
	while True:
		Prompt()
		cmd = GetCommand()
		if cmd[:4] == 'quit' or cmd[:4] == 'exit':
			break
		elif cmd[:4] == 'list':
			ListCommand(cmd)
		elif cmd[:3] == 'run':
			RunCommand(cmd)
		elif cmd[:4] == 'help':
			HelpCommand(cmd)
		elif len(cmd) == 0:
			break
		else:
			sys.stdout.write('can not parse (%s)\n'%(cmd))
			sys.stdout.flush()
def main():
	command_ui()
	sys.exit(0)

if __name__ == '__main__':
	main()

