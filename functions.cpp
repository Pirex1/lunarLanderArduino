#include <Arduino.h>
#include <Adafruit_ILI9341.h>
#include <TouchScreen.h>
#include <SPI.h>
#include "header.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

//#define PI 3.14159265

#define TFT_DC 9
#define TFT_CS 10
#define SD_CS 6

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

#define THRUSTBUTTON 8
#define FUELPIN 11
#define LEDPIN 13
#define SPEAKERPIN 12

#define DISPLAY_WIDTH  320
#define DISPLAY_HEIGHT 240

#define JOY_VERT  A1
#define JOY_HORIZ A0
#define JOY_SEL 7


#define JOY_CENTER   512
#define JOY_DEADZONE 64

float gravity = 0.01;
int groundLength;

Lander lander;
landingPoints landPoints;
surfacePoints surface;
float thrust;

int randx[20];
int randy[20];
bool space;

int currentScreenX = 0;
int currentScreenY = 0;


void setup(){

	init();

	Serial.begin(9600);

	tft.begin();

	pinMode(THRUSTBUTTON, INPUT_PULLUP);
	pinMode(JOY_SEL, INPUT_PULLUP);
	pinMode(LEDPIN, OUTPUT);
	pinMode(SPEAKERPIN, OUTPUT);
	tft.setRotation(3);

  tft.fillScreen(ILI9341_BLACK);

	menu();

	tft.fillScreen(ILI9341_BLACK);

	tft.setCursor(2,2);
	tft.setTextColor(ILI9341_WHITE);
	tft.setTextSize(1);
	tft.print("Fuel: ");

	Serial.println("Initializing...");


	for(int i = 0; i < 20; i++){
		randx[i] = random(0, DISPLAY_WIDTH);
		randy[i] = random(0, 150);
	}

	space = false;

	drawSurface(currentScreenX,currentScreenY);

	//background();int anaXVal = analogRead(JOY_HORIZ);
	int initalX = DISPLAY_WIDTH;
	int initalY = DISPLAY_HEIGHT/6;

	//specifies starting values of the lander;
	lander.x = initalX;
	lander.y = initalY;
	lander.vx = 0;
	lander.vy = 0;
	lander.ax = 0;
	lander.ay = 0;
	lander.heading = 270;
	lander.score = 0;
	lander.crashed = false;
	lander.landed = false;



	delay(10);

}

void menu(){
	tft.setTextColor(ILI9341_WHITE);
	tft.setCursor(DISPLAY_WIDTH/5 - 20,30);
	tft.setTextSize(3);
	tft.print("Lunar Lander");
	tft.setCursor(DISPLAY_WIDTH/4-10,80);

	tft.setTextSize(1);
	tft.setCursor(DISPLAY_WIDTH/4- 20, 100);
	tft.print("Select difficulty using joystick");

	//code for difficulty selector
	tft.setCursor(35,DISPLAY_HEIGHT-80);
	tft.setTextSize(2);
	tft.setTextColor(ILI9341_GREEN);
	tft.print("EASY");
	tft.setCursor(120,DISPLAY_HEIGHT-80);
	tft.setTextSize(2);
	tft.setTextColor(ILI9341_YELLOW);
	tft.print("MEDIUM");
	tft.setCursor(230,DISPLAY_HEIGHT-80);
	tft.setTextSize(2);
	tft.setTextColor(ILI9341_RED);
	tft.print("HARD");


	int currentDiff = 1;
	int joyY;
	int cursorY = DISPLAY_HEIGHT - 80;
	bool joyButton = true;
	int cursorX[3] = {35,120,229};
	int cursorXEnd[3] = {75,100,75};
	tft.drawRect(cursorX[currentDiff] - 15, cursorY - 15, cursorXEnd[currentDiff],45,ILI9341_WHITE);
	while(joyButton){
		joyButton = digitalRead(JOY_SEL);
		joyY = analogRead(JOY_HORIZ);
		if(joyY > JOY_CENTER + JOY_DEADZONE && currentDiff != 0){
			currentDiff--;
			tft.drawRect(cursorX[currentDiff]-15,cursorY-15,cursorXEnd[currentDiff],45,ILI9341_WHITE);
			tft.drawRect(cursorX[currentDiff+1]-15,cursorY-15,cursorXEnd[currentDiff+1],45,ILI9341_BLACK);
		}
		else if(joyY < JOY_CENTER - JOY_DEADZONE && currentDiff != 2){
			currentDiff++;
			tft.drawRect(cursorX[currentDiff]-15,cursorY-15,cursorXEnd[currentDiff],45,ILI9341_WHITE);
			tft.drawRect(cursorX[currentDiff-1]-15,cursorY-15,cursorXEnd[currentDiff-1],45,ILI9341_BLACK);
		}
		else if(joyButton == false){

			if(currentDiff == 0){
				lander.fuel = 800;
				gravity = 0.01;
				groundLength = 35;
			}
			else if(currentDiff == 1){
				lander.fuel = 500;
				gravity = 0.01;
				groundLength = 30;
			}
			else if(currentDiff == 2){
				lander.fuel = 300;
				gravity = 0.02;
				groundLength = 25;
			}
		}
		delay(100);
	}
	tft.fillScreen(ILI9341_BLACK);
}

