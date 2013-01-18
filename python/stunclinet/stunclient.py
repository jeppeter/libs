#! /usr/bin/env python

#############################################################################
#                                                                           #
#   File: stunclient.py                                                     #
#                                                                           #
#   Copyright (C) 2008 Du XiaoGang <dugang@188.com>                         #
#                                                                           #
#   This file is part of UDPonNAT.                                          #
#                                                                           #
#   UDPonNAT is free software: you can redistribute it and/or modify        #
#   it under the terms of the GNU General Public License as                 #
#   published by the Free Software Foundation, either version 3 of the      #
#   License, or (at your option) any later version.                         #
#                                                                           #
#   UDPonNAT is distributed in the hope that it will be useful,             #
#   but WITHOUT ANY WARRANTY; without even the implied warranty of          #
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           #
#   GNU General Public License for more details.                            #
#                                                                           #
#   You should have received a copy of the GNU General Public License       #
#   along with UDPonNAT.  If not, see <http://www.gnu.org/licenses/>.       #
#                                                                           #
#############################################################################

import socket, struct, time, re
import sys
class STUNError(Exception):
    pass

class ServerError(STUNError):
    def __init__(self, reason):
        self.reason = reason

    def __str__(self):
        return '<Server Error: %s>' % self.reason

class STUNClient:
    '''A STUN client implementated by Python.'''
    # constant
    # Message Types
    BindingRequest = 0x0001
    BindingResponse = 0x0101
    BindingErrorResponse = 0x0111
    # Message Attribute Types
    MAPPED_ADDRESS = 0x0001
    CHANGE_REQUEST = 0x0003
    CHANGED_ADDRESS = 0x0005
    ERROR_CODE = 0x0009
    # return values of test**
    RET_TEST_I1_UDP_BLOCKED = 0
    RET_TEST_I1_IP_SAME = 1
    RET_TEST_I1_IP_DIFF = 2
    RET_TEST_II_NO_RESP = 3
    RET_TEST_II_GOT_RESP = 4
    RET_TEST_I2_IP_SAME = 5
    RET_TEST_I2_IP_DIFF = 6
    RET_TEST_III_NO_RESP = 7
    RET_TEST_III_GOT_RESP = 8
    RET_TEST_IV_LOCAL = 9
    RET_TEST_IV_DIFF = 10
    # network types
    NET_TYPE_OPENED = 0
    NET_TYPE_FULLCONE_NAT = 1
    NET_TYPE_REST_NAT = 2
    NET_TYPE_PORTREST_NAT = 3
    NET_TYPE_SYM_UDP_FIREWALL = 4
    NET_TYPE_SYM_NAT_LOCAL = 5
    NET_TYPE_SYM_NAT = 6
    NET_TYPE_UDP_BLOCKED = 7

    def __init__(self):
        self.sock = None
        self.serverIP = ''
        self.serverPort = -1
        self.timeout = 3
        self.tid0 = 0x55555555
        self.tid1 = 0x5a5a5a5a
        self.tid2 = 0xaaaaaaaa
        self.tid3 = 0

        self.localIP = ''
        self.localPort = -1
        self.mappedIP = ''
        self.mappedPort = -1
        self.changedIP = ''
        self.changedPort = -1

    def setServerAddr(self, host, port=3478):
        # is host in ***.***.***.***?
        if re.match(r'^\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}$', host) == None:
            # hostname
            self.serverIP = socket.gethostbyname(host)
            self.serverPort = port
        else:
            self.serverIP = host
            self.serverPort = port

    def getServerAddr(self):
        return (self.serverIP, self.serverPort)

    def setTimeout(self, to):
        self.timeout = to 

    def getTimeout(self):
        return self.timeout

    def getNatType(self):
        '''
        o  On the open Internet
        o  Firewall that blocks UDP
        o  Firewall that allows UDP out, and responses have to come back to
           the source of the request (like a symmetric NAT, but no
           translation.  We call this a symmetric UDP Firewall)
        o  Full-cone NAT
        o  Symmetric NAT
        o  Restricted cone or restricted port cone NAT

        The flow makes use of three tests.  In test I, the client sends a
        STUN Binding Request to a server, without any flags set in the
        CHANGE-REQUEST attribute, and without the RESPONSE-ADDRESS attribute.
        This causes the server to send the response back to the address and
        port that the request came from.  In test II, the client sends a
        Binding Request with both the "change IP" and "change port" flags
        from the CHANGE-REQUEST attribute set.  In test III, the client sends
        a Binding Request with only the "change port" flag set.

        The client begins by initiating test I.  If this test yields no
        response, the client knows right away that it is not capable of UDP
        connectivity.  If the test produces a response, the client examines
        the MAPPED-ADDRESS attribute.  If this address and port are the same
        as the local IP address and port of the socket used to send the
        request, the client knows that it is not natted.  It executes test
        II.
        
        If a response is received, the client knows that it has open access
        to the Internet (or, at least, its behind a firewall that behaves
        like a full-cone NAT, but without the translation).  If no response
        is received, the client knows its behind a symmetric UDP firewall.
        
        In the event that the IP address and port of the socket did not match
        the MAPPED-ADDRESS attribute in the response to test I, the client
        knows that it is behind a NAT.  It performs test II.  If a response
        is received, the client knows that it is behind a full-cone NAT.  If
        no response is received, it performs test I again, but this time,
        does so to the address and port from the CHANGED-ADDRESS attribute
        from the response to test I.  If the IP address and port returned in
        the MAPPED-ADDRESS attribute are not the same as the ones from the
        first test I, the client knows its behind a symmetric NAT.  If the
        address and port are the same, the client is either behind a
        restricted or port restricted NAT.  To make a determination about
        which one it is behind, the client initiates test III.  If a response
        is received, its behind a restricted NAT, and if no response is
        received, its behind a port restricted NAT.

                             +--------+
                             |  Test  |
                             |   I    |
                             +--------+
                                  |
                                  |
                                  V
                                 /\              /\
                              N /  \ Y          /  \ Y             +--------+
               UDP     <-------/Resp\--------->/ IP \------------->|  Test  |
               Blocked         \ ?  /          \Same/              |   II   |
                                \  /            \? /               +--------+
                                 \/              \/                    |
                                                  | N                  |
                                                  |                    V
                                                  V                    /\
                                              +--------+  Sym.      N /  \
                                              |  Test  |  UDP    <---/Resp\
                                              |   II   |  Firewall   \ ?  /
                                              +--------+              \  /
                                                  |                    \/
                                                  V                     |Y
                       /\                         /\                    |
        Symmetric  N  /  \       +--------+   N  /  \                   V
           NAT  <--- / IP \<-----|  Test  |<--- /Resp\               Open
           |         \Same/      |   I    |     \ ?  /               Internet
           |          \? /       +--------+      \  /
           V           \/                         \/
        +--------+     |                           |Y
        | TestIV |     |                           |
        +--------+     |                           V
           |           |                           Full
           |           |                           Cone
           V           V              /\
           /\      +--------+        /  \ Y
          /  \     |  Test  |------>/Resp\---->Restricted
         /local--+ |   III  |       \ ?  /
         \ ?  /  | +--------+        \  /
          \  /   V                    \/
           \/ Local Sym NAT            |N
           |                           |       Port
           +--->Symmetric NAT          +------>Restricted
        '''
        ret = self.testI1()
        if ret == self.RET_TEST_I1_UDP_BLOCKED:
            return self.NET_TYPE_UDP_BLOCKED
        elif ret == self.RET_TEST_I1_IP_SAME:
            ret = self.testII()
            if ret == self.RET_TEST_II_GOT_RESP:
                return self.NET_TYPE_OPENED
            return self.NET_TYPE_SYM_UDP_FIREWALL
        else:
            ret = self.testII()
            if ret == self.RET_TEST_II_GOT_RESP:
                return self.NET_TYPE_FULLCONE_NAT
            ret = self.testI2()
            if ret == self.RET_TEST_I2_IP_DIFF:
                ret = self.testIV()
                if ret == self.RET_TEST_IV_LOCAL:
                    return self.NET_TYPE_SYM_NAT_LOCAL
                return self.NET_TYPE_SYM_NAT
            ret = self.testIII()
            if ret == self.RET_TEST_III_GOT_RESP:
                return self.NET_TYPE_REST_NAT
            return self.NET_TYPE_PORTREST_NAT

    def getMappedAddr(self):
        mappedIP = ''

        # make request packet
        req = struct.pack('!HHIIII', self.BindingRequest, 0, 
                          self.tid0, self.tid1, self.tid2, self.tid3)
        # send/recv
        self.sock.sendto(req, (self.serverIP, self.serverPort))
        st = time.time()
        while time.time() - st < self.timeout:
            try:
                resp, fro = self.sock.recvfrom(4096)
                if fro != (self.serverIP, self.serverPort):
                    continue
                # check resp
                if len(resp) < 20:
                    continue
                mh = resp[0: 20]
                t, l, tid0, tid1, tid2, tid3 = struct.unpack('!HHIIII', mh)
                if t != self.BindingResponse:
                    continue
                if l != len(resp) - 20:
                    continue
                if tid0 != self.tid0 or tid1 != self.tid1 \
                   or tid2 != self.tid2 or tid3 != self.tid3:
                    continue
                break
            except socket.timeout:
                continue
        else:
            raise ServerError, 'Couldn\'t get server\'s response.'
        self.tid3 += 1

        # got response
        restLen = len(resp) - 20
        respIdx = 20
        # for Message Attributes
        while True:
            if restLen < 4:
                break
            mah = resp[respIdx: respIdx + 4]
            restLen -= 4
            respIdx += 4
            t, l = struct.unpack('!HH', mah)
            if l > restLen:
                raise ServerError, 'Invalid server\'s response.'
            v = resp[respIdx: respIdx + l]
            restLen -= l
            respIdx += l
            if t == self.MAPPED_ADDRESS:
                if l != 8:
                    raise ServerError, 'Invalid server\'s response.'
                _, f, p, s1, s2, s3, s4 = struct.unpack('!BBHBBBB', v)
                if f != 1:
                    raise ServerError, 'Invalid server\'s response.'
                mappedIP = '%d.%d.%d.%d' % (s1, s2, s3, s4)
                mappedPort = p
        if mappedIP == '':
            raise ServerError, 'Invalid server\'s response.'
        return (mappedIP, mappedPort)

    def testI1(self):
        # make request packet
        req = struct.pack('!HHIIII', self.BindingRequest, 0, 
                          self.tid0, self.tid1, self.tid2, self.tid3)
        # send/recv
        self.sock.sendto(req, (self.serverIP, self.serverPort))
        st = time.time()
        while time.time() - st < self.timeout:
            try:
                resp, fro = self.sock.recvfrom(4096)
                if fro != (self.serverIP, self.serverPort):
                    continue
                # check resp
                if len(resp) < 20:
                    continue
                mh = resp[0: 20]
                t, l, tid0, tid1, tid2, tid3 = struct.unpack('!HHIIII', mh)
                if t != self.BindingResponse:
                    continue
                if l != len(resp) - 20:
                    continue
                if tid0 != self.tid0 or tid1 != self.tid1 \
                   or tid2 != self.tid2 or tid3 != self.tid3:
                    continue
                break
            except socket.timeout:
                continue
        else:
            return self.RET_TEST_I1_UDP_BLOCKED
        self.tid3 += 1

        # got response
        restLen = len(resp) - 20
        respIdx = 20
        # for Message Attributes
        while True:
            if restLen < 4:
                break
            mah = resp[respIdx: respIdx + 4]
            restLen -= 4
            respIdx += 4
            t, l = struct.unpack('!HH', mah)
            if l > restLen:
                raise ServerError, 'Invalid server\'s response.'
            v = resp[respIdx: respIdx + l]
            restLen -= l
            respIdx += l
            if t == self.MAPPED_ADDRESS:
                if l != 8:
                    raise ServerError, 'Invalid server\'s response.'
                _, f, p, s1, s2, s3, s4 = struct.unpack('!BBHBBBB', v)
                if f != 1:
                    raise ServerError, 'Invalid server\'s response.'
                self.mappedIP = '%d.%d.%d.%d' % (s1, s2, s3, s4)
                self.mappedPort = p
            elif t == self.CHANGED_ADDRESS:
                if l != 8:
                    raise ServerError, 'Invalid server\'s response.'
                _, f, p, s1, s2, s3, s4 = struct.unpack('!BBHBBBB', v)
                if f != 1:
                    raise ServerError, 'Invalid server\'s response.'
                self.changedIP = '%d.%d.%d.%d' % (s1, s2, s3, s4)
                self.changedPort = p
        if self.mappedIP == '' or self.changedIP == '':
            raise ServerError, 'Invalid server\'s response.'

        # get local addr/port
        self.getLocalIPPort()
        if self.localIP == self.mappedIP and self.localPort == self.mappedPort:
            return self.RET_TEST_I1_IP_SAME
        else:
            return self.RET_TEST_I1_IP_DIFF

    def getLocalIPPort(self):
        # there are some bugs, but i can't find better way.
        s = socket.socket()
        s.connect(('www.google.com', 80))
        self.localIP, _ = s.getsockname()
        s.close()
        _, self.localPort = self.sock.getsockname()

    def testII(self):
        # make request packet
        req = struct.pack('!HHIIIIHHBBBB', self.BindingRequest, 8, 
                          self.tid0, self.tid1, self.tid2, self.tid3, 
                          self.CHANGE_REQUEST, 4, 0, 0, 0, 6)
        # send/recv
        self.sock.sendto(req, (self.serverIP, self.serverPort))
        st = time.time()
        while time.time() - st < self.timeout:
            try:
                resp, fro = self.sock.recvfrom(4096)
                if fro != (self.changedIP, self.changedPort):
                    continue
                # check resp
                if len(resp) < 20:
                    continue
                mh = resp[0: 20]
                t, l, tid0, tid1, tid2, tid3 = struct.unpack('!HHIIII', mh)
                if t != self.BindingResponse:
                    continue
                if l != len(resp) - 20:
                    continue
                if tid0 != self.tid0 or tid1 != self.tid1 \
                   or tid2 != self.tid2 or tid3 != self.tid3:
                    continue
                break
            except socket.timeout:
                continue
        else:
            return self.RET_TEST_II_NO_RESP
        self.tid3 += 1
        return self.RET_TEST_II_GOT_RESP

    def testI2(self):
        mappedIP = ''

        # make request packet
        req = struct.pack('!HHIIII', self.BindingRequest, 0, 
                          self.tid0, self.tid1, self.tid2, self.tid3)
        # send/recv
        self.sock.sendto(req, (self.changedIP, self.changedPort))
        st = time.time()
        while time.time() - st < self.timeout:
            try:
                resp, fro = self.sock.recvfrom(4096)
                if fro != (self.changedIP, self.changedPort):
                    continue
                # check resp
                if len(resp) < 20:
                    continue
                mh = resp[0: 20]
                t, l, tid0, tid1, tid2, tid3 = struct.unpack('!HHIIII', mh)
                if t != self.BindingResponse:
                    continue
                if l != len(resp) - 20:
                    continue
                if tid0 != self.tid0 or tid1 != self.tid1 \
                   or tid2 != self.tid2 or tid3 != self.tid3:
                    continue
                break
            except socket.timeout:
                continue
        else:
            raise ServerError, 'Couldn\'t get server\'s response.'
        self.tid3 += 1

        # got response
        restLen = len(resp) - 20
        respIdx = 20
        # for Message Attributes
        while True:
            if restLen < 4:
                break
            mah = resp[respIdx: respIdx + 4]
            restLen -= 4
            respIdx += 4
            t, l = struct.unpack('!HH', mah)
            if l > restLen:
                raise ServerError, 'Invalid server\'s response.'
            v = resp[respIdx: respIdx + l]
            restLen -= l
            respIdx += l
            if t == self.MAPPED_ADDRESS:
                if l != 8:
                    raise ServerError, 'Invalid server\'s response.'
                _, f, p, s1, s2, s3, s4 = struct.unpack('!BBHBBBB', v)
                if f != 1:
                    raise ServerError, 'Invalid server\'s response.'
                mappedIP = '%d.%d.%d.%d' % (s1, s2, s3, s4)
                mappedPort = p
        if mappedIP == '':
            raise ServerError, 'Invalid server\'s response.'
        if mappedIP == self.mappedIP and mappedPort == self.mappedPort:
            return self.RET_TEST_I2_IP_SAME
        else:
            return self.RET_TEST_I2_IP_DIFF

    def testIII(self):
        # make request packet
        req = struct.pack('!HHIIIIHHBBBB', self.BindingRequest, 8, 
                          self.tid0, self.tid1, self.tid2, self.tid3, 
                          self.CHANGE_REQUEST, 4, 0, 0, 0, 2)
        # send/recv
        self.sock.sendto(req, (self.serverIP, self.serverPort))
        st = time.time()
        while time.time() - st < self.timeout:
            try:
                resp, fro = self.sock.recvfrom(4096)
                if fro != (self.serverIP, self.changedPort):
                    continue
                # check resp
                if len(resp) < 20:
                    continue
                mh = resp[0: 20]
                t, l, tid0, tid1, tid2, tid3 = struct.unpack('!HHIIII', mh)
                if t != self.BindingResponse:
                    continue
                if l != len(resp) - 20:
                    continue
                if tid0 != self.tid0 or tid1 != self.tid1 \
                   or tid2 != self.tid2 or tid3 != self.tid3:
                    continue
                break
            except socket.timeout:
                continue
        else:
            return self.RET_TEST_III_NO_RESP
        self.tid3 += 1
        return self.RET_TEST_III_GOT_RESP

    def testIV(self):
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        sock.settimeout(self.timeout)

        # make request packet
        req = struct.pack('!HHIIII', self.BindingRequest, 0, 
                          self.tid0, self.tid1, self.tid2, self.tid3)
        # send/recv
        resp1 = ''
        resp2 = ''
        self.sock.sendto(req, (self.serverIP, self.serverPort))
        self.sock.sendto(req, (self.changedIP, self.changedPort))
        st = time.time()
        while time.time() - st < self.timeout:
            try:
                resp, fro = self.sock.recvfrom(4096)
                # check resp
                if len(resp) < 20:
                    continue
                mh = resp[0: 20]
                t, l, tid0, tid1, tid2, tid3 = struct.unpack('!HHIIII', mh)
                if t != self.BindingResponse:
                    continue
                if l != len(resp) - 20:
                    continue
                if tid0 != self.tid0 or tid1 != self.tid1 \
                   or tid2 != self.tid2 or tid3 != self.tid3:
                    continue
                # check fro
                if fro == (self.serverIP, self.serverPort):
                    resp1 = resp
                elif fro == (self.changedIP, self.changedPort):
                    resp2 = resp
                if resp1 != '' and resp2 != '':
                    break
            except socket.timeout:
                continue
        else:
            raise ServerError, 'Couldn\'t get server\'s response.'
        self.tid3 += 1
        sock.close()

        mappedIP1 = ''
        mappedIP2 = ''

        # for response from serverIP
        restLen = len(resp1) - 20
        respIdx = 20
        # for Message Attributes
        while True:
            if restLen < 4:
                break
            mah = resp1[respIdx: respIdx + 4]
            restLen -= 4
            respIdx += 4
            t, l = struct.unpack('!HH', mah)
            if l > restLen:
                raise ServerError, 'Invalid server\'s response.'
            v = resp1[respIdx: respIdx + l]
            restLen -= l
            respIdx += l
            if t == self.MAPPED_ADDRESS:
                if l != 8:
                    raise ServerError, 'Invalid server\'s response.'
                _, f, p, s1, s2, s3, s4 = struct.unpack('!BBHBBBB', v)
                if f != 1:
                    raise ServerError, 'Invalid server\'s response.'
                mappedIP1 = '%d.%d.%d.%d' % (s1, s2, s3, s4)
                mappedPort1 = p
        if mappedIP1 == '':
            raise ServerError, 'Invalid server\'s response.'

        # for response from serverIP
        restLen = len(resp2) - 20
        respIdx = 20
        # for Message Attributes
        while True:
            if restLen < 4:
                break
            mah = resp2[respIdx: respIdx + 4]
            restLen -= 4
            respIdx += 4
            t, l = struct.unpack('!HH', mah)
            if l > restLen:
                raise ServerError, 'Invalid server\'s response.'
            v = resp2[respIdx: respIdx + l]
            restLen -= l
            respIdx += l
            if t == self.MAPPED_ADDRESS:
                if l != 8:
                    raise ServerError, 'Invalid server\'s response.'
                _, f, p, s1, s2, s3, s4 = struct.unpack('!BBHBBBB', v)
                if f != 1:
                    raise ServerError, 'Invalid server\'s response.'
                mappedIP2 = '%d.%d.%d.%d' % (s1, s2, s3, s4)
                mappedPort2 = p
        if mappedIP2 == '':
            raise ServerError, 'Invalid server\'s response.'

        if mappedIP1 == mappedIP2 \
           and mappedPort1 in range(mappedPort2 - 10, mappedPort2 + 10):
            return self.RET_TEST_IV_LOCAL
        return self.RET_TEST_IV_DIFF

    def createSocket(self,port=0):
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock.settimeout(self.timeout)
        if port != 0:
        	self.sock.bind(('',port))

    def close(self):
        self.sock.close()

    def natType2String(self, t):
        if t == self.NET_TYPE_OPENED:
            return 'Opened(%d)' % self.NET_TYPE_OPENED
        elif t == self.NET_TYPE_FULLCONE_NAT:
            return 'Full Cone NAT(%d)' % self.NET_TYPE_FULLCONE_NAT
        elif t == self.NET_TYPE_REST_NAT:
            return 'Restricted NAT(%d)' % self.NET_TYPE_REST_NAT
        elif t == self.NET_TYPE_PORTREST_NAT:
            return 'Port Restricted NAT(%d)' % self.NET_TYPE_PORTREST_NAT
        elif t == self.NET_TYPE_SYM_UDP_FIREWALL:
            return 'Symmetric UDP Firewall(%d)' % self.NET_TYPE_SYM_UDP_FIREWALL
        elif t == self.NET_TYPE_SYM_NAT_LOCAL:
            return 'Symmetric NAT with localization(%d)' % self.NET_TYPE_SYM_NAT_LOCAL
        elif t == self.NET_TYPE_SYM_NAT:
            return 'Symmetric NAT(%d)' % self.NET_TYPE_SYM_NAT
        elif t == self.NET_TYPE_UDP_BLOCKED:
            return 'Blocked(%d)' % self.NET_TYPE_UDP_BLOCKED


if __name__ == '__main__':
    sc = STUNClient()
    sc.setServerAddr('stun.ekiga.net')
    #sc.setServerAddr('stun.l.google.com', 19302)
    port = 0
    if len(sys.argv) > 1 :
        port = int(sys.argv[1])
    sc.createSocket(port)
    print 'NAT TYPE:', sc.natType2String(sc.getNatType())
    print 'MAPPED ADDRESS:', sc.getMappedAddr()
    sc.close()
