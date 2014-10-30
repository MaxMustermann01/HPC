#
# Exercise 2 - Makefile
# AUTHORS  Klaus Naumann
#          Christoph Klein
#          Günther Schindler
#
CC = mpicc
CFLAGS = -Wall
TEXC = pdflatex
DOCUNAME = docu_2

matrixmultiply :
	$(CC) $(CFLAGS)	src/part_3/main.c \
			src/part_3/matrix_multiply.c \
			src/part_3/time_measurement.c \
			-o matrixmultiply

docu :
	$(TEXC) $(DOCUNAME).tex

clean :
	rm -f 	matrixmultiply \
		$(DOCUNAME).aux $(DOCUNAME).log $(DOCUNAME).pdf $(DOCUNAME).toc