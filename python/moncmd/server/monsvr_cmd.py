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

def ListCommand(cmd):
	global svrthreadimpl
	cmds = cmd.split()
	if cmds[0] != 'list':
		sys.stdout.write('not list command\n')
		sys.stdout.flush()
		return 
	if svrthreadimpl is None:
		sys.stdout.write('not run back ground svr\n')
		sys.stdout.flush()
		return
	reports = []
	if len(cmds) > 1:
		for c in cmds[1:]:
			try:
				report = svrthreadimpl.GetReportResult(c)
				reports.append(report)
			except:
				pass
	else:
		try:
			report = svrthreadimpl.GetReportResult()
			reports = report
		except:
			pass

	sys.stdout.write('there are %d result%s\n'%(len(reports), len(reports) > 1 and 's' or ' '))
	sys.stdout.write('%15s|%10s|%25s\n'%('addr','time','result'))
	for r in reports:
		assert(len(r) >= 3)
		sys.stdout.write('%25s|%10s|%25s\n'%(r[0],r[1],r[2]))
	return
def RunCommand(cmd):
	cmds = cmd.split()
	if len(cmds) ==0 or cmds[0] != 'run':
		sys.stdout.write('not run cmd (%s)\n'%(cmd))
		return
	addrs = []
	for i in xrange(1,len(cmds)):
		if cmds[i] == '--':
			break
		addrs.append(cmds[i])
	if len(addrs) >= (len(cmds) - 2):
		sys.stdout.write('no cmds for run ,please use --\n')
		return

def HelpCommand():
	fp = sys.stdout
	fp.write('help                                        : for list this help information\n')
	fp.write('list  [ip:port]...                       : for list the information none list all\n')
	fp.write('run [ip:port] -- [cmds]...      : for run command\n')
	fp.write('quit | exit                              : exit command\n')
	fp.write('\n')
	fp.flush()
	return

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

