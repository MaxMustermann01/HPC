#*************************************************************************************************
#
# Heidelberg University - IntroHPC Exercise 07
#
# Group :		IntroHPC03
# Participant : Klaus Naumann
#				Christoph Klein
#				Günther Schindler
#
# File : 		Makefile
#
# Purpose : 	N-Body Simulation (3D)
#
#*************************************************************************************************

CPP = g++
CFLAGS = -Wall
CLATEX = pdflatex
DOCUNAME = docu_7

nbody :
	$(CPP) $(CFLAGS)	src/main.cpp \
						inc/nbody.cpp \
						-o ./bin/nbody

clean :
	rm -f ./bin/nbody
	rm -f  $(DOCUNAME).pdf $(DOCUNAME).log $(DOCUNAME).aux

.PHONY: docu
docu:
	$(CLATEX) ./docu/$(DOCUNAME).tex
