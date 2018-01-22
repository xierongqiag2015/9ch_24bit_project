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
        split_data = self.splist(datas.split(","), 1000)
        for data in split_data:
            cur_time = time.strftime('%Y-%m-%d %H:%M:%S',time.localtime(time.time()))
            up_link = UpLink(ip=ip, port=port, data=",".join(data), updated_at=cur_time, flag=1)
            session.add(up_link)
        session.commit()
        session.close()

    def clear_db_flag(self):
        engine = create_engine(DB_CONNECT_STRING, echo=True)
        DB_Session = sessionmaker(bind=engine)
        session = DB_Session()
        session.execute('update up_link set flag=0 where flag=1')
        session.commit()
        session.close()


    def handle(self):
        total_data=[]
        data = ''
        data = self.request[0]
        if "End" in data:
            total_data.append(data[:data.find("End")])
            # connected_sock.close()
            total_data = ",".join(total_data)
            print 'receive total data:\n', total_data
            connected_addr = client_con["MCU0"]
            self.add_to_db(connected_addr, total_data)
            
        if len(data) > 0:
            print 'receive data:\n', data

            #cur_thread = threading.current_thread()
            #send_data = '{}:{}'.format(cur_thread.ident,data)
            if data.startswith("Start"):
                self.clear_db_flag()
                self.add_to_db(self.client_address, data.strip("Start,"))
                total_data[data.find(",")+1:]
            elif data.startswith("client"):
                client_con[data.split(":")[1]] = self.request[1]
            elif data.startswith("downlink") and data.split(":")[1] in client_con.keys():
                print "down link:", data
                connected_sock = client_con[data.split(":")[1]][0]
                connected_addr = client_con[data.split(":")[1]][1]
                connected_sock.sendto("CMD:Manual:1:30".encode('utf-8'), connected_addr)
            else:
                # add_to_db(client_addr, data)
                total_data.append(data)
                pass
            
            # if data.startswith("client"):
            #     client_con[data.split(":")[1]] = connected_sock
            #     # add_to_db(client_addr, data)
                
            # if data.startswith("downlink") and data.split(":")[1] in client_con.keys():
            #     connected_sock = client_con[data.split(":")[1]]
            #     connected_sock.sendall("abc") #用sendall，不要用send，send并不一定发送所有send_data，可能发送了部分就返回了            print 'send:',send_data
            
        else:
            total_data = ",".join(total_data)
            print 'receive total data:\n', total_data
            self.add_to_db(self.client_address, total_data)
            # print 'close the connected socket and terminate sub thread'
            # connected_sock.close()

# while True:
#     try:
#         connected_sock, client_addr = listen_sock.accept()
        
#         print "client_addr:", client_addr
#         # if connected_sock:
#         #     connected_sock.sendall("abc")
#     except socket.timeout:
#         length = len(sub_threads)
#         while length:
#             sub = sub_threads.pop(0)
#             sub_id = sub.ident  #进程ID
#             sub.join(0.1) #等待线程结束，0.1秒
#             if sub.isAlive():
#                 sub_threads.append(sub)
#             else:
#                 print 'killed sub thread ',sub_id
#             length -=1
#     else:
#         t = threading.Thread(target=handle,name='sub thread',args=(connected_sock, client_addr))
#         #它继承了listen_socket的阻塞/非阻塞特性，因为listen_socket是非阻塞的，所以它也是非阻塞的
#         #要让他变为阻塞，所以要调用setblocking
#         connected_sock.setblocking(1) 
#         t.start()
#         sub_threads.append(t)

if __name__ == '__main__':
    # ip_port =('', 18100)
    print 'build connect when new UDP comes'
    obj = SocketServer.ThreadingUDPServer(ADDR, My_server)
    obj.serve_forever()
