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

import matplotlib.pyplot as plt
import numpy as np
import os

sshShortcut = "hpc_creek01"
executableDir = "~/sandbox/"
lLabels = ["blocking_one_node", "non_blocking_one_node", "blocking_flood_two_nodes", "non_blocking_two_nodes"]
lExecutables = ["blocking", "non_blocking"]
lCflags = ["","","-host creek01,creek02", "-host creek01,creek02"]

## plot manipulation ##
fig = plt.figure(figsize=(4.5,4.5))
ax = fig.add_subplot(111)
ax.set_xlabel("Packet Size [Bytes]")
ax.set_ylabel("Bandwidth [Bytes/s]")
ax.set_yscale("log")
ax.set_xscale("log")
ax.grid()

## take measurements from creek01 and plot to ax##
for i in range(4):
    
    ## get data from standard output via ssh ##
    print "COMMAND","ssh "+sshShortcut+" 'srun mpirun "+lCflags[i]+" -np 2 "+executableDir+lExecutables[i%2]+"'"
    pipe = os.popen("ssh "+sshShortcut+" 'srun mpirun "+lCflags[i]+" -np 2 "+executableDir+lExecutables[i%2]+"'")
    output = pipe.read()
    pipe.close()
    print "OUTPUT\n"
    print output
    ## convert output string to float arrays ##
    lTimes = []
    lPacketSizes = []
    lOutput = output.split("\n")[1:-1]

    for row in lOutput:
        lTimes.append(float(row.split()[1]))
        lPacketSizes.append(float(row.split()[0]))
    
    arrBandwidth = np.zeros(len(lTimes))
    arrBandwidth = np.array(lPacketSizes)/np.array(lTimes)

    ## plot data ##
    ax.plot(lPacketSizes, arrBandwidth, label=lLabels[i])

## plot manipulation ##
ax.legend(loc=3,bbox_to_anchor=(0.,1.02,1.,.102), ncol=1, mode="expand", borderaxespad=0.)
fig.savefig("plot.pgf")

