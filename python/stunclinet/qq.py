#coding:utf-8
#基于python2.6版本开发
import httplib,urllib,os,threading,re
import sys 
import logging
reload(sys) 
sys.setdefaultencoding('utf8') 
class PYQQ:
    def __init__(self):
        self.reqIndex = 0
    
    #HTTP请求
    def httpRequest(self,method,url,data={}):
        try:
            _urld = httplib.urlsplit(url)
            conn = httplib.HTTPConnection(_urld.netloc,80,True,3)
            conn.connect()
            logging.info("server %s port 80\n"%(_urld.netloc))
            data = urllib.urlencode(data)
            logging.info("data %s\n"%(str(data)))
            if method=='get':
            	logging.info("url %s\n"%(str(url)))
                conn.putrequest("GET", url, None)
                logging.info("\n")
                conn.putheader("Content-Length",'0')
                logging.info("\n")
            elif method=='post':
            	logging.info("\n")
                conn.putrequest("POST", url)
                logging.info("url %s\n"%(str(url)))
                conn.putheader("Content-Length", str(len(data)))
                conn.putheader("Content-Type", "application/x-www-form-urlencoded")

            logging.info("\n")
            conn.putheader("Connection", "close")
            conn.endheaders()
            logging.info("\n")
            if len(data)>0:
            	logging.info("len %d\n"%(len(data)))
                conn.send(data)
            logging.info("\n")
            f = conn.getresponse()
            logging.info("\n")
            self.httpBody = f.read().encode('gbk')
            logging.info("\n")
            f.close()
            conn.close()
        except:
            self.httpBody=''
        return self.httpBody
    #HTTP请求的pycurl版本，和上面的程序选一即可
    def httpRequest_(self,method,url,data={}):
        import pycurl,StringIO
        c = pycurl.Curl()
        c.setopt(pycurl.URL,url)
        if method=='post':
            import urllib
            c.setopt(c.POSTFIELDS, urllib.urlencode(data))
        
        c.fp = StringIO.StringIO()
        c.setopt(pycurl.WRITEFUNCTION,c.fp.write)
        c.perform()
        self.httpBody = c.fp.getvalue().encode('gbk')
        del c.fp
        c.close()
        c = None
        return self.httpBody
    #通过首尾获取字符串的内容
    def getCon(self,start,end):
        findex = self.httpBody.find(start)
        if findex == -1 : return None
        tmp = self.httpBody.split(start)
        
        eindex = tmp[1].find(end)
        if eindex == -1:
            return tmp[1][0:]
        else:
            return tmp[1][0:eindex]
    #获取postfield的值
    def getField(self,fd):
        KeyStart = '<postfield name="'+ str(fd) +'" value="'
        return self.getCon(KeyStart,'"/>')
    #获取登陆验证码,并保存至当前目录的qqcode.gif
    def getSafecode(self):
        url = self.getCon('<img src="','"')
        import urllib2
        pager = urllib2.urlopen(url)
        data=pager.read()
        file=open(os.getcwd()+'\qqcode.gif','w+b')
        file.write(data)
        file.close()
        return True
    #登陆QQ
    def login(self):
        self.qq = raw_input('请输入QQ号:')
        self.pwd = raw_input('请输入密码:')
        s1Back = self.httpRequest('post','http://pt.3g.qq.com/handleLogin',{'r':'324525157','qq':self.qq,'pwd':self.pwd,'toQQchat':'true','q_from':'','modifySKey':0,'loginType':1})
        if s1Back.find('请输入验证码')!=-1:
            self.sid = self.getField('sid')
            self.hexpwd = self.getField('hexpwd')
            self.extend = self.getField('extend')
            self.r_sid = self.getField('r_sid')
            self.rip = self.getField('rip')
            if self.getSafecode():
                self.safeCode = raw_input('请输入验证码（本文件同目录的qqcode.gif）:')
            else:
                print '验证码加载错误'
            
            postData = {'sid':self.sid,'qq':self.qq,'hexpwd':self.hexpwd,'hexp':'true','auto':'0',
                        'logintitle':'手机腾讯网','q_from':'','modifySKey':'0','q_status':'10',
                        'r':'271','loginType':'1','prev_url':'10','extend':self.extend,'r_sid':self.r_sid,
                        'bid_code':'','bid':'-1','toQQchat':'true','rip':self.rip,'verify':self.safeCode,
            }
            s1Back = self.httpRequest('post','http://pt.3g.qq.com/handleLogin',postData)
        
        self.sid = self.getCon('sid=','&')
        print '登陆成功'
        self.getMsgFun()    
    #定时获取消息
    def getMsgFun(self):
        self.reqIndex = self.reqIndex + 1
        s2Back = self.httpRequest('get','http://q32.3g.qq.com/g/s?aid=nqqchatMain&sid='+self.sid)
        if s2Back.find('alt="聊天"/>(')!=-1:
            #有新消息，请求获取消息页面
            s3back = self.httpRequest('get','http://q32.3g.qq.com/g/s?sid='+ self.sid + '&aid=nqqChat&saveURL=0&r=1310115753&g_f=1653&on=1')
            
            #消息发起者的昵称
            if s3back.find('title="临时会话')!=-1:
                _fromName = '临时对话'
            else:
                _fromName = self.getCon('title="与','聊天')
            
            #消息发起者的QQ号
            _fromQQ = self.getCon('num" value="','"/>') 
            
            #消息内容
            _msg_tmp = self.getCon('saveURL=0">提示</a>)','<input name="msg"')
            crlf = '\n'
            if _msg_tmp.find('\r\n')!=-1: crlf = '\r\n'
            _msg = re.findall(r'(.+)<br/>'+ crlf +'(.+)<br/>',_msg_tmp)
            
            for _data in _msg:
                self.getMsg({'qq':_fromQQ,'nick':_fromName,'time':_data[0],'msg':str(_data[1]).strip()})
        
        if self.reqIndex>=30:
            #保持在线
            _url = 'http://pt5.3g.qq.com/s?aid=nLogin3gqqbysid&3gqqsid='+self.sid
            self.httpRequest('get',_url)
            self.reqIndex = 0
        t = threading.Timer(2.0,self.getMsgFun)
        t.start()    
    #发送消息
    #qq 目标QQ
    #msg 发送内容
    def sendMsgFun(self,qq,msg):
        msg = unicode(msg,'gbk').encode('utf8')
        postData = {'sid':self.sid,'on':'1','saveURL':'0','saveURL':'0','u':qq,'msg':str(msg),}
        s1Back = self.httpRequest('post','http://q16.3g.qq.com/g/s?sid='+ self.sid +'&aid=sendmsg&tfor=qq',postData)
        print '发送消息给',qq,'成功'    
    #收到消息的接口，重载或重写该方法
    def getMsg(self,data):
        print data['time'],"收到",data['nick'],"(",data['qq'],")的新消息"
        self.sendMsgFun(data['qq'],data['nick']+'，我收到了你的消息：'+ data['msg'])
logging.basicConfig(level=logging.INFO,format="%(levelname)-8s %(asctime)-12s [%(filename)-10s:%(funcName)-20s:%(lineno)-5s] %(message)s")
QQ = PYQQ()
QQ.login()
