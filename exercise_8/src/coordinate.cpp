#include "coordinate.h"
#include "nbody.hpp"

using namespace std;

bool coordinate::open_writefile(void)
{
	/** Öffnen der Datei **/
	cout << "coordinate::open_writefile(): opening " << this->filename << "... ";
	this->writefile.open(filename,  ios_base::in | ios_base::out | ios_base::binary | ios_base::trunc);
	if(this->writefile.is_open() == true)
	{
		cout << "successfully opened!" << endl;
		this->fileflag = FILE_IS_OPEN;
		init_writefile();
		return true;
	}
	else
	{
		cout << "could not open file!" << endl;
		this->fileflag = FILE_NOT_EXIST;
		return false;
	}
}

int coordinate::init_writefile()
{
	if(this->fileflag)
	{
		// maximale Anzahl an Circles und Steps werden in Datei geschrieben
		writefile.write(reinterpret_cast<const char*>(&maxcircles), sizeof(maxcircles));
		writefile.write(reinterpret_cast<const char*>(&maxtimesteps), sizeof(maxtimesteps));
		writefile.write(reinterpret_cast<const char*>(&maxmass), sizeof(maxmass));
		writefile.write(reinterpret_cast<const char*>(&minmass), sizeof(minmass));
		writefile.write(reinterpret_cast<const char*>(&cSize), sizeof(COORDINATE_SIZE));
		writefile << '\n';
		return 1;
	}
	else
	{
		/** Fehlerfall **/
		cout << "write(): Datei exestiert nicht..." << endl;
		return -1;
	}
}

int coordinate::initMass(int circleIdx, double bodymass)
{
	if(this->fileflag)
	{
		if(circleIdx < (maxcircles-1))
		{
			// maximale Anzahl an Circles und Steps werden in Datei geschrieben
			writefile.write(reinterpret_cast<const char*>(&bodymass), sizeof(double));
			return 1;
		}
		else
		{
			// letze Masse gefolgt vom 'newline'
			writefile.write(reinterpret_cast<const char*>(&bodymass), sizeof(double));
			writefile << '\n';
			return 1;
		}
	}
	else
	{
		/** Fehlerfall **/
		cout << "initMass(): Datei exestiert nicht..." << endl;
		return -1;
	}
}

bool coordinate::create_nbodys(void)
{
	cout << "coordinate::create_nbodys(): allocating nbodys...";

	this->pNbodys = new NBODY*[maxtimesteps];

	for(int i = 0; i < maxtimesteps;i++)
	{
		this->pNbodys[i] = new NBODY[maxcircles];
	}
	pNbodys[maxtimesteps-1][maxcircles-1].circle = 1234;

	if(pNbodys[maxtimesteps-1][maxcircles-1].circle == 1234)
	{
		this->cordflag = NBODY_OK;
		cout << "successfully allocated!" << endl;
		return 1;
	}
	else
	{
		cout << "could not allocate nbodys!" << endl;
		return 0;
	}
}

bool coordinate::delete_nbodys(void)
{
	cout << "coordinate::delete_nbodys(): deleting nbodys...";
	for(int i = 0; i < maxtimesteps; i++)
	{
		delete [] pNbodys[i];
		pNbodys[i] = NULL;
	}
	delete [] pNbodys;
	pNbodys = NULL;
	cout << " successfully deleted!" << endl;
	return true;
}

coordinate::coordinate()
{
	// set default values
	this->filename = NULL;
	this->cordflag = NBODY_NOT_EXIST;
	sizeof_nbodys = sizeof(NBODY);
	maxcircles = MAX_CIRCLES;
	maxtimesteps = MAX_TIMESTEPS;

	/** Dateiname erstellen **/
	cout << "coordinate::coordinate(): allocating filename... ";
	filename = new char[strlen("default.bin") + 1];
	if(filename == NULL)
	{
		cout << "could not allocate!" << endl;
	}
	else
	{
		cout << "successfully allocated!" << endl;
		
		/* Dateiname beschreiben**/
		strncpy(filename, "default.bin", strlen("default.bin"));

		/** Öffnen der Datei **/
		open_writefile();
	}
}