void stars(int randx[], int randy[], bool isSpace){
	//uses the random function to generate a 20 stars across the screen
	for(int i = 0; i < 20; i++){

		tft.drawPixel(randx[i],randy[i],ILI9341_WHITE);

	}
	if(isSpace == true){
		for(int i = 19; i >= 0; --i){
			tft.drawPixel(randx[i]-5,randy[i]+DISPLAY_HEIGHT/2,ILI9341_WHITE);
		}
	}

}

void drawSurface(int currentScreenX, int currentScreenY){
	if(currentScreenY == 0){
		randomSeed(currentScreenX);
		int startX = 0;
		int startY = DISPLAY_HEIGHT - 20;
		int endX;
		int endY;
		int yIndex = 0;
		int xIndex = 0;


		for(int i = 0; i < 15; ++i){
			if((i == 4) | (i == 8) | (i == 10)){
				endY = startY;

			}
			else{
				endY = random(DISPLAY_HEIGHT - 80, DISPLAY_HEIGHT - 20);
			}
			endX = startX + random(20, groundLength);
			if(groundLength == 25){
				tft.drawLine(startX, startY, endX, endY, ILI9341_BLUE);
			}
			else{
				tft.drawLine(startX, startY, endX, endY, ILI9341_WHITE);
			}

			if((i == 4) | (i == 8) | (i == 10)){
				landPoints.ylocal[yIndex] = startY;
				yIndex++;

				landPoints.xlocal[xIndex] = startX;
				landPoints.xlocal[xIndex+1]= endX;
				xIndex+=2;
				tft.drawPixel(startX,startY-1,ILI9341_RED);
				tft.drawPixel(endX,endY-1,ILI9341_RED);
				tft.drawPixel(startX,startY-2,ILI9341_RED);
				tft.drawPixel(endX,endY-2,ILI9341_RED);
			}
			addToSurface(startX, startY, endX, endY);
			startX = endX;
			startY = endY;
		}

	}





}

void addToSurface(int startX, int startY, int endX, int endY){
	surface.y[0] = DISPLAY_HEIGHT - 20;
	if(startY == endY){
		for(int i = startX; i <= endX ; ++i){
			surface.x[i] = i;
			surface.y[i] = startY;

		}

	}
	else{
		float rise = endY - startY;
		float run = endX - startX;
		float slope = rise/run;

		for(int i = startX; i < endX && i < DISPLAY_WIDTH; ++i){
			surface.x[i] = i;
			surface.y[i+1] = surface.y[i] + slope;
		}
	}


}


