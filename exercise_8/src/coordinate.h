#ifndef COORDINATES_H
#define COORDINATES_H

#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <cstring>
#include "nbody.hpp"
using namespace std;

#define MAX_CIRCLES 100			// default value
#define MAX_TIMESTEPS 10000		// default value
								// d.h. jeder Kreis ändert 5 mal die Position
								// innerhalb der Animation

#define FILE_IS_OPEN 1
#define FILE_NOT_EXIST 0
#define NBODY_NOT_EXIST 0
#define NBODY_OK 1

// NBODY = floating coodrinates
// Size of NBODY = 4 x 4 = 16Bytes/128Bits
typedef struct
{
	int circle;		// nbody index
	double xCord;
	double yCord;
	double zCord;
	double mass;
	double xPos;
	double yPos;
	double zPos;
}NBODY;

#ifndef CORDSIZE
#define CORDSIZE
typedef struct
{
	int xLength;
	int yLength;
	int zLength;
}COORDINATE_SIZE;
#endif


class coordinate
{
private:
	char* filename;
	fstream writefile;
	int fileflag;

	NBODY **pNbodys;	// TODO: nbodys dynamisch anlegen siehe MAX_POSITIONS
//	NBODY aNbodys[1][MAX_CIRCLES];
	int sizeof_nbodys;
	int cordflag;
	int maxcircles;
	int maxtimesteps;
	int maxmass;
	int minmass;
	COORDINATE_SIZE cSize;

	bool create_nbodys(void);
	bool delete_nbodys(void);

	bool open_writefile(void);
	bool close_writefile(void);
	int init_writefile();

public:
	coordinate();
	coordinate(const char* filename);
	coordinate(int circles);
	coordinate(int circles, int timesteps);
	coordinate(const char* filename, int circles);
	coordinate(const char* filename, int circles, int timesteps, int maxmass, int minmass, COORDINATE_SIZE cSize, bool openFile);
	~coordinate();

	int initMass(int circleIdx, double bodymass);
	int writeSingleCord(int circleIdx);
	int writeSingleCord(int circleIdx, double xCord, double yCord, double zCord);
	int writeTimestep(sList list);
	void writeEndl(void);

//	int setCords(int circleIdx, float xCord, float yCord, float zCord);
//	int setCords(int circleIdx, int posIdx, float xCord, float yCord, float zCord);
//	int createTestCords(void);
//	int open();
//	int isopen();
//	int close();
};

#endif