coordinate::coordinate(int circles)
{
	// set default values
	this->filename = NULL;
	this->cordflag = NBODY_NOT_EXIST;
	sizeof_nbodys = sizeof(NBODY);

	// check border values of maxcircles ( value area of 0 < maxcircles < 2048)
	maxcircles = circles;
	if(circles < 1)
	{
		cout << "maxcircles out of range (0 < maxcircles < 1000000)... setting maxcircles to 1!" << endl;
		maxcircles = 1;	
	}
	if(circles > 1000000)
	{
		cout << "maxcircles out of range (0 < maxcircles < 1000000)... setting maxcircles to 1000000!" << endl;
		maxcircles = 1000000;	
	}
	maxtimesteps = MAX_TIMESTEPS;

	/** Dateiname erstellen **/
	cout << "coordinate::coordinate(): allocating filename... ";
	filename = new char[strlen("default.bin") + 1];
	if(filename == NULL)
	{
		cout << "could not allocate!" << endl;
	}
	else
	{
		cout << "successfully allocated!" << endl;
		/* Dateiname beschreiben**/
		strcpy(filename, "default.bin");

		/** Öffnen der Datei **/
		open_writefile();
	}
}

coordinate::coordinate(const char* filename, int circles)
{
	// set default values
	sizeof_nbodys = sizeof(NBODY);
	this->filename = NULL;
	
	// check border values of maxcircles ( value area of 0 < maxcircles < 2048)
	maxcircles = circles;
	if(circles < 1)
	{
		cout << "maxcircles out of range (0 < maxcircles < 2048)... setting maxcircles to 1!" << endl;
		maxcircles = 1;	
	}
	if(circles > 1000000)
	{
		cout << "maxcircles out of range (0 < maxcircles < 1000000)... setting maxcircles to 1000000!" << endl;
		maxcircles = 1000000;	
	}
	maxtimesteps = MAX_TIMESTEPS;	

	/** Dateiname erstellen **/
	if(strlen(filename) < 1)	// filename ist leer
	{
		/** FILENAME UNGÜLTIG **/
		cout << "coordinate::coordinate(): given filename (arg1) not exists... using 'default.bin' as filename" << endl;

		cout << "coordinate::coordinate(): allocating filename... " << endl;
		this->filename = new char[strlen("default.bin") + 1];
		strncpy(this->filename, "default.bin", sizeof("default.bin"));	
		if(this->filename == NULL)
		{
			cout << "could not allocate!" << endl;
			return;
		}
		
		cout << "successfully allocated!" << endl;

		/** Dateiname beschreiben **/
		strncpy(this->filename, filename, strlen(filename));

		/** Öffnen der Datei **/
		open_writefile();
	}
	else
	{
		/** FILENAME GÜLTIG **/
		cout << "coordinate::coordinate(): allocating filename... ";
		this->filename = new char[strlen(filename) + 1];	
		if(this->filename == NULL)
		{
			cout << "could not allocate!" << endl;
			return;
		}
		

		cout << "successfully allocated!" << endl;

		/** Dateiname beschreiben **/
		strncpy(this->filename, filename, strlen(filename));

		/** Öffnen der Datei **/
		open_writefile();		
	}
}

