# echo proxy 2
# TCP Client
# UDP Server
import os
import signal
import sys
import jsockets

sServer = jsockets.socket_tcp_connect(sys.argv[2], sys.argv[3])
if sServer is None:
    print('could not open socket for Server')
    sys.exit(1)

sProxy = jsockets.socket_udp_bind(sys.argv[1])
if sProxy is None:
    print('could not open socket for proxy1')
    sys.exit(1)

print('Cliente Conectado')
while True:
    data, addr = sProxy.recvfrom(1024)
    if not data:
        break
    sServer.send(data)
    data2 = sServer.recv(4096)
    sProxy.sendto(data2, addr)

sProxy.close()
