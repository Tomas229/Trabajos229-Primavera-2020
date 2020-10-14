#!/usr/bin/python3
# Echo server program - version of server_echo4_n.c
# Usando threads para multi-clientes
import os
import signal
import sys
import threading
import jsockets

sProxy = jsockets.socket_udp_connect(sys.argv[2], sys.argv[3])
if sProxy is None:
    print('could not open socket for proxy2')
    sys.exit(1)


sCliente = jsockets.socket_tcp_bind(sys.argv[1])
if sCliente is None:
    print('could not open socket for client')
    sys.exit(1)
while True:
    conn, addr = sCliente.accept()
    print('Connected by', addr)
    while True:
        data = conn.recv(1024)
        if not data:
            break
        sProxy.send(data)
    conn.close()
    print('Client disconnected')
