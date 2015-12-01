#!python

import sys
import codecs

def ReEncode(infile,outfile):
	f = open(infile,'r+b')
	fo = open(outfile,'w+b')	
	while True:
		l = f.read()
		if l is None or len(l) == 0:
			break
		wl = ""
		for a in l:
			if ord(a) < 0x80:
				wl += a
		fo.write(wl)
	fo.close()
	f.close()
	return

def main():
	if len(sys.argv) < 2:
		sys.stderr.write('%s infile\n'%(sys.argv[0]))
		sys.exit(4)
	ReEncode(sys.argv[1],'saved.html')

if __name__ == '__main__':
	main()