coordinate::coordinate(const char* filename, int circles, int timesteps, int maxmass, int minmass, COORDINATE_SIZE cSize, bool openFile)
{
	sizeof_nbodys = sizeof(NBODY);
	this->filename = NULL;

	this->maxmass = maxmass;
	this->minmass = minmass;
	this->cSize.xLength = cSize.xLength;
	this->cSize.yLength = cSize.yLength;
	this->cSize.zLength = cSize.zLength;
	
	// check border values of maxcircles ( value area of 0 < maxcircles < 1000000)
	maxcircles = circles;
	if(circles < 1)
	{
		cout << "maxcircles out of range (0 < maxcircles < 1000000)... setting maxcircles to 1!" << endl;
		maxcircles = 1;	
	}
	if(circles > 1000000)
	{
		cout << "maxcircles out of range (0 < maxcircles < 1000000)... setting maxcircles to 1000000!" << endl;
		maxcircles = 1000000;	
	}
	// check border values of maxcircles ( value area of 0 < maxcircles < 1000)
	maxtimesteps = timesteps;
	if(timesteps < 1)
	{
		cout << "maxcircles out of range (0 < maxtimesteps < 50000)... setting maxcircles to 1!" << endl;
		maxtimesteps = 1;	
	}
	if(timesteps > 50000)
	{
		cout << "maxcircles out of range (0 < maxtimesteps < 50000)... setting maxcircles to 50000!" << endl;
		maxtimesteps = 50000;	
	}

/*	Entfällt da Koordinaten direkte nach der Berechnung in die Datei geschrieben werden
	if(!create_nbodys())
	{
		return;
	}
	*/

	/** Dateiname erstellen **/
	if(strlen(filename) < 1) // filename ist leer
	{
		/** FILENAME UNGÜLTIG **/
		cout << "coordinate::coordinate(4): given filename (arg1) not exists... using 'writefile.bin' as filename" << endl;

		cout << "coordinate::coordinate(4): allocating filename... " << endl;
		this->filename = new char[strlen("writefile.bin") + 1];
		strncpy(this->filename, "writefile.bin", sizeof("writefile.bin"));
		if(this->filename == NULL)
		{
			cout << "could not allocate!" << endl;
			return;
		}
		
		cout << "successfully allocated!" << endl;

		/** Dateiname beschreiben **/
		strncpy(this->filename, filename, strlen(filename));

		/** Öffnen der Datei **/
		open_writefile();
	}
	else
	{
		cout << "coordinate::coordinate(4): allocating filename... ";
		this->filename = new char[strlen(filename) + 1];	
		if(this->filename == NULL)
		{
			cout << "could not allocate!" << endl;
			return;
		}
		

		cout << "successfully allocated!" << endl;

		/** Dateiname beschreiben **/
		strncpy(this->filename, filename, strlen(filename));

		/** Öffnen der Datei **/
		if (openFile) open_writefile();		
	}	
}

coordinate::~coordinate()
{
//	delete_nbodys();

	cout << "coordinate::~coordinate(): " << this->filename << " wird geschlossen..." << endl;
	if(writefile.is_open() == true)
	{
		writefile.close();
		cout << "coordinate::~coordinate(): " << this->filename << " erfolgreich geschlossen..." << endl;
	}
	else
	{
		cout << "coordinate::~coordinate(): Es wurde keine Datei angelegt..." << endl;
	} 
}

int coordinate::writeSingleCord(int circleIdx)
{
	if(this->fileflag)
	{
//		writefile << circle << "," << xCord << "," << yCord << "," << zCord << ":";
		return 1;
	}
	else
	{
		/** Fehlerfall **/
		cout << "write(): Datei exestiert nicht..." << endl;
		return -1;
	}
}

int coordinate::writeSingleCord(int circleIdx, double xCord, double yCord, double zCord)
{
	if(this->fileflag)
	{
		writefile.write(reinterpret_cast<const char*>(&circleIdx), sizeof(circleIdx));
		writefile.write(reinterpret_cast<const char*>(&xCord), sizeof(xCord));
		writefile.write(reinterpret_cast<const char*>(&yCord), sizeof(yCord));
		writefile.write(reinterpret_cast<const char*>(&zCord), sizeof(zCord));
		return 1;
	}
	else
	{
		/** Fehlerfall **/
		cout << "write(): Datei exestiert nicht..." << endl;
		return -1;
	}	
}

