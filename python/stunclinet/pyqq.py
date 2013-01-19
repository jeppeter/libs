#! python
#coding:utf-8

import httplib

class PyQQ:
	def __init__(self):
		pass

	def Request(self,method,url,data={})
		try:
			_url = httplib.urlsplit(url)
			_server,_port = _url.netloc.split(':')
			if _port is None:
				_port = 80
			_conn = httplib.HTTPConnection(_server,_port,True,3)
			_conn.connect()
			data = urllib.urlencode(data)
			if method == 'get':
				_conn.putrequest("GET",url,None)
				_conn.putheader("Content-Length",'0')
			elif method == 'post':
				_conn.putrequest("POST",url)
				_conn.putheader("Content-Length", str(len(data)))
				_conn.putheader("Content-Type", "application/x-www-form-urlencoded")
			_conn.putheader("Connection", "close")
			_conn.endheaders()

			if len(data) > 0:
				_conn.send(data)
			f = _conn.getresponse()
			self.httpBody = f.read().encode('gbk')
			f.close()
			_conn.close()
		except:
			self.httpBody = ''
		return self.httpBody

