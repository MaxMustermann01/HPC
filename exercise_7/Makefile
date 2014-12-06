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

nbody :
	$(CPP) $(CFLAGS)	src/main.cpp \
						inc/nbody.cpp \
						-o nbody

clean :
	rm -f nbody
