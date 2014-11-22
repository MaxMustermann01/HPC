#
# Exercise 6 - Makefile
# AUTHORS  Klaus Naumann
#          Christoph Klein
#          Günther Schindler
#
CC = mpicc
CPP = mpic++
CFLAGS = 
TEXC = pdflatex
DOCUNAME = docu_4

.PHONY: relaxation
relaxation:
	$(CC) $(CFLAGS)		src/main.c \
				src/matrix.c \
				src/time_measurement.c \
				-o relaxation 

.PHONY: docu
docu :
	$(TEXC) docu/$(DOCUNAME).tex

.PHONY: clean
clean:
	rm -rf relaxation
	rm -rf $(DOCUNAME).aux $(DOCUNAME).log $(DOCUNAME).toc $(DOCUNAME).pdf
