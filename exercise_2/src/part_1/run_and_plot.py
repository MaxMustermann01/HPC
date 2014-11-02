# -*- coding: utf-8 -*-
import time
import os
import matplotlib as mp

av = []
ts = []
n = range(2,25)

if not os.access("./main.c",os.F_OK):
    raise EnvironmentError("main.c is missing!")

os.system("scp ./main.c hpc_creek08:~/chr.c")
os.system("ssh hpc_creek08 'mpicc -Wall ~/chr.c -o ~/chr'")

for nn in n:
    print "starting executable with",nn,"processes"
    pipe = os.popen("ssh hpc_creek08 'mpirun -host creek01,creek02,creek03,creek04,creek05,creek06,creek07,creek08 -np "+str(nn)+" ~/chr'",'r')
    time.sleep(2)
    output = pipe.read()
    print "the output for",nn,"processes is:", output
    pipe.close()
    totalTime = float(output.split()[0])
    averageTime = float(output.split()[1])
    ts.append(totalTime)
    av.append(1000*averageTime)

os.system("ssh hpc_creek08 'rm ~/chr'")
os.system("ssh hpc_creek08 'rm ~/chr.c'")



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

import matplotlib.pyplot as plt

fig = plt.figure(figsize=(4,8))

ax1 = fig.add_subplot(211)
ax2 = fig.add_subplot(212)
ax1.plot(n,ts)
ax1.set_xlabel(r"number of processes $n$")
ax1.set_ylabel("total communication\ntime [s]")
ax2.plot(n,av)
ax2.set_xlabel(r"number of processes $n$")
ax2.set_ylabel("average time per\nmessage [ms]")
fig.subplots_adjust(left = 0.2, hspace=0.0)
ax1.xaxis.grid()
ax2.xaxis.grid()
ax1.xaxis.set_ticklabels([])
ax1yticks = ax1.yaxis.get_major_ticks()
ax2yticks = ax2.yaxis.get_major_ticks()

ax1yticks[0].label1On = False
ax2yticks[-1].label1On = False
ax2yticks[0].label1On = False

fig.savefig("plot.pgf")
