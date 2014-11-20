#
# Exercise 5 - Makefile
# AUTHORS  Klaus Naumann
#          Christoph Klein
#          Günther Schindler
#
CC = mpicc
GCC = gcc
CPP = mpic++
CFLAGS = -O3
TEXC = pdflatex
DOCUNAME = docu_5

relaxation:
	$(GCC) $(CFLAGS)	src/main.c \
				src/matrix.c \
				src/time_measurement.c \
				-o relaxation

docu :
	$(TEXC) docu/$(DOCUNAME).tex

clean:
	rm -rf relaxation
	rm -rf $(DOCUNAME).aux $(DOCUNAME).log $(DOCUNAME).toc $(DOCUNAME).pdf
