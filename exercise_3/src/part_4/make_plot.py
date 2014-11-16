"""import matplotlib as mp
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
    
mp.rcParams.update(pdf_with_custom_preamble)"""

import matplotlib.pyplot as plt
import numpy as np
import os
import csv

createData = False # just create ./plotdata.csv no plotting
readData = True # read from ./plotdata.csv and plot
pthData = "./plotdata.csv" # .csv extension is mandatory

## environmental variables ##
sshShortcut = "hpc_creek01"
executableDir = "~/sandbox/"
lLabels = ["blocking_one_node", "non_blocking_one_node"\
    , "blocking_flood_two_nodes", "non_blocking_two_nodes"]
lExecutables = ["blocking", "non_blocking"]
lCflags = ["","","-host creek01,creek02", "-host creek01,creek02"]

## take measurements from creek01 and plot to ax##
if createData:
    f = open(pthData,'w')
    writer = csv.DictWriter(f,["packageSizes"] + lLabels, extrasaction="raise",\
        delimiter="\t", quotechar='#')
    writer.writeheader()
    data = dict()
    for i in range(4):
        
        ## get data from standard output via ssh ##
        print "COMMAND","ssh "+sshShortcut+" 'srun mpirun "+lCflags[i]\
            +" -np 2 "+executableDir+lExecutables[i%2]+"'"
        pipe = os.popen("ssh "+sshShortcut+" 'srun mpirun "+lCflags[i]\
            +" -np 2 "+executableDir+lExecutables[i%2]+"'")
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
        data[lLabels[i]] = arrBandwidth 
       
    data["packageSizes"] = lPacketSizes

    ## write to csv file ##
    for i in range(len(lPacketSizes)):
        row = dict()
        for l in (["packageSizes"]+lLabels):
            row[l] = data[l][i]
        writer.writerow(row)    
    
    f.close()

if readData:

    ## plot manipulation ##
    fig = plt.figure(figsize=(4.5,4.5))
    ax = fig.add_subplot(111)
    ax.set_xlabel("Packet Size [Bytes]")
    ax.set_ylabel("Bandwidth [Bytes/s]")
    ax.set_yscale("log")
    ax.set_xscale("log")
    ax.grid()

    ## open csv file and read data ##
    f = open(pthData, 'r')
    reader = csv.DictReader(f,delimiter="\t")
    data = dict(zip(["packageSizes"]+lLabels,[[],[],[],[],[]]))
    for row in reader:
        for lab in row:
            data[lab].append(row[lab])
    f.close()

    ## plot data ##
    for lab in data:
        if not lab == "packageSizes":
            ax.plot(data["packageSizes"], data[lab], label=lab)

    ## plot manipulation ##
    ax.legend(loc=3,bbox_to_anchor=(0.,1.02,1.,.102),\
        ncol=1, mode="expand", borderaxespad=0.)
    ##fig.savefig("plot.pgf")
    plt.show()

