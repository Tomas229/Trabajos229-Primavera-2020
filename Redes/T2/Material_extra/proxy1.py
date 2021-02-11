#!/usr/bin/python3
# proxy
# Usando procesos para multi-clientes
import sys
import jsockets
import threading

TIME_OUT = 0.3


PAYLOAD_LENGTH = 1500
HEADER_LENGTH = 2
SEQ = 0
EXPECTED_SEQ = 0
ACKED = False
LOSS = 2  # is this?
mutex = threading.Condition()

# client-TCP -> proxy -> UDP-server


def TCP_Rdr(conn1, conn2):
    global SEQ, EXPECTED_SEQ, ACKED
    while True:
        try:
            data = conn1.recv(PAYLOAD_LENGTH-HEADER_LENGTH)
        except:
            data = None
        if not data:
            break

        with mutex:
            data = "D"+str(SEQ)+data.decode()
            ACKED = False

            while not ACKED:
                conn2.send(data.encode())
                mutex.wait(TIME_OUT)
            SEQ = (SEQ + 1) % 2


# client-TCP <- proxy <- UDP-server


def UDP_Rdr(conn1, conn2):
    global SEQ, EXPECTED_SEQ, ACKED
    while True:
        try:
            data = conn1.recv(PAYLOAD_LENGTH)
        except:
            data = None
        if not data:
            break

        data = data.decode()
        if data[0] == "D":
            seq_num = int(data[1])
            conn1.send(("A"+str(seq_num)).encode())
            if seq_num == EXPECTED_SEQ:
                EXPECTED_SEQ = (EXPECTED_SEQ + 1) % 2
                data = data[2:]
                conn2.send(data.encode())

            else:
                pass

        elif data[0] == "A":
            with mutex:
                seq_num = int(data[1])
                if seq_num == SEQ:
                    ACKED = True
                    mutex.notify_all()
                else:
                    pass


def proxy(conn1, conn2):
    print("proxy started")
    while True:
        conn2.send("CS".encode())
        ack = conn2.recv(PAYLOAD_LENGTH)
        if ack.decode() == "A0":
            break
    print("CS sent")
    while True:
        conn2.send(("L"+str(LOSS)).encode())
        ack = conn2.recv(PAYLOAD_LENGTH)
        if ack.decode() == "A1":
            break
    print("LOSS sent")
    print("Handshake ready")
    aux(conn1, conn2)


def aux(conn1, conn2):
    newthread1 = threading.Thread(
        target=UDP_Rdr, daemon=True, args=(conn2, conn1))
    newthread1.start()
    TCP_Rdr(conn1, conn2)

# Main--------------------


if len(sys.argv) != 4:
    print('Use: '+sys.argv[0]+' port-in host port-out')
    sys.exit(1)

portin = sys.argv[1]
host = sys.argv[2]
portout = sys.argv[3]


sock_tcp = jsockets.socket_tcp_bind(portin)
if sock_tcp is None:
    print('bind falló')
    sys.exit(1)
print("tcp bind exitoso")

sock_udp = jsockets.socket_udp_connect(host, portout)
if sock_udp is None:
    print('could not open socket to proxy2')
    sys.exit(1)
print("Conectado a proxy 2")

while True:
    conn, addr = sock_tcp.accept()
    while True:
        proxy(conn, sock_udp)
        break
    conn.close()

# py client_echo3.py localhost 1818
