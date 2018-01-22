import SocketServer

class My_server(SocketServer.BaseRequestHandler):
    def handle(self):
        #while True:
        print "request", self.request
        data = self.request[0]
        print(data.decode('utf-8'))
        print(self.client_address, self.request[1])
        #mag =input('>>>:').strip()
        #self.request[1].sendto(mag.encode('utf-8'),self.client_address)
        # print(self.request)
        # print(self.client_address)
        self.request[1].sendto('bbb'.encode('utf-8'), self.client_address)

if __name__ == '__main__':
    ip_port =('', 18100)
    obj = SocketServer.ThreadingUDPServer(ip_port, My_server)
    obj.serve_forever()#提供了一个循环
