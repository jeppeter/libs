#!/usr/bin/python
import sys
from HTMLParser import HTMLParser

class ProxyParser(HTMLParser):
	def handle_starttag(self,tag,attrs):
		print 'start (%s) attrs (%s)'%(tag,attrs)
		return

	def handle_endtag(self,tag):
		print 'end (%s)'%(tag)
		return

	def handle_data(self,data):
		print 'data (%s)'%(data)
		return



def main():
	if len(sys.argv) < 2:
		sys.stderr.write('%s htmlfile\n'%(sys.argv[0]))
		sys.exit(4)
	lines = ''
	with open(sys.argv[1]) as f:
		for l in f:
			lines += l
	parser = HTMLParser()
	#parser.feed(lines)
	parser.feed('<h1>Python</h1>')
	return

if __name__ == '__main__':
	main()



