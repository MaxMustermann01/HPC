#################################################################################
# FILENAME         make_data.py
#
# DESCRIPTION      this sript executes the program for sheetV exercise 2
#                  for different thread counts and matrix sizes and writes
#                  the measurements to a comma seperated csv file (csv is 
#                  a common data format useful for column sorted data).
#                  The user should specify the given options in the 
#                  'USER INPUT' section. 
#
# AUTHORS          Christoph Klein
#
# LAST CHANGE      2 DEZ 2014
#
################################################################################

import csv
import os
import math as m

############
# USER INPUT
############

# path to data output (will be replaced)
dataPath = "./sh_size.csv"
# path to executable
execPath = "/home/introhpc03/sandbox/bin/matMul"
# use pinned memory
pinned = True
#
# parameter range:
#   the program will take data for every possible parameter
#   combination. That means the program will be executed
#   len(lThreadRoot) * len(lMatrixWidth) times.
#
lThreadRoot = [27]
lMatrixWidth = []
count = 50
offset = (m.log10(4000) - 2) / float(count)
vv = 2
for i in range(count):
    lMatrixWidth.append(vv)
    vv += offset

#lMatrixWidth = map(lambda x: int(10**x), lMatrixWidth)
lMatrixWidth = [11,16,22,32,45,64,90]

##################
# USEFUL FUNCTIONS
##################

def sshCommand(command):
    pipe = os.popen("ssh hpc_creek08 '"+command+"'",'r')
    out = pipe.read()
    return out

def giveVal(string, search):
    nr = string.find(search)
    words = string[nr:].split()
    if len(words) > 1: return words[1]
    else: raise ValueError("giveValueToString: key not found")

###########
# EXECUTION
###########

f = open(dataPath, 'w')
writer = csv.writer(f, delimiter = ',', quotechar='#')
fm = "{:>10}" # format specifier

# write header
writer.writerow(["thread-root", "total-thread-count", "matrix-width"\
    , "matrix-size", "copy-to-device[ms]", "bandwidth-to-device[GB/s]"\
    , "copy-from-device[ms]", "bandwidth-from-device[GB/s]"\
    , "kernel-matrix-multiplication[ms]", "cpu-matrix-multiplication[ms]"\
    , "speedup-kernel", "speedup-overall"])

for tr in lThreadRoot:
    for mw in lMatrixWidth:
        flags = " -t " + str(tr) + " -s " + str(mw) + " --shared"
        if (pinned): flags += " -p "
        row = [str(tr),str(tr**2),str(mw),str(mw**2)]
        print "ACTUAL COMMAND:", execPath + flags
        values = [0]*8
        for i in range(20):
            pipe = os.popen(execPath + flags, 'r')
            out = pipe.read()
            pipe.close()
            values[0] += float(giveVal(out, "time-to-copy-to-device"))
            values[1] += float(giveVal(out, "copy-bandwidth-to-device"))
            values[2] += float(giveVal(out, "time-to-copy-from-device"))
            values[3] += float(giveVal(out, "copy-bandwidth-from-device"))
            values[4] += float(giveVal(out, "time-for-matrix-multiplication"))
            values[5] += float(giveVal(out, "time-for-cpu-matrix-multiplication"))
            values[6] += float(giveVal(out, "speedup-kernel"))
            values[7] += float(giveVal(out, "speedup-overall"))
        values = map(lambda x: x/20.0,values)
        row += map(str,values)
        row = map(fm.format, row)
        writer.writerow(row) 

f.close()        
