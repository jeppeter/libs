#!/usr/bin/python

import sys
import argparse
import logging
import urllib2


def get_proxy_list(proxyurl,proxylisturl):
	lists = None
	proxy = urllib2.ProxyHandler({'http' : proxyurl, 'https' : proxyurl})
	opener = urllib2.build_opener(proxy)
	urllib2.install_opener(opener)
	response = urllib2.urlopen(proxylisturl)
	lists = response.read()
	return lists


def get_proxy_lists(proxyurl,proxylistbaseurl):
	proxylisturl = '%s/list_1.html'%(proxylistbaseurl)
	print get_proxy_list(proxyurl,proxylisturl)
	return

def main():
	if len(sys.argv) < 3:
		sys.stderr.write('%s proxyurl proxy_home\n'%(sys.argv[1]))
		sys.exit(4)
	ret = get_proxy_lists(sys.argv[1],sys.argv[2])
	return

if __name__ == '__main__':
	main()
