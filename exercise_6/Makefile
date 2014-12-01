#
# Exercise 6 - Makefile
# AUTHORS  Klaus Naumann
#          Christoph Klein
#          Günther Schindler
#
CC = mpicc
CPP = mpic++
CFLAGS = -Wall
TEXC = pdflatex
DOCUNAME = docu_3

relaxation:
	$(CP) $(CFLAGS)		src/main.c \
				src/matrix.c \
				src/time_measurement.c
			-o relaxation

clean :
	rm -f 	relaxation