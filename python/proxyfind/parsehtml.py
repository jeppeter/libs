#!python
import sys
import re

def ParseHtml(htmlfile):
	f = open(htmlfile,'r')
	proxys = []
	mat = re.compile('<tr><td>(\d+)</td><td>(((\d+)\.)+(\d+))</td><td>(\d+)</td><td>')
	for l in f:
		mobj = mat.match(l)
		if mobj :
			proxys.append('%s:%s'%(mobj.group(2),mobj.group(6)))	
	return proxys



def main():
	if len(sys.argv) < 2:
		sys.stderr.write('%s htmlfile\n'%(sys.argv[0]))
		sys.exit(4)
	proxys = ParseHtml(sys.argv[1])
	for p in proxys:
		print p

if __name__ == '__main__':
	main()