#! python

import ftplib
import sys
import os


def Login(host,port=21,user=None,pwd=None):
	fc = ftplib.FTP()
	fc.set_debuglevel(2)
	fc.connect(host,port)
	if user is None and pwd is None:
		fc.login
	else:
		fc.login(user,pwd)
	return fc



def ftpput(fc,rdir,fname):
	bname = os.path.basename(fname)
	# now first to change dir
	fc.cwd(rdir)
	fs = fc.nlst()
	print('%s'%(fs))
	if bname in fs:
		fc.delete(bname)
	fc.storbinary('STOR '+bname,open(fname,'rb'))
	return

def test():	
	fc = Login(sys.argv[1],int(sys.argv[2]),sys.argv[3],sys.argv[4])
	ftpput(fc,sys.argv[5],sys.argv[6])
	fc.close()
	return

if __name__ == '__main__':
	test()
