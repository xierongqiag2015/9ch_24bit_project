#! /usr/bin/env python
# -*- coding: utf-8 -*-

import logging
import os
import socket
import sys
import Tkinter as tk
import time
import ttk
import tkFont

import PyTkinter as pytk
from Utils import Tooltip
import numpy as np
import matplotlib
import matplotlib.pyplot as plt

from numpy import arange, sin, pi
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg, NavigationToolbar2TkAgg
# implement the default mpl key bindings
from matplotlib.backend_bases import key_press_handler


from matplotlib.figure import Figure
import matplotlib.animation as animation

matplotlib.use('TkAgg')
# g_font = ('Monaco', 12)
g_font = ('宋体', 12)
logging.basicConfig(level=logging.DEBUG,
                format='%(asctime)s %(filename)s[line:%(lineno)d] %(levelname)s %(message)s',
                datefmt='%a, %d %b %Y %H:%M:%S',
                filename='D:/qlq_install/project/SlaveDebugTool/Log/myapp.log',
                filemode='w')


class SerialFrame(object):
    '''
    Serial窗体
    '''

    def __init__(self, master=None):
        '''
        初始化
        '''
        self.root = master
        self.create_frame()

    def create_frame(self):
        '''
        创建窗体，分为上下2个部分，下半部分为状态栏
        '''
        self.frm = pytk.PyFrame(self.root)

        self.frm_top = pytk.PyLabelFrame(self.frm)
        self.frm_status = pytk.PyLabelFrame(self.frm)

        self.frm_top.pack(fill="both", expand=1)
        self.frm_status.pack(fill="both", expand=0)

        self.create_frm_top()
        self.create_frm_status()

    def create_frm_top(self):
        '''
        上半部分窗口分为左中右3个部分
        '''
        self.frm_left = pytk.PyLabelFrame(self.frm_top)
        # self.frm_middle = pytk.PyFrame(self.frm_top)
        #self.frm_right = pytk.PyLabelFrame(self.frm_top)

        self.frm_left.pack(fill="both", expand=1, padx=2, pady=5)
        # self.frm_left.grid(row=0, column=0, padx=5, pady=5, sticky="wesn")
        #self.frm_right.pack(fill="both", expand=1, padx=2, pady=5, side=tk.RIGHT)
        # self.frm_middle.pack(fill="both", expand=1, padx=4, pady=5, side=tk.RIGHT)

        self.create_frm_left()
        # self.create_frm_middle()
        #self.create_frm_right()

    def create_frm_left(self):
        '''
        上半部分左边窗口：
        Listbox显示连接的USB设备
        Button按钮点击连接设备
        '''
        self.frm_left_label = pytk.PyLabel(self.frm_left,
                                           text="设备",
                                           font=g_font,
                                           anchor="w")
        self.frm_left_listbox = pytk.PyListbox(self.frm_left,
                                               font=g_font)
        self.frm_left_serial_set = pytk.PyLabelFrame(self.frm_left)
        # self.frm_left_btn = pytk.PyButton(self.frm_left,
        #                                   text="Config",
        #                                   font=g_font,
        #                                   command=self.Toggle)
        
        self.frm_left_config_btn = pytk.PyButton(self.frm_left,
                                          text="配置",
                                          font=g_font,
                                          command=self.get_var)
        self.frm_left_show_btn = pytk.PyButton(self.frm_left,
                                          text="实时显示",
                                          font=g_font,
                                          command=self.wave_show)

        self.frm_left_label.pack(fill="both", expand=0, padx=5, pady=5)
        self.frm_left_listbox.pack(fill="both", expand=1, padx=5, pady=5)
        self.frm_left_serial_set.pack(fill="both", expand=0, padx=5, pady=5)
        self.frm_left_config_btn.pack(fill="both", expand=0, padx=5, pady=5)
        self.frm_left_show_btn.pack(fill="both", expand=0, padx=5, pady=5)

        self.frm_left_listbox.bind("<Double-Button-1>", self.Open)
        self.create_frm_left_serial_set()

    def create_frm_left_serial_set(self):
        '''
        采样率、采集时间等
        '''
        #setting_label_list = ["BaudRate :", "Parity :", "DataBit :", "StopBit :", "From :", "To :"]
        setting_label_list = ["采样率:", "手动采集时间:", "自动采集从:", "自动采集到:"]
        # baudrate_list = ["1200", "2400", "4800", "9600", "14400", "19200", "38400",
        #                  "43000", "57600", "76800", "115200"]
        # PARITY_NONE, PARITY_EVEN, PARITY_ODD PARITY_MARK, PARITY_SPACE
        # parity_list = ["N", "E", "O", "M", "S"]
        # bytesize_list = ["5", "6", "7", "8"]
        # stopbits_list = ["1", "1.5", "2"]

        self.frm_left_left = pytk.PyFrame(self.frm_left_serial_set)
        self.frm_left_right = pytk.PyFrame(self.frm_left_serial_set)
        self.frm_left_left.pack(fill="both", expand=1, side=tk.LEFT)
        self.frm_left_right.pack(fill="both", expand=1, side=tk.RIGHT)

        for item in setting_label_list:
            frm_left_label_temp = pytk.PyLabel(self.frm_left_left, 
                                               text=item,
                                               font=g_font
                                               )
            frm_left_label_temp.pack(fill="both", expand=1, padx=5, pady=5)

        # self.frm_left_combobox_baudrate = ttk.Combobox(self.frm_left_right,
        #                                                width=15,
        #                                                font=g_font,
        #                                                values=baudrate_list)
        # self.frm_left_combobox_parity = ttk.Combobox(self.frm_left_right,
        #                                                width=15,
        #                                                font=g_font,
        #                                                values=parity_list)
        # self.frm_left_combobox_databit = ttk.Combobox(self.frm_left_right,
        #                                                width=15,
        #                                                font=g_font,
        #                                                values=bytesize_list)
        # self.frm_left_combobox_stopbit = ttk.Combobox(self.frm_left_right,
                                                       # width=15,
                                                       # font=g_font,
                                                       # values=stopbits_list)
        #sampling rate
        samp_var = tk.StringVar()
        manual_samp_var = tk.StringVar()
        from_var = tk.StringVar()
        to_var = tk.StringVar()


        self.frm_left_entry_samp = tk.Entry(self.frm_left_right,
                                            background="grey",
                                            # foreground="#FFFFFF",
                                            width=15,
                                            font=g_font, 
                                            textvariable=samp_var)
        self.frm_left_entry_manual_samp = tk.Entry(self.frm_left_right,
                                                  background="grey",
                                                  # foreground="#FFFFFF",
                                                  width=15,
                                                  font=g_font, 
                                                  textvariable=manual_samp_var)
        self.frm_left_entry_from = tk.Entry(self.frm_left_right,
                                            background="grey",
                                            # foreground="#FFFFFF",
                                            width=15,
                                            font=g_font, 
                                            textvariable=from_var)
        self.frm_left_entry_to = tk.Entry(self.frm_left_right,
                                          width=15,
                                          font=g_font,
                                          background="grey",
                                          # foreground="#FFFFFF",
                                          textvariable=to_var)

        Tooltip.createToolTip(self.frm_left_entry_samp, "单位:HZ")
        Tooltip.createToolTip(self.frm_left_entry_manual_samp, "单位:分钟")
        Tooltip.createToolTip(self.frm_left_entry_from, "年-月-日 时:分")
        Tooltip.createToolTip(self.frm_left_entry_to, "年-月-日 时:分")

        # self.frm_left_combobox_baudrate.pack(fill="both", expand=1, padx=5, pady=5)
        # self.frm_left_combobox_parity.pack(fill="both", expand=1, padx=5, pady=5)
        # self.frm_left_combobox_databit.pack(fill="both", expand=1, padx=5, pady=5)
        # self.frm_left_combobox_stopbit.pack(fill="both", expand=1, padx=5, pady=5)
        self.frm_left_entry_samp.pack(fill="both", expand=1, padx=5, pady=5)
        self.frm_left_entry_manual_samp.pack(fill="both", expand=1, padx=5, pady=5)
        self.frm_left_entry_from.pack(fill="both", expand=1, padx=5, pady=5)
        self.frm_left_entry_to.pack(fill="both", expand=1, padx=5, pady=5)

        # self.frm_left_combobox_baudrate.current(3)
        # self.frm_left_combobox_parity.current(0)
        # self.frm_left_combobox_databit.current(3)
        # self.frm_left_combobox_stopbit.current(0)

    def wave_show(self):
        cwd = os.getcwd()
        print "cwd==", cwd
        if not cwd.endswith("UI"):
            os.chdir(cwd + "\UI")
        os.popen("chmod 777 ./tkinter_gui.py")
        os.popen('python ./tkinter_gui.py')

    def show_frame(self, cont):
        frame = self.frames[cont]
        frame.tkraise()
        frame.canvas.draw_idle()

    def create_frm_middle(self):
        '''
        上半部分中间窗口：
        主要用来显示threshold
        
        '''
        
        # self.frm_middle_reset = pytk.PyLabelFrame(self.frm_middle)
        # self.frm_middle_send = pytk.PyText(self.frm_middle,
        #                                    font=g_font,
        #                                    width=60,
        #                                    height=5)
        # self.frm_middle_reset.pack(fill="both", expand=0, padx=1)
        # self.frm_middle_send.pack(fill="both", expand=1, padx=1)

        # # a tk.DrawingArea
        # line_ani = animation.FuncAnimation(fig, update, metric, interval=50, blit=True)
        # canvas = FigureCanvasTkAgg(fig, master=self.frm_middle)
        # canvas.show()
        # canvas.get_tk_widget().pack(side=tk.TOP, fill=tk.BOTH, expand=1)

        # # toolbar = NavigationToolbar2TkAgg(canvas, self.frm_middle)
        # # toolbar.update()
        # canvas._tkcanvas.pack(side=tk.TOP, fill=tk.BOTH, expand=1)


        # def on_key_event(event):
        #     print('you pressed %s' % event.key)
        #     key_press_handler(event, canvas, toolbar)

        # canvas.mpl_connect('key_press_event', on_key_event)


        # def _quit():
        #     self.frm_middle.quit()     # stops mainloop
        #     self.frm_middle.destroy()  # this is necessary on Windows to prevent
        #                     # Fatal Python Error: PyEval_RestoreThread: NULL tstate

        # button = tk.Button(master=self.frm_middle, text='Quit', command=_quit)
        # button.pack(side=tk.BOTTOM)

        #tk.mainloop()

    def create_frm_right(self):
        '''
        上半部分右边窗口：
        分为4个部分：
        1、Label显示和重置按钮和发送按钮
        2、Text显示（发送的数据）
        3、Label显示和十进制选择显示和清除接收信息按钮
        4、Text显示接收到的信息
        '''
        self.frm_right_reset = pytk.PyLabelFrame(self.frm_right)
        self.frm_right_send = pytk.PyText(self.frm_right,
                                          font=g_font,
                                          width=60,
                                          height=5)
        self.frm_right_clear = pytk.PyLabelFrame(self.frm_right)
        self.frm_right_receive = pytk.PyText(self.frm_right,
                                             font=g_font,
                                             width=60,
                                             height=15)

        self.frm_right_reset.pack(fill="both", expand=0, padx=1)
        self.frm_right_send.pack(fill="both", expand=1, padx=1)
        self.frm_right_clear.pack(fill="both", expand=0, padx=1)
        self.frm_right_receive.pack(fill="both", expand=1, padx=1)

        self.frm_right_receive.tag_config("green", foreground="#228B22")

        #self.create_frm_right_reset()
        #self.create_frm_right_clear()

    def create_frm_right_reset(self):
        '''
        1、Label显示和重置按钮和发送按钮
        '''
        self.frm_right_reset_label = pytk.PyLabel(self.frm_right_reset,
                                                  text="Hex Bytes",
                                                  font=g_font,
                                                  anchor="w")
        self.frm_right_reset_btn = pytk.PyButton(self.frm_right_reset,
                                                 text="Reset",
                                                 width=10,
                                                 font=g_font,
                                                 command=self.Reset)
        self.frm_right_send_btn = pytk.PyButton(self.frm_right_reset,
                                                text="Send",
                                                width=10,
                                                font=g_font,
                                                command=self.Send)

        self.new_line_cbtn_var = tk.IntVar()
        self.send_hex_cbtn_var = tk.IntVar()
        self.frm_right_reset_newLine_checkbtn = pytk.PyCheckbutton(self.frm_right_reset,
                                                                   text="New Line",
                                                                   variable=self.new_line_cbtn_var,
                                                                   font=g_font)
        self.frm_right_reset_hex_checkbtn = pytk.PyCheckbutton(self.frm_right_reset,
                                                               text="Hex",
                                                               variable=self.send_hex_cbtn_var,
                                                               font=g_font)

        self.frm_right_reset_label.pack(fill="both", expand=1, padx=5, pady=5, side=tk.LEFT)
        self.frm_right_reset_newLine_checkbtn.pack(fill="both", expand=0, padx=5, pady=5, side=tk.LEFT)
        self.frm_right_reset_hex_checkbtn.pack(fill="both", expand=0, padx=5, pady=5, side=tk.LEFT)
        self.frm_right_reset_btn.pack(fill="both", expand=0, padx=5, pady=5, side=tk.LEFT)
        self.frm_right_send_btn.pack(fill="both", expand=0, padx=5, pady=5, side=tk.RIGHT)

    def create_frm_right_clear(self):
        '''
        3、Label显示和清除接收信息按钮
        '''
        self.checkValue = tk.IntVar()
        self.frm_right_clear_label = pytk.PyLabel(self.frm_right_clear,
                                                  text="Data Received",
                                                  anchor="w",
                                                  font=g_font)
        self.frm_right_threshold_label = pytk.PyLabel(self.frm_right_clear,
                                                      text="Threshold:",
                                                      font=g_font)

        self.threshold_str = tk.StringVar()
        self.frm_right_threshold_entry = pytk.PyEntry(self.frm_right_clear,
                                                      textvariable=self.threshold_str,
                                                      width=6,
                                                      font=g_font)

        self.receive_hex_cbtn_var = tk.IntVar()
        self.frm_right_hex_checkbtn = pytk.PyCheckbutton(self.frm_right_clear,
                                                         text="Hex",
                                                         variable=self.receive_hex_cbtn_var,
                                                         relief="flat",
                                                         font=g_font)

        self.frm_right_clear_btn = pytk.PyButton(self.frm_right_clear,
                                                 text="Clear",
                                                 width=10,
                                                 font=g_font)

        self.frm_right_clear_label.pack(fill="both", expand=1, padx=5, pady=5, side=tk.LEFT)
        self.frm_right_threshold_label.pack(fill="both", expand=0, padx=5, pady=5, side=tk.LEFT)
        self.frm_right_threshold_entry.pack(fill="both", expand=0, padx=5, pady=5, side=tk.LEFT)
        self.frm_right_hex_checkbtn.pack(fill="both", expand=0, padx=5, pady=5, side=tk.LEFT)
        self.frm_right_clear_btn.pack(fill="both", expand=0, padx=5, pady=5, side=tk.RIGHT)

    def create_frm_status(self):
        '''
        下半部分状态栏窗口
        '''
        self.frm_status_label = pytk.PyLabel(self.frm_status,
                                             text="Welcome to Serial Tools!",
                                             font=g_font)
        # self.frm_status_label.grid(row=0, column=0, padx=5, pady=5, sticky="wesn")
        self.frm_status_label.pack(fill="both", expand=0)

    def Toggle(self):
        pass

    def get_var(self):
        data = dict()
        ip = "114.221.9.247"
        samp = self.frm_left_entry_samp.get()
        manual_samp_time = self.frm_left_entry_manual_samp.get()
        time_from = self.frm_left_entry_from.get()
        time_to = self.frm_left_entry_to.get()

        if not (self.validate_time(time_from, "start") and self.validate_time(time_to, "end")):
            logging.error("Invalid time format, %Y-%m-%d %H:%M")

        if manual_samp_time and (time_from and time_to):
            raise

        #data['ip'] = ip
        print "time_from", time_from
        print "time_to", time_to
        data['samp'] = samp
        if manual_samp_time:
            data['manual_samp'] = manual_samp_time
        if time_from and time_to:
            data['time_from'] = time_from
            data['time_to'] = time_to
        print "data==", data

         
        self.socket_client(str(data))


    def validate_time(self, date, direction=""):
        try:
            logging.info("The %s time you set is: %s" % (direction, date))
            if date:
                date_tmp = time.strptime(date, "%Y-%m-%d %H:%M")
                if date != time.strftime('%Y-%m-%d %H:%M', date_tmp):
                    raise ValueError
                return True
            else:
                return False
        except ValueError:
            return False


    def socket_client(self, data):
        HOST = '47.96.141.182'
        PORT = 18100
        ADDR =(HOST, PORT)
        BUFSIZE = 1024

        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        try:
            a = sock.connect(ADDR)
            # print "a==", a
        except Exception,e:
            logging.error('error', e)
            sock.close()
            sys.exit()

        logging.info('have connected with server')
        if len(data) > 0:
            print 'send:', data
            #logging.info('send:', data)
            #data['downlink'] = "MCU0"
            print 'downlink:MCU0,' + data
            sock.sendall('downlink:MCU0:' + data) #不要用send()
            recv_data = sock.recv(BUFSIZE)
            # logging.info('received from server:', recv_data)
        sock.close()

    def Open(self, event):
        pass

    def Reset(self):
        self.frm_right_send.delete("0.0", "end")

    def Send(self):
        pass


if __name__ == '__main__':
    '''
    main loop
    '''
    root = tk.Tk()
    root.columnconfigure(0, weight=1)
    root.rowconfigure(0, weight=1)
    root.geometry()

    monacofont = tkFont.Font(family="Monaco", size=16)
    root.option_add("*TCombobox*Listbox*background", "#292929")
    root.option_add("*TCombobox*Listbox*foreground", "#FFFFFF")
    root.option_add("*TCombobox*Listbox*font", monacofont)

    #root.configure(bg="#292929")
    root.configure()
    combostyle = ttk.Style()
    combostyle.theme_use('default')
    combostyle.configure("TCombobox",
                         #selectbackground="#292929",
                         #fieldbackground="#292929",
                         #background="#292929",
                         foreground="#FFFFFF")

    app = SerialFrame(root)
    app.frm.pack(fill="both", expand=1)
    root.mainloop()