void drawlander(bool colour){
	int usedcol;
	int thrustcol;

	if(colour == false){
 		usedcol = ILI9341_BLACK;
		thrustcol = usedcol;

	}
	else if(colour == true){
		if(groundLength == 25){
			usedcol = ILI9341_RED;
		}
		else{
			usedcol = ILI9341_WHITE;
		}
		thrustcol = 0xFFE0;
	}


	float legRx, legRy, legLx, legLy;
	float endlegRx, endlegRy, endlegLx, endlegLy;
	float jetRx, jetRy, jetLx, jetLy;


	float rad = lander.heading*(PI/180);
	float headingx = 5*cos(rad);//rcostheta
	float headingy = 5*sin(rad);//rsintheta


	legRx = headingx*cos(3*PI/4) + headingy*sin(3*PI/4);
	legRy = -headingx*sin(3*PI/4) + headingy*cos(3*PI/4);

	legLx = headingx*cos(-3*PI/4) + headingy*sin(-3*PI/4);
	legLy = -headingx*sin(-3*PI/4) + headingy*cos(-3*PI/4);

	jetRx = -2*headingx;
	jetLx = jetRx;
	jetRy = -2*headingy;
	jetLy = jetRy;

	endlegRx = 1.5*legRx;
	endlegRy = 1.5*legRy;

	endlegLx = 1.5*legLx;
	endlegLy = 1.5*legLy;


	//redraws each part of the lander
	tft.drawCircle(lander.x/2,lander.y/2,4,usedcol);
	tft.drawLine(lander.x/2+legRx,lander.y/2+legRy,lander.x/2+endlegRx,lander.y/2+endlegRy,usedcol);
	tft.drawLine(lander.x/2+legLx,lander.y/2+legLy,lander.x/2+endlegLx,lander.y/2+endlegLy,usedcol);


	//drawing animation for engine thrust;
	if(thrust != 0 ){
		tft.drawLine(lander.x/2+legRx,lander.y/2+legRy,lander.x/2+jetRx,lander.y/2+jetRy,thrustcol);
		tft.drawLine(lander.x/2+legLx,lander.y/2+legLy,lander.x/2+jetLx,lander.y/2+jetLy,thrustcol);
	}

}



void flylander(){
	//reads input of the joystick
	int anaXVal = analogRead(JOY_HORIZ);
	bool buttonVal = digitalRead(THRUSTBUTTON);

	checkboundaries();
	stars(randx,randy,space);

	drawlander(false);

	if(anaXVal > JOY_CENTER + JOY_DEADZONE ){
		lander.heading+=5;
		if(lander.heading > 360){
			lander.heading = 0;
		}
	}
	else if (anaXVal < JOY_CENTER - JOY_DEADZONE){
		lander.heading-=5;
		if(lander.heading < 0){
			lander.heading = 360;
		}
	}
	else if(lander.heading < 10 && lander.heading > 350){
		lander.heading = 0;
	}

	if(buttonVal == false && lander.fuel != 0){
		thrust = 0.05;
		playTone(80,3);

	}
	else{
		thrust = 0;
	}

	updatelander(thrust);
	safeToLand();
	drawlander(true);
	fuel();
	checkLand();
	if(lander.landed == true){
		endRound();
	}
	else if(lander.crashed == true){
		endGame();
	}
	if(lander.fuel > 200){
		delay(6);
	}
	else{
		delay(5);
	}

}

void updatelander(float thrust){
	float rad = (float)lander.heading*(PI/180);

	//add gravity to y acceleration;

	lander.ax = thrust*cos(rad);

	lander.ay =	thrust*sin(rad) + gravity;

	lander.vx =	lander.vx + lander.ax;

	lander.vy =	lander.vy + lander.ay;

	lander.x = lander.x + lander.vx + (1/2)*lander.ax;

	lander.y =	lander.y + lander.vy +(1/2)*lander.ay;
}

