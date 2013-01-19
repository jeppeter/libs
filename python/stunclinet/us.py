#! python

from socket import *
import sys
import select
port = 3312
if len(sys.argv) > 1:
	port = int(sys.argv[1])
address = ('0.0.0.0', port)
server_socket = socket(AF_INET, SOCK_DGRAM)
server_socket.bind(address)

while(1):
    print "Listening"
    recv_data, addr = server_socket.recvfrom(2048)
    print recv_data
    if recv_data == "Request 1" :
        print "Received request 1"
        server_socket.sendto("Response 1", addr)
    elif recv_data == "Request 2" :
        print "Received request 2"
        data = "Response 2"
        server_socket.sendto(data, addr)