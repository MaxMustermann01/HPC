# -*- coding: utf-8 -*-
import matplotlib as mp
mp.use("pgf")
pdf_with_custom_preamble = {
    "font.family":"serif",
    "font.serif":[],
    "font.size":11,
    "pgf.rcfonts":False,
    "text.usetex":True,
    "pgf.texsystem":"pdflatex",
    "pgf.preamble": [
        r"\usepackage[T1]{fontenc}",
        r"\usepackage{lmodern}",
        ]
}

mp.rcParams.update(pdf_with_custom_preamble)

import csv
import matplotlib.pyplot as plt
import numpy as np

pathData = "./sh_thread_count.csv"
pathPlot = "./sh_thread_count.pgf"

#######################
# OCCUPANCY CALCULATION
#######################


pBlockSize = map(lambda x: x**2,range(1,33))

regPerThread = 11
sharedMemPerSM = 49152 # bytes
maxThreadCountPerSM = 1536
maxRegPerSM = 32768

def sharedMemPerBlock(blocksize):
    return blocksize * 3 * 4

def checkSharedMemSize(threadCount, blockSize, sharedMemSize):
    blockCount = int(threadCount)/int(blockSize)
    sharedMemTot = blockCount * sharedMemSize
    if (sharedMemTot > sharedMemPerSM):
        return (int(sharedMemPerSM)/int(sharedMemSize))*blockSize
    else:
        return blockCount * blockSize

def checkMaxThreadCount(threadCount, blockSize):
    blockCount = int(threadCount)/int(blockSize)
    realThreadCount = blockCount * blockSize
    if (realThreadCount > maxThreadCountPerSM):
        return int(maxThreadCountPerSM) / int(blockSize) 
    else:
        return realThreadCount

def checkRegisters(threadCount, blockSize):
    blockCount = int(threadCount) / int(blockSize)
    realThreadCount = blockCount * blockSize
    registerCount = realThreadCount * regPerThread
    if (registerCount > maxRegPerSM):
        return ((int(maxRegPerSM) / int(regPerThread)) / int(blockSize)) * blockSize
    else:
        return realThreadCount


pThreadsPerSM = []
pUsedSharedMem = []

for bs in pBlockSize:
    thread = maxThreadCountPerSM
    sharedMem = sharedMemPerBlock(bs)
    thread = checkSharedMemSize(thread, bs, sharedMem)
    thread = checkMaxThreadCount(thread, bs)
    thread = checkRegisters(thread, bs)
    pThreadsPerSM.append(thread)
    blockCount = int(thread) / int(bs)
    pUsedSharedMem.append(  float(blockCount*sharedMem) / float(sharedMemPerSM))

################
# PLOT OCUPPANCY
################

fig = plt.figure(figsize=(6,7))
ax0 = fig.add_subplot(211)
ax1 = fig.add_subplot(212)

ax0.plot(pBlockSize,np.array(pThreadsPerSM)/float(maxThreadCountPerSM), c='g')
ax0.set_ylabel("Occupancy")
ax0.set_xlabel(" ")
ax0.xaxis.set_tick_params(labelbottom=False)


ax0.lines[0].set_marker('.')
ax0.set_xscale("log")
ax0.grid()

ax1.plot(pBlockSize, pUsedSharedMem)
ax1.set_xlabel("threads per block")
ax1.set_ylabel("used shared memory per SM")
ax1.set_xscale("log")
ax1.lines[0].set_marker('.')
ax1.grid()
x = pBlockSize[-6]
y = pUsedSharedMem[-6]

ax1.annotate("("+str(x)+", "+str(y)[:4]+")",xy=(x,y))

# MATPLOTLIB TICK MANIPULATION
fig.canvas.draw()
tl = ax0.yaxis.get_major_ticks()
tl[0].label1On = False
tl = ax1.yaxis.get_major_ticks()
tl[-1].label1On = False

fig.subplots_adjust(hspace = 0)

fig.savefig("./occupancy.pgf")

##########
# GET DATA
##########


pData = []
f = open(pathData, 'r')
reader = csv.reader(f, delimiter=',', quotechar='#')

## skip header ##
reader.next()

for row in reader:
    pData.append(map(float, row))

f.close()    

pData = np.transpose(pData)

###########
# PLOT DATA
###########

fig = plt.figure(figsize=(6,4))
ax = fig.add_subplot(111)

ax.set_xlabel("threads per block")
ax.set_ylabel("kernel time [ms]")
ax.set_xscale("log")
ax.set_yscale("log")
ax.grid()
ax.plot(pData[1], pData[8])
ax.lines[0].set_marker('.')

fig.savefig(pathPlot)

fig = plt.figure(figsize=(6,4))
ax = fig.add_subplot(111)

ax.set_xlabel("threads per block")
ax.set_ylabel("speedup")
#ax.set_xscale("log")
ax.grid()
ax.plot(pData[1], pData[-2], label="without copy time")
ax.plot(pData[1], pData[-1], label="with copy time")
ax.lines[0].set_marker('.')
ax.lines[1].set_marker('.')
ax.legend(bbox_to_anchor=(0.,1.02,1,.102),loc=3,ncol=2,mode="expand",borderaxespad=0)

maxId = np.argmax(pData[-2])
x = pData[1][maxId]
y = pData[-2][maxId]

ax.annotate("("+"{:.2e}".format(x)+", "+"{:.1f}".format(y)+")",\
    xy=(x,y), xytext=(400, 26), arrowprops=\
    dict(facecolor="blue",arrowstyle="->"), color = 'blue')
x = pData[1][-5]
y = pData[-2][-5]

bbox_props = dict(boxstyle="rarrow,pad=0.3", fc="yellow",ec="black", lw=2)

t = ax.text(x,15,"Occupancy Break-In",bbox = bbox_props,\
    horizontalalignment="center", va="center",rotation=90, size = 15)

fig.savefig("./thread_speedup.pgf")

##########
# GET DATA
##########

pathData = "./sh_size.csv"
pData = []
f = open(pathData, 'r')
reader = csv.reader(f, delimiter=',', quotechar='#')

## skip header ##
reader.next()

for row in reader:
    pData.append(map(float, row))

f.close()    

pData = np.transpose(pData)

###########
# PLOT DATA
###########

pathPlot = "./sh_size.pgf"
fig = plt.figure(figsize=(6,4))
ax = fig.add_subplot(111)

ax.set_xlabel("matrix elements")
ax.set_ylabel("time [ms]")
ax.set_xscale("log")
ax.set_yscale("log")
ax.grid()
ax.plot(pData[3], pData[8], label="kernel time")
ax.plot(pData[3], pData[4], label="copy to device")
ax.plot(pData[3], pData[6], label="copy from device")
ax.plot(pData[3], pData[8]+pData[4]+pData[6], label="kernel time + copy time")

ax.lines[0].set_marker('.')
ax.lines[1].set_marker('.')
ax.lines[2].set_marker('.')
ax.lines[3].set_marker('.')
fig.subplots_adjust(top=0.8)
ax.legend(bbox_to_anchor=(0.,1.02,1,.102),loc=3,ncol=2,mode="expand",borderaxespad=0)
fig.savefig(pathPlot)

###########
# PLOT DATA
###########

pathPlot = "./sh_speedup.pgf"
fig = plt.figure(figsize=(6,6))
ax = fig.add_subplot(211)
ax0 = fig.add_subplot(212)
ax.set_xlabel("matrix elements")
ax.set_ylabel("speedup")
ax.set_xscale("log")
ax.grid()
ax.plot(pData[3], pData[10], label="without copy time")
ax.plot(pData[3], pData[11], label="with copy time")
maxId = np.argmax(pData[10])
x = pData[3][maxId]
y = pData[10][maxId]
ax.annotate("("+"{:.2e}".format(x)+", "+"{:.2f}".format(y)+")",\
                xy=(x,y),color="b")
maxId = np.argmax(pData[11])
x = pData[3][maxId]
y = pData[11][maxId]
ax.annotate("("+"{:.2e}".format(x)+", "+"{:.1f}".format(y)+")",\
    xy=(x,y), xytext=(10**7, 26), arrowprops=\
    dict(facecolor="green",arrowstyle="->"), color = 'g')
ax.lines[0].set_marker('.')
ax.lines[1].set_marker('.')
ax.legend(bbox_to_anchor=(0.,1.02,1,.102),loc=3,ncol=2,mode="expand",borderaxespad=0)

ax0.set_xlabel("matrix elements")
ax0.set_ylabel("speedup")
ax0.grid()
ax0.plot(pData[3], pData[10], label="without copy time")
ax0.plot(pData[3], pData[11], label="with copy time")
ax0.lines[0].set_marker('.')
ax0.lines[1].set_marker('.')

fig.subplots_adjust(hspace=0.2)

fig.savefig(pathPlot)
