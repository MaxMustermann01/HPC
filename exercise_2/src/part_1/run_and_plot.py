# -*- coding: utf-8 -*-
import time
import os
import numpy as np
import matplotlib as mp

av = np.zeros(23)
ts = np.zeros(23)
n = range(2,25)
readonly = False
numberMeasurements = 10.0

if not readonly:
    os.system("scp ./main.c hpc_creek08:~/chr.c")
    os.system("ssh hpc_creek08 'mpicc -Wall ~/chr.c -o ~/chr'")
    for i in range(int(numberMeasurements)):

        for i,nn in list(enumerate(n)):
            print "starting executable with",nn,"processes"
            pipe = os.popen("ssh hpc_creek08 'mpirun -host creek01,creek02,creek03,creek04,creek05,creek06,creek07,creek08 -np "+str(nn)+" ~/chr'",'r')
            output = pipe.read()
            print "the output for",nn,"processes is:", output
            pipe.close()
            totalTime = float(output.split()[0])
            averageTime = float(output.split()[1])
            ts[i] += totalTime
            av[i] += 1000*averageTime

    os.system("ssh hpc_creek08 'rm ~/chr'")
    os.system("ssh hpc_creek08 'rm ~/chr.c'")
    ts /= numberMeasurements
    av /= numberMeasurements

    if os.access("./plotdata.dat",os.F_OK):
        os.remove("./plotdata.dat")
    f = open("./plotdata.dat",'w')    
    for nn,avav,tsts in zip(n,av,ts):
        f.write(str(nn)+" "+"{:.10f}".format(avav)+" "+"{:.10f}".format(tsts)+"\n")
    print "Wrote plotdata to file ..."
    f.close()    

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

f = open("./plotdata.dat",'r')
content = f.read()
n = []
av = []
ts = []
for line in content.split("\n")[:-1]:
    splittedline = line.split()
    nn = float(splittedline[0])
    avav = float(splittedline[1])
    tsts = float(splittedline[2])

    n.append(nn) 
    av.append(avav)
    ts.append(tsts)

print "read plotdata from file"
f.close()    

mp.rcParams.update(pdf_with_custom_preamble)

import matplotlib.pyplot as plt

fig = plt.figure(figsize=(5,6))

ax1 = fig.add_subplot(211)
ax2 = fig.add_subplot(212)
ax1.plot(n,ts)
ax1.set_xlabel(r"number of processes $n$")
ax1.set_ylabel(r"total communication time [s]")
ax2.plot(n,av)
ax2.set_xlabel(r"number of processes $n$")
ax2.set_ylabel(r"average time per message [ms]")
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
