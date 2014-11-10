#
# Exercise 3 - Makefile
# AUTHORS  Klaus Naumann
#          Christoph Klein
#          Günther Schindler
#
CC = mpicc
CPP = mpic++
CFLAGS = -Wall
TEXC = pdflatex
DOCUNAME = docu_3

measure_latency:
	$(CPP) $(CFLAGS)	src/part_3/measure_latency.cpp \
			-o measure_latency

docu :
	$(TEXC) $(DOCUNAME).tex

clean :
	rm -f 	measure_latency \
		$(DOCUNAME).aux $(DOCUNAME).log $(DOCUNAME).pdf $(DOCUNAME).toc

