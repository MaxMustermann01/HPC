#
#   Exercise 10  Makefile
#
#   AUTHORS     Günther Schindler
#               Klaus Naumann
#               Christoph Klein
#

DOCUNAME = docu_10
TEXC = pdflatex
FLAGS = -arch=sm_20 -O2 --compiler-options "-O2 -Wall -Wextra"

.PHONY: build
build: ./bin/matMul

.PHONY: clean
clean:
	rm -f ./bin/*
	rm -f   $(DOCUNAME).pdf $(DOCUNAME).toc $(DOCUNAME).aux $(DOCUNAME).log

.PHONY: rebuild
rebuild: clean build

./bin/matMul: ./src/main.cu
	nvcc $(FLAGS) -o ./bin/matMul ./src/main.cu ./src/mmult_cpu.c -I./inc/

.PHONY: docu
docu:
	$(TEXC) ./docu/$(DOCUNAME).tex

