import csv

f = open("./sh_size.csv",'r')
reader = csv.reader(f, delimiter=',', quotechar='#')
fout = open("./table.tex",'w')
reader.next()

outstr = r"\begin{tabular}{l|l|l|l|l}"+"\n"
outstr += r"Problem size & Time [ms] & Speedup & GFLOP/s \\ \hline" + "\n"
for row in reader:
    flop = "{:.2e}".format(float(row[3])*2*float(row[2])/float(row[8])*1e-9)
    outstr+=row[3]+" & "+"{:.4f}".format(float(row[8]))+" & "+"{:.4f}".format(float(row[10]))+" & "+flop+r"\\"+"\n"

outstr += "\end{tabular}"    
fout.write(outstr)
f.close()
fout.close()
