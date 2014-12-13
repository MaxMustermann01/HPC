#*************************************************************************************************
#
# Heidelberg University - IntroHPC Exercise 08
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

CPP = mpic++
CFLAGS = -Wall -std=c++0x
CLATEX = pdflatex
DOCUNAME = docu_8

nbody :
	$(CPP) $(CFLAGS)	src/main.cpp \
						src/nbody.cpp \
                        src/communication.cpp \
                        src/coordinate.cpp \
                        src/time_measurement.c \
						-o ./bin/nbody

clean :
	rm -f ./bin/nbody
	rm -f  $(DOCUNAME).pdf $(DOCUNAME).log $(DOCUNAME).aux
	rm -f ./output.bin    

.PHONY: docu
docu:
	$(CLATEX) ./docu/$(DOCUNAME).tex