int coordinate::writeTimestep(sList list)
{
	if(this->fileflag)
	{
		for(int i = 0; i < list.size; i++)
		{
			writefile.write(reinterpret_cast<const char*>(&i), sizeof(int));
			writefile.write(reinterpret_cast<const char*>(&(list.pB[i].dPx)), sizeof(double));
			writefile.write(reinterpret_cast<const char*>(&(list.pB[i].dPy)), sizeof(double));
			writefile.write(reinterpret_cast<const char*>(&(list.pB[i].dPz)), sizeof(double));
		}
		return 1;
	}
	else
	{
		/** Fehlerfall **/
		cout << "write(): Datei exestiert nicht..." << endl;
		return -1;
	}	
}

void coordinate::writeEndl(void)
{
	writefile << "\n";
}
/*
int coordinate::setCords(int circleIdx, float xCord, float yCord, float zCord)
{
	if(this->cordflag)
	{
		this->pNbodys[0][circleIdx].circle = circleIdx;
		this->pNbodys[0][circleIdx].xCord = xCord;
		this->pNbodys[0][circleIdx].yCord = yCord;
		this->pNbodys[0][circleIdx].zCord = zCord;
		return 1;
	}
	else
	{
		cout << "pNbodys Array exestiert nicht!..." << endl;
		return -1;
	}
}

int coordinate::setCords(int circleIdx, int posIdx, float xCord, float yCord, float zCord)
{
	if(this->cordflag)
	{
		this->pNbodys[posIdx][circleIdx].circle = circleIdx;
		this->pNbodys[posIdx][circleIdx].xCord = xCord;
		this->pNbodys[posIdx][circleIdx].yCord = yCord;
		this->pNbodys[posIdx][circleIdx].zCord = zCord;
		return 1;
	}
	else
	{
		cout << "pNbodys Array exestiert nicht!..." << endl;
		return -1;
	}
}


#define ALPHA_STEP (360 / (float)MAX_CIRCLES)
#define PI 3.14159265
#define rad(X) ((float)PI/180.0*X)

int coordinate::createTestCords(void)
{
	NBODY testcord;
	
	float radius = 3.0;
	float alpha = 0.0;
	float step = rad(360.0/(float)maxcircles);
	
	int circleIdx = 0;
	int positionIdx = 0;

	cout << "maxcircles: " << maxcircles << endl;
	cout << "step: " << step << endl;

	// Startkoordinaten anlegen
	for(testcord.circle = 0; testcord.circle < this->maxcircles; testcord.circle++)
	{
		testcord.xCord = sin(alpha) * radius;	// errechnet absolute y Coordinate
		testcord.yCord = cos(alpha) * radius;	// errechnet absolute x Coordinate
		testcord.zCord = -30;
		writefile.write(reinterpret_cast<const char*>(&testcord), sizeof(NBODY));
		alpha += step;
	}
//	writefile << "\n";


	// Bewegungskoordinaten anlegen
	radius = 3.2;
	for(positionIdx = 0; positionIdx < this->maxtimesteps; positionIdx++)
	{
		for(testcord.circle = 0; testcord.circle < this->maxcircles; testcord.circle++)
		{
			testcord.xCord = sin(alpha) * radius;	// errechnet absolute y Coordinate
			testcord.yCord = cos(alpha) * radius;	// errechnet absolute x Coordinate
//			testcord.zCord = 0.5;
			writefile.write(reinterpret_cast<const char*>(&testcord), sizeof(NBODY));
			alpha += step;
		}
//		writefile << "\n";
		radius += 0.1;		// Kreise driften auseinander
	}
	return 1;
}


int coordinate::open()
{
	return 0;
}

int coordinate::isopen()
{
	return 0;
}

int coordinate::close()
{
	return 0;
}
*/
