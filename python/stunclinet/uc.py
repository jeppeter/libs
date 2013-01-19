#! python
from socket import *
import sys
import select
ip='127.0.0.1'
port=3312
if len(sys.argv) > 2:
	ip = sys.argv[1]
	port = int(sys.argv[2])
address = (ip,port)
client_socket = socket(AF_INET, SOCK_DGRAM)

num_retransmits = 0
while(num_retransmits < 60):
    num_retransmits = num_retransmits + 1
    data = "Request 1"
    client_socket.sendto(data, address)
    print "Sending request 1"
    recv_data, addr = client_socket.recvfrom(2048)
    print recv_data, "!!"
    data = "Request 2"
    client_socket.sendto(data, address)
    print "Sending request 2"
    recv_data, addr = client_socket.recvfrom(2048)
    print recv_data, "!!"