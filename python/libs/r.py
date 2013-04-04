#! python

import re
import sys

def MatchString(p,s):
	spat = re.compile(p)
	if spat.search(s):
		print '(%s) match (%s)'%(p,s)
		sarr = re.findall(p,s)
		print 'match (%s) (%s) (%s)'%(p,s,repr(sarr))
	else:
		print '(%s) not match (%s)'%(p,s)


if __name__ == '__main__':
	MatchString(sys.argv[1],sys.argv[2])
