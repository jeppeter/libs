import signal
import os
import sys
import time

Running = 1

def SigHandler(signo,frame):
	print "caught signo %d"%(signo)
	global Running
	Running = 0

signal.signal(signal.SIGINT,SigHandler)

while Running == 1:
	print " one Second"
	try:
		time.sleep(1)
	except:
		pass
print "Exit"
