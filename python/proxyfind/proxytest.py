#!/usr/bin/python

import sys
import logging
import urllib2
import argparse


def get_url_by_proxy(proxyurl,geturl):
	try:
		proxy = urllib2.ProxyHandler({'http' : proxyurl, 'https' : proxyurl})
		opener = urllib2.build_opener(proxy)
		urllib2.install_opener(opener)
		response = urllib2.urlopen(geturl)
	except :
		return -1
	return 0


def main():
	if len(sys.argv) < 3:
		sys.stderr.write('%s proxyurl geturl\n'%(sys.argv[1]))
		sys.exit(4)
	ret = get_url_by_proxy(sys.argv[1],sys.argv[2])
	if ret != 0 :
		sys.exit(5)
	sys.exit(0)

if __name__ == '__main__':
	main()