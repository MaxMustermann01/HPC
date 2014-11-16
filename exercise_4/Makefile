#
# Exercise 3 - Makefile
# AUTHORS  Klaus Naumann
#          Christoph Klein
#          Günther Schindler
#
CC = mpicc
CPP = mpic++
CFLAGS = 
TEXC = pdflatex
DOCUNAME = docu_3

.PHONY: mat_mult
mat_mult:
	$(CPP) $(CFLAGS) ./src/main.c ./src/distribution.c ./src/tests.c ./src/parallel_matmult.c ./src/matrix_multiply.c -o ./bin/mat_mult 

.PHONY: docu
docu :
	$(TEXC) $(DOCUNAME).tex

.PHONY: clean
clean:
	rm -rf ./bin/*
    
