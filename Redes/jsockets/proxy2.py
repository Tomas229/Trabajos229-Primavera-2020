#!/usr/bin/python3
# Echo server program - version of server_echo4_n.c
# Usando threads para multi-clientes
import os
import signal
import sys
import threading
import jsockets

sServer = jsockets.socket_tcp_connect(sys.argv[2], sys.argv[3])
if sServer is None:
    print('could not open socket for Server')
    sys.exit(1)

sProxy = jsockets.socket_udp_bind(sys.argv[1])
if sProxy is None:
    print('could not open socket for proxy1')
    sys.exit(1)
while True:
    data, addr = sProxy.recvfrom(1024)
    if not data:
        break
    sServer.send(data)

sProxy.close()
