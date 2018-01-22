#-*- coding:utf-8 -*-

import socket
import SocketServer
import threading

import time

from sqlalchemy import create_engine
from sqlalchemy.orm import sessionmaker
from up_link import UpLink
from down_link import DownLink
from models import Base

HOST = ''
PORT = 18100
ADDR = (HOST,PORT)
sub_threads = []
client_con = dict()
total_data=[]
BUFSIZE = 100
DB_CONNECT_STRING = 'mysql://root:ossdbg1@47.96.141.182/collected_datas'


class My_server(SocketServer.BaseRequestHandler):
    # def handle(self):
    #     #while True:
    #     print "request", self.request
    #     data = self.request[0]
    #     print(data.decode('utf-8'))
    #     print(self.client_address, self.request[1])
    #     #mag =input('>>>:').strip()
    #     #self.request[1].sendto(mag.encode('utf-8'),self.client_address)
    #     # print(self.request)
    #     # print(self.client_address)
    #     self.request[1].sendto('bbb'.encode('utf-8'), self.client_address)

    def splist(self, l, s):
        return [l[i:i+s] for i in range(len(l)) if i%s==0]

    def add_to_db(self, client_addr, datas):
        engine = create_engine(DB_CONNECT_STRING, echo=True)
        DB_Session = sessionmaker(bind=engine)
        session = DB_Session()
        ip = client_addr[0]
        port = client_addr[1]
        datas = ",".join(datas)
        split_data = self.splist(datas.split(","), 1000)
        for data in split_data:
            cur_time = time.strftime('%Y-%m-%d %H:%M:%S',time.localtime(time.time()))
            up_link = UpLink(ip=ip, port=port, data=",".join(data), updated_at=cur_time, flag=1)
            session.add(up_link)
        session.commit()
        session.close()
        total_data=[]

    def clear_db_flag(self):
        engine = create_engine(DB_CONNECT_STRING, echo=True)
        DB_Session = sessionmaker(bind=engine)
        session = DB_Session()
        session.execute('update up_link set flag=0 where flag=1')
        session.commit()
        session.close()


    def handle(self):
        data = ''
        data = self.request[0]
        print "client_con", client_con
        if "End" in data:
           connected_addr = client_con["MCU0"][1]
           self.add_to_db(connected_addr, total_data) 
        if len(data) > 0:
            if data.startswith("Start"):
                self.clear_db_flag()
                total_data[data.find(",")+1:]
            elif data.startswith("client"):
                client_socket = self.request[1]
                client_con[data.split(":")[1].strip()] = (client_socket, self.client_address)
                client_socket.sendto("CONNECT SERVER SUCCESS ACK".encode('utf-8'), self.client_address)
            elif data.startswith("downlink") and data.split(":")[1] in client_con.keys():
                connected_sock = client_con[data.split(":")[1]][0]
                connected_addr = client_con[data.split(":")[1]][1]
                connected_sock.sendto("CMD:Manual:1:30".encode('utf-8'), connected_addr)
            else:
                total_data.append(data)

if __name__ == '__main__':
    # ip_port =('', 18100)
    print 'build connect when new UDP comes'
    obj = SocketServer.ThreadingUDPServer(ADDR, My_server)
    obj.serve_forever()
