#ifndef HEADER_H
#define FUNCTIONS_H

class Lander{
public:
	float vx, vy;
	float ax, ay;
	float x,y;
	float heading;
	bool landed;
	bool crashed;
	int fuel;
	int score;
};



struct landingPoints{
	int xlocal[6];
	int ylocal[3];
};

struct surfacePoints{
	float x[320];
	float y[320];
};


void setup();
void menu();
void stars(int randx[], int randy[], bool isSpace);
void drawlander(bool colour);
void flylander();
void drawSurface(int currentScreenX, int currentScreenY);
void addToSurface(int startX, int startY, int endX, int endY);
void updatelander(float thrust);
void fuel();
void checkboundaries();
void endRound();
void checkLand();
void endGame();
void safeToLand();
void playTone(int period, int duration);
#endif
