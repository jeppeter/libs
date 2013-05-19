#! python

import logging
import sys
from xml.etree import ElementTree as ET
import cgi

from xunit.utils import exception
import cgi
import re

class X2HHandlerError(exception.XUnitException):
	pass

class X2HFileError(exception.XUnitException):
	pass

class X2HTagError(exception.XUnitException):
	pass

class Xml2Html(object):
	def __init__(self,fname=None):
		self.__fname =fname
		self.__fout = None
		self.__tree = None
		self.__root = None
		return

	def __del__(self):
		self.__fname = None
		self.__fout = None
		if self.__tree :
			del self.__tree
		self.__tree = None
		self.__root = None
		return

	def __MustFindTag(self,fromtag,tag):
		elem = fromtag.findall(tag)
		if len(elem) == 0:
			raise X2HTagError('Must in (%s) Find (%s)'%(fromtag.tag,tag))
		return elem

	def __DumpHeader(self):
		self.__fout.write('<html>\n<body>\n')
		st = self.__MustFindTag(self.__root,'starttime')
		et = self.__MustFindTag(self.__root,'endtime')
		cf = self.__MustFindTag(self.__root,'configfile')
		ts = self.__MustFindTag(self.__root,'testsubject')
		self.__fout.write('<p1 ><center><font size="10">%s Config(%s)</font></center></p1><br/>\n'%(ts[0].text,cf[0].text))
		self.__fout.write('<p align="center"><font size="5">At  %s-%s</font></p><br/>\n'%(st[0].text,et[0].text))
		return

	def __DumpCase(self,elem):
		cn = self.__MustFindTag(elem,'func')
		st = self.__MustFindTag(elem,'starttime')
		et = self.__MustFindTag(elem,'endtime')
		res = self.__MustFindTag(elem,'result')
		r = self.__MustFindTag(res[0],'indication')
		rmsg =self.__MustFindTag(res[0],'msg')
		color = "white"
		if r[0].text != 'succ':
			color = "red"
		lkname = '#section-%s'%(cn[0].text.replace(':','_').replace('.','_'))
		self.__fout.write('<tr bgcolor="%s">\n'%(color))
		self.__fout.write('<td align="center">%s</td>\n'%(cn[0].text))
		self.__fout.write('<td align="center">%s-%s</td>\n'%(st[0].text,et[0].text))
		self.__fout.write('<td align="center">%s</td>\n'%(rmsg[0].text))
		self.__fout.write('<td align="center"><a href="%s">More Info</a></td>\n'%(lkname))
		self.__fout.write('</tr>\n')
		return

	def __DumpCaseHeader(self):
		self.__fout.write('<tr align="center" bgcolor="greee">\n')
		self.__fout.write('<th><font color="yellow">Case</font></th>\n')
		self.__fout.write('<th><font color="yellow">Time</font></th>\n')
		self.__fout.write('<th><font color="yellow">Result</font></th>\n')
		self.__fout.write('<th><font color="yellow">Link</font></th>\n')
		self.__fout.write('</tr>\n')
		return

	def __DumpCases(self):
		elm  = self.__root.findall('xunitcase')
		tc = len(elm)
		sc = 0
		fc = 0
		skipc = 0
		ec = 0
		for e in elm:
			r = self.__MustFindTag(e,'result')
			tg = self.__MustFindTag(r[0],'indication')
			if tg[0].text == 'succ':
				sc += 1
			elif tg[0].text == 'fail':
				fc += 1
			elif tg[0].text == 'error':
				ec += 1
			elif tg[0].text == 'skip':
				skipc += 1
		self.__fout.write('<p2><center><font size="6">Success %d Fail %d Error %d Skip %d</font></center></p2>\n'%(sc,fc,ec,skipc))
		self.__fout.write('<table align="center" border="1">\n')
		self.__DumpCaseHeader()
		self.__fout.write('<p>\n')
		for e in elm:
			self.__DumpCase(e)
		self.__fout.write('</p>\n')
		self.__fout.write('</table>\n')
		return

	def __DumpDebug(self,casename):
		lkname = 'section-%s'%(casename.replace(':','_').replace('.','_'))
		self.__fout.write('<tr>\n')
		self.__fout.write('<td align="center"><a name="%s">%s</a></td>\n'%(lkname,casename))
		# now to find all the message from one case
		infostr = ''
		caseelm =self.__root.findall('xunitcase')
		for ce in caseelm:
			# now we find the case debug message
			delms = ce.findall('debugmsg')
			for de in delms:
				fn = self.__MustFindTag(de,'func')
				if fn[0].text == casename:
					emsg = self.__MustFindTag(de,'msg')
					for e in emsg:
						infostr += e.text

		delm = self.__root.findall('debugmsg')
		for de in delm:
			fn = self.__MustFindTag(de,'func')
			if fn[0].text == casename:
				emsg = self.__MustFindTag(de,'msg')
				for e in emsg:
					infostr += e.text
		if len(infostr) > 0:
			s = cgi.escape(infostr).encode('ascii', 'xmlcharrefreplace')
			s = re.sub('[\n]+','\n',s)
			s = s.replace('\n','<br/>')
			# to make the replace the string
			self.__fout.write('<td>%s</td>\n'%(s))
		else:
			self.__fout.write('<td>None</td>\n')
		self.__fout.write('</tr>\n')
		return
	def __DumpDebugHeader(self):
		self.__fout.write('<tr align="center" bgcolor="greee">\n')
		self.__fout.write('<th><font color="yellow">Tag</font></th>\n')
		self.__fout.write('<th><font color="yellow">Info</font></th>\n')
		self.__fout.write('</tr>\n')
		return
	def __DumpDebugInfo(self):
		elm  = self.__root.findall('xunitcase')
		self.__fout.write('<p2><center><font size="6">More Information</font></center></p2>\n')
		self.__fout.write('<table align="center" border="1">\n')
		self.__fout.write('<p>\n')
		self.__DumpDebugHeader()

		for e in elm:
			fn = self.__MustFindTag(e,'func')
			self.__DumpDebug(fn[0].text)
			
		
		self.__fout.write('</p>\n')
		self.__fout.write('</table>\n')
		return

	def __DumpEndHeader(self):
		self.__fout.write('</body>\n')
		self.__fout.write('</html>\n')
		return
	def __Dump(self):
		try:
			self.__tree = ET.parse(self.__fname)
			self.__root = self.__tree.getroot()
			if self.__root.tag != 'xunittest':
				raise X2HTagError('root tag (%s) not valid'%(self.__root.tag))
			self.__DumpHeader()
			self.__DumpCases()
			self.__DumpDebugInfo()
			self.__DumpEndHeader()
		except:
			raise  X2HFileError('dump(%s) error'%(self.__fname))
		return

	def Dump(self,fname=None,fout=sys.stdout):
		if fname is not None:
			self.__fname = fname
		if fout is None:
			raise X2HHandlerError('fout is None')
		self.__fout = fout
		self.__Dump()
		return 
		
