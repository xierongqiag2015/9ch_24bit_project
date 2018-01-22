import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import matplotlib
import numpy as np
matplotlib.use("TkAgg")
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
from matplotlib.figure import Figure
import re
import Tkinter as tk
import ttk
import time

from sqlalchemy import create_engine
from sqlalchemy.orm import sessionmaker
from up_link import UpLink
from down_link import DownLink
from models import Base

DB_CONNECT_STRING = 'mysql://root:ossdbg1@47.96.141.182/collected_datas'
engine = create_engine(DB_CONNECT_STRING, echo=True)
DB_Session = sessionmaker(bind=engine)
session = DB_Session()
Base.metadata.create_all(engine)
total_datas = []
record = session.query(UpLink).filter(UpLink.flag == 1).all()

def find_all_index(arr, item):
    return [i for i,a in enumerate(arr) if a==item]

metric = []
for r in record:
    data = r.data
    data = data.split(",")
    index = find_all_index(data, "")
    
    for i in index:
        data[i] = 1.0
    data = [float(x) for x in data]

    for i in index:
        data[i] = (data[i-1] +data[i-2])/2
    
    if data:
        metric.append(data)

start = range(1000)
fig = plt.figure()
window = fig.add_subplot(111)
line, = window.plot(start)
plt.ylim(0, 1.25)

def update(data):
    line.set_ydata(data)
    return line,

ani = animation.FuncAnimation(fig, update, metric, interval=1000)
plt.show()