void fuel(){
	tft.fillRect(2,2,60,12,ILI9341_BLACK);
	if(thrust != 0){
		lander.fuel--;

	}
	tft.setTextColor(ILI9341_WHITE);
	tft.setCursor(2,2);
	tft.print("Fuel: ");

	tft.setCursor(35,2);
	if(lander.fuel < 200){
		tft.setTextColor(ILI9341_RED);
		digitalWrite(LEDPIN, HIGH);
		playTone(800,8);
	}
	else{
		tft.setTextColor(ILI9341_WHITE);
		digitalWrite(LEDPIN,LOW);
	}
	tft.print(lander.fuel);
}

void checkboundaries(){
	currentScreenY = constrain(currentScreenY,0,3);
	currentScreenX = constrain(currentScreenX,-5,5);

	if(lander.y/2 < 0){
		tft.fillScreen(ILI9341_BLACK);
		space = true;
		//move screen up aka remove surface
		currentScreenY++;
		lander.y = 2*DISPLAY_HEIGHT;
		drawSurface(currentScreenX,currentScreenY);
	}
	else if(lander.y > 2*DISPLAY_HEIGHT && currentScreenY != 0){
		tft.fillScreen(ILI9341_BLACK);
		space = false;
		//move screen down
		currentScreenY--;
		lander.y = 0;
		drawSurface(currentScreenX,currentScreenY);
	}
	else if (lander.x/2 < 0){
		tft.fillScreen(ILI9341_BLACK);
		//move screen to the left
		currentScreenX--;
		lander.x = 2*DISPLAY_WIDTH;
		drawSurface(currentScreenX,currentScreenY);
	}
	else if(lander.x/2 > DISPLAY_WIDTH){
		tft.fillScreen(ILI9341_BLACK);
		currentScreenX++;
		lander.x = 0;
		drawSurface(currentScreenX,currentScreenY);
	}
}

void checkLand(){
	float landRx, landRy, landLx, landLy;
	float crashRx, crashRy, crashLx, crashLy;
	float crashTx, crashTy, crashBx, crashBy;


	float rad = lander.heading*(PI/180);
	float headingx = 5*cos(rad);//rcostheta
	float headingy = 5*sin(rad);//rsintheta

	//these points match the end points of the lander legs
	landRx = lander.x/2 + 1.5*(headingx*cos(3*PI/4) + headingy*sin(3*PI/4));
	landRy = lander.y/2 + 1.5*(-headingx*sin(3*PI/4) + headingy*cos(3*PI/4));

	landLx = lander.x/2 + 1.5*(headingx*cos(-3*PI/4) + headingy*sin(-3*PI/4));
	landLy = lander.y/2 + 1.5*(-headingx*sin(-3*PI/4) + headingy*cos(-3*PI/4));

	crashRy = lander.y/2 ;
	crashRx =	lander.x/2 + 2;
	crashLx = lander.x/2 - 2;
	crashLy = lander.y/2;
	crashTy = lander.y/2 - 2;
	crashTx = lander.x/2;
	crashBx = lander.x/2;
	crashBy = lander.y/2 + 2;


	int mapIndexIni =	lander.x/2 - 5;
	int mapIndexFin = mapIndexIni + 10;
	if(currentScreenY == 0){
		if(lander.vy <= 0.80 && lander.vx <= 0.60 && lander.vx >=-0.60){

			for(int i = 0; i < 3; ++i){
				if((int)landRy >= landPoints.ylocal[i] && (int)landLy >= landPoints.ylocal[i]){
					if((int)landRx >= landPoints.xlocal[2*i] && (int)landLx <= landPoints.xlocal[2*i+1]){
						lander.landed = true;
					}
				}
			}
		}

		for(int i = mapIndexIni; i <= mapIndexFin; ++i){
			if(((int)crashRx == surface.x[i] && (int)crashRy >= surface.y[i] | (int)crashLx == surface.x[i] && (int)crashLy >= surface.y[i])){
				if(lander.landed != true){
					lander.crashed = true;
				}
			}
			else if(((int)crashBx == surface.x[i] && (int)crashBy >= (int)surface.y[i] | (int)crashTx == surface.x[i] && (int)crashTy >= surface.y[i])){
				if(lander.landed != true){
					lander.crashed = true;
				}
			}

		}
	}

}

