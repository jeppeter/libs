#!python

import subprocess
import time
import sys
import threading
import os


class ProxyRun(threading.Thread):
	def __init__(self,proxyurl,geturl):
		self.__proxy = proxyurl
		self.__get = geturl
		self.__exited = 1
		self.__exitcode= -1
		self.__p = None
		self.__stime = None
		self.__etime = None
		threading.Thread.__init__(self)
		return

	def __run_proxy(self):
		try:
			fdir = os.path.abspath(os.path.dirname(__file__))
			fprox = fdir + os.path.seperator + 'proxytest.py'
			cmd = "python '%s' '%s' '%s' "%(fprox,self.__proxy,self.__get)
			self.__p = subprocess.subprocess.Popen(cmds,stdout=subprocess.PIPE,stderr=subprocess.PIPE,shell=True)
		except:
			return -1
		return 0

	def __ex_readline(self,f,ch='\r'):
		s = ''
		while True:
			b = f.read(1)
			if b is None or len(b) == 0:
				if len(s) == 0:
					return None
				return s
			if b != ch and b != '\n':
				s += b
				continue
			return s
		return s


	def run(self):
		self.__exited = 0
		ret = self.__run_proxy()
		if ret != 0 :
			self.__exited = 1
			self.__exitcode = -1
			return
		self.__stime = time.time()
		while True:
			pret = self.__p.poll()
			if pret is not None:
				self.__exitcode = pret
				break
			rl = self.__ex_readline(self.__p.stderr)
			rl = self.__ex_readline(self.__p.stdout)
		if self.__exitcode == 0 :
			self.__etime = time.time()
		self.__exited = 1
		return

	def is_exited(self):
		if self.__exited :
			return True
		else:
			return False

	def get_time(self):
		if self.__stime is not None and self.__etime is not None :
			return (self.__etime - self.__stime)
		return 0xffffffff

	def stop_running(self):		
		if self.__p is not None:
			cnt = 0
			pids = [self.__p.pid]			
			while self.isAlive():
				cnt += 1
				try:
					for curpid in pids:
						ctypes.windll.kernel32.GenerateConsoleCtrlEvent(0,curpid)
					time.sleep(0.3)
				except OSError as e:
					pass
				except:
					pass
			self.__exitcode = self.__p.poll()
			del self.__p
			self.__p = None
		return


def main():
	if len(sys.argv) < 3:
		sys.stderr.write('%s proxyurl geturl\n'%(sys.argv[0]))
		sys.exit(4)
	prun = ProxyRun(sys.argv[1],sys.argv[2])
	prun.start()
	try:
		time.sleep(0.3)
		while True:
			if prun.is_exited():
				break
			time.sleep(0.4)
		sys.stdout.write('in (%s) get (%s) time (%d)\n'%(sys.argv[1],sys.argv[2],prun.get_time()))
	except:
		prun.stop_running()
		sys.stdout.write('run error\n')

if __name__ == '__main__':
	main()