void endRound(){
	lander.fuel +=200;
	lander.x = DISPLAY_WIDTH/2;
	lander.y = 50;
	lander.vx = 0;
	lander.vy = 0;
	lander.ax = 0;
	lander.ay = 0;
	lander.score++;
	lander.heading = 270;
	lander.crashed = false;
	lander.landed = false;
	tft.setTextColor(ILI9341_GREEN);
	tft.setTextSize(2);
	tft.setCursor(DISPLAY_WIDTH/2- 50,DISPLAY_HEIGHT/2 - 20);
	tft.print("landed!");
	tft.setCursor(DISPLAY_WIDTH/2,DISPLAY_HEIGHT/2 + 30);
	tft.setTextSize(1);
	tft.print("Total Score: ");
	tft.print(lander.score);
	delay(3000);
	tft.fillScreen(ILI9341_BLACK);
	tft.setTextSize(1);
	currentScreenX = random(-5,5);
	currentScreenY = 0;
	drawSurface(currentScreenX,currentScreenY);

}

void endGame(){
	tft.setTextColor(ILI9341_RED);
	tft.setTextSize(2);
	for(int i = 0; i < 3; ++i){
		tft.setTextColor(ILI9341_RED);
		tft.setCursor(DISPLAY_WIDTH/2 - 50 ,DISPLAY_HEIGHT/2 - 20);
		tft.print("GAME OVER");
		delay(800);
		tft.setTextColor(ILI9341_BLACK);
		tft.setCursor(DISPLAY_WIDTH/2-50,DISPLAY_HEIGHT/2 - 20);
		tft.print("GAME OVER");
		delay(500);
	}
	delay(800);
	tft.setTextColor(ILI9341_WHITE);
	tft.setCursor(DISPLAY_WIDTH/2-50,DISPLAY_HEIGHT/2);
	tft.setTextSize(1);
	tft.print("Score: ");
	tft.print(lander.score);
	lander.x = DISPLAY_WIDTH/2;
	lander.y = 50;
	lander.vx = 0;
	lander.vy = 0;
	lander.ax = 0;
	lander.ay = 0;
	lander.score = 0;
	lander.heading = 270;
	lander.crashed = false;
	lander.landed = false;
	delay(2000);
	tft.fillScreen(ILI9341_BLACK);
	menu();
	tft.setTextSize(1);
	currentScreenX = 0;
	currentScreenY = 0;
	drawSurface(currentScreenX,currentScreenY);

}


void safeToLand(){
	if(lander.vy <= 0.60 && lander.vx <= 0.30 && lander.vx >=-0.30){
		tft.setTextSize(1);
		tft.setCursor(120,10);
		tft.setTextColor(ILI9341_GREEN);
		tft.print("Safe to land");
	}
	else{
		tft.setTextSize(1);
		tft.setCursor(120,10);
		tft.setTextColor(ILI9341_BLACK);
		tft.print("Safe to land");
		tft.fillRect(DISPLAY_WIDTH+50,2,50,10,ILI9341_BLACK);
	}

}

void playTone(int period, int duration){
	long elaspedTime = 0;
	int halfPeriod = period / 2;
	while(elaspedTime < duration *1000L){
		digitalWrite(SPEAKERPIN, HIGH);
		delayMicroseconds(halfPeriod);

		digitalWrite(SPEAKERPIN, LOW);
		delayMicroseconds(halfPeriod);

		elaspedTime = elaspedTime + period;
	}
}
