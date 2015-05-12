#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include "V:\Andrew&Kenneth\Kenneth\CS120Project\headerFiles\bit.h"
#include "V:\Andrew&Kenneth\Kenneth\CS120Project\headerFiles\nokia5100LCD.c"
#include "V:\Andrew&Kenneth\Kenneth\CS120Project\headerFiles\gameGrid.c"

#define bool unsigned char

unsigned char snapshot = 0x00;
unsigned short bar = 0;
unsigned char buttonHeld = 0;
unsigned char waitForConfirmation = 0;
bool gameStart = 0;
bool gameisOver = 0;
bool invis = 0;
bool softReset = 0;

unsigned short starPower = 0;

unsigned char mainGrid [13][7];
bool objectsOnGrid[2] = {0, 0}; //[0] = diamond, [1] apple
	
short playerPosX = -1;
short playerPosY = -1;
	
short applePosX = -1;
short applePosY = -1;

short diamondPosX = -1;
short diamondPosY = -1;

short obs1PosX = -1;
short obs1PosY = -1;
short obs1Dirc = -1;

short obs2PosX = -1;
short obs2PosY = -1;
short obs2Dirc = -1;

short obs3PosX = -1;
short obs3PosY = -1;
short obs3Dirc = -1;


short fruitTimer = 0;
short obsTimer = 0;
short starPowerTimer = 0;

volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerOn() {
	TCCR1B = 0x0B;
	OCR1A = 125;
	TIMSK1 = 0x02;
	TCNT1 = 0;
	_avr_timer_cntcurr = _avr_timer_M;
	SREG |= 0x80;
}

void TimerOff() {
	TCCR1B = 0x00;
}

void TimerISR() {
	TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect) {
	_avr_timer_cntcurr--;
	if (_avr_timer_cntcurr == 0) {
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

// this is where it all happens as far as grabbing the NES control pad data
void GetNESControllerData() {

	PORTC = 0x04; //set NESLatch to 1
	PORTC = 0x00; //set NESLatch to 0
	
	snapshot = 0x00;
	for (unsigned char i = 0; i <= 7; ++i) {
		snapshot = (~PINC & 0x01) << i | snapshot;
		PORTC = 0x02; //set NESClock to 1
		PORTC = 0x00; //set NESClock to 0
	}
}

void incrementBar() {
	if (starPower == 8)
		return;
		
	else {
		++starPower;
		bar = (bar >> 1) | 0x80;
		PORTD = bar;
	}

		
/*	PORTC = (PORTC | 0xC0) & (bar >> 8) */
}

void decrementBar() {
	if (starPower == 0)
		return;
	else {
		--starPower;
	}
	bar = (bar << 1);
	PORTD = bar;
}

void resetBarTo0() {
	bar = 0;
	PORTD = bar;
}

void drawBox() {
	for (unsigned char x = 0; x < 84; ++x) {
		updatePixel(x, 2, 1);
		updatePixel(x, 45, 1);
	}
	
	for (unsigned char y = 0; y < 48; ++y) {
		updatePixel(2, y, 1);
		updatePixel(81, y, 1);
	}
	
	updatePixel(0,0,1);
	updatePixel(1,0,1);
	updatePixel(0,1,1);
	updatePixel(1,1,1);
	
	updatePixel(82,0,1);
	updatePixel(83,0,1);
	updatePixel(82,1,1);
	updatePixel(83,1,1);
	
	updatePixel(82,46,1);
	updatePixel(83,46,1);
	updatePixel(82,47,1);
	updatePixel(83,47,1);
	
	updatePixel(0,46,1);
	updatePixel(0,47,1);
	updatePixel(1,46,1);
	updatePixel(1,47,1);
}

void NESControllerTick() {
// 	NES_A       B00000001
// 	NES_B       B00000010
//  NES_SELECT  B00000100
// 	NES_START   B00001000
// 	NES_UP      B00010000
// 	NES_DOWN    B00100000
// 	NES_LEFT    B01000000
// 	NES_RIGHT   B10000000
	GetNESControllerData();
	if (snapshot == 0x00 && buttonHeld) {
		buttonHeld = 0;
	}
	
	if(snapshot == 0x80 && !buttonHeld) { //Right is pressed
		if(invis)
			moveObjectRight(&playerPosX, &playerPosY, starPlayer, 1);
		else
			moveObjectRight(&playerPosX, &playerPosY, player, 1);
		buttonHeld = 1;
	}
	else if(snapshot == 0x10 && !buttonHeld) { //Up is pressed
		buttonHeld = 1;
		if(invis)
			moveObjectUp(&playerPosX, &playerPosY, starPlayer, 1);
		else
			moveObjectUp(&playerPosX, &playerPosY, player, 1);
	}
	else if(snapshot == 0x40 && !buttonHeld) { //Left is pressed
		buttonHeld = 1;
		if(invis)
			moveObjectLeft(&playerPosX, &playerPosY, starPlayer, 1);
		else
			moveObjectLeft(&playerPosX, &playerPosY, player, 1);
	}
	else if(snapshot == 0x20 && !buttonHeld) { //Down is pressed
		buttonHeld = 1;
		if(invis)
			moveObjectDown(&playerPosX, &playerPosY, starPlayer, 1);
		else
			moveObjectDown(&playerPosX, &playerPosY, player, 1);
	}
	else if(snapshot == 0x08 && !buttonHeld) { //START is pressed
		buttonHeld = 1;
		if(!gameStart)
			sendStart();
	}
	else if(snapshot == 0x01 && !buttonHeld) { //A is pressed
		buttonHeld = 1;
		if(starPower == 8){
			sendStarPower();
			invis = 1;
		}
	}
	else if(snapshot == 0x04 && !buttonHeld) { //Select is pressed
		buttonHeld = 1;
		softReset = 1;
	}
}

void waitForAck(unsigned char c) {
	while(1) {
		if((PINB & 0x10) == 0x10)
		{
			PORTB = 0;
			break;
		}
	}
	if(c == 'P')
	{}
	if(c == 'E')
	{}
	if(c == 'S'){
		gameInit();
		gameStart = 1;
	}
	if(c == 'G'){
		gameStart = 0;
		gameisOver = 1;
	}
	if(c == 'R'){
		gameStart = 0;
		gameisOver = 1;
	}
	if(c == '1'){
		if(!invis)
			incrementBar();
	}
	if(c == '3'){
		if(!invis)
			for (unsigned char i = 0; i < 3; ++i)
				incrementBar();
	}
	PORTB = 0x00;
}

void sendEndStarPower(){
	PORTB = 0x05;
	waitForAck('E');
}

void sendStarPower(){
	PORTB = 0x04;
	waitForAck('P');
}

void sendStart() {
	PORTB = 0x0F;
	waitForAck('S');
};

void sendGameOver(){
	PORTB = 0x03;
	waitForAck('G');
}

void sendReset(){
	PORTB = 0x0B;
	waitForAck('R');
}

void sendThreeScore() {
	PORTB = 0x02;
	waitForAck('3');
}

void sendOneScore() {
	PORTB = 0x01;
	waitForAck('1');
};

void moveObjectRight (short *currentGridX, short *currentGridY, short object[], bool isPlayer) {
	if (isPlayer && *currentGridX == 12)
		return;
	else {
		drawObject(*currentGridX, *currentGridY, emptyCell);
		if (*currentGridX < 12) 
			drawObject(++*currentGridX, *currentGridY, object);
		else {
			*currentGridX = -1;
			*currentGridY = -1;
		}
	}
}

void moveObjectLeft (short *currentGridX, short *currentGridY, short object[], bool isPlayer) {
	if (isPlayer && *currentGridX == 0)
		return;
	else {
		drawObject(*currentGridX, *currentGridY, emptyCell);
		if (*currentGridX > 0)
			drawObject(--*currentGridX, *currentGridY, object);
		else {
			*currentGridX = -1;
			*currentGridY = -1;
		}
	}
}

void moveObjectDown (short *currentGridX, short *currentGridY, short object[], bool isPlayer) {
	if (isPlayer && *currentGridY == 6)
		return;
	else {
		drawObject(*currentGridX, *currentGridY, emptyCell);
		if (*currentGridY < 6) 
			drawObject(*currentGridX, ++*currentGridY, object);
		else {
			*currentGridX = -1;
			*currentGridY = -1;
		}
	}
}

void moveObjectUp (short *currentGridX, short *currentGridY, short object[], bool isPlayer) {
	if (isPlayer && *currentGridY == 0)
		return;
	else {
		drawObject(*currentGridX, *currentGridY, emptyCell);
		if (*currentGridY > 0)
			drawObject(*currentGridX, --*currentGridY, object);
		else {
			*currentGridX = -1;
			*currentGridY = -1;
		}
	} 
}

void gameInit(bool reset) {
	drawBox();
	gameStart = 1;
	//center the player
	playerPosX = 6;
	playerPosY = 3;
	drawObject(playerPosX, playerPosY, player);
}

void throwFruit() {
	//0 - Diamond
	//1-7 - Apple
	int objectNum = rand() % 8;
	
	for(unsigned char x = 0; x < 2; ++x) {//if apple or diamond is already on grid
		if(objectsOnGrid[x] == 1)
			return;
	}
	
	fruitTimer = (fruitTimer + 1) % 1000;
	if(!fruitTimer) {
		unsigned char randX = rand() % 13;
		unsigned char randY = rand() % 7;
		if(objectNum == 0) {
			drawObject(randX, randY, diamond);
			diamondPosX = randX;
			diamondPosY = randY;
			objectsOnGrid[0] = 1;
		}		
		else {
			drawObject(randX, randY, apple);
			applePosX = randX;
			applePosY = randY;
			objectsOnGrid[1] = 1;
		}
	}
}

void throwNewObstacles() {
	if (obs1PosX == -1 && obs1PosY == -1) {
		short direction1 = rand() % 4;
		if (direction1 == 0) { //from left side, direction is 0
			obs1PosX = 0;
			obs1PosY = rand() % 7;
			obs1Dirc = 0;
		}
		else if (direction1 == 1) { //from top side, direction is 1
			obs1PosX = rand() % 13;
			obs1PosY = 0;
			obs1Dirc = 1;
		}
		else if (direction1 == 2) { //from right side, direction is 2
			obs1PosX = 12;
			obs1PosY = rand() % 7;
			obs1Dirc = 2;
		}
		else if (direction1 == 3) { //from bottom side, direction is 3
			obs1PosX = rand() % 13;
			obs1PosY = 6;
			obs1Dirc = 3;
		}
		drawObject(obs1PosX, obs1PosY, obstacle);
	}
	
	
	if (obs2PosX == -1 && obs2PosY == -1) {
		short direction2 = rand() % 4;
		if (direction2 == 0) { //from left side, direction is 0
			obs2PosX = 0;
			obs2PosY = rand() % 7;
			obs2Dirc = 0;
		}
		else if (direction2 == 1) { //from top side, direction is 1
			obs2PosX = rand() % 13;
			obs2PosY = 0;
			obs2Dirc = 1;
		}
		else if (direction2 == 2) { //from right side, direction is 2
			obs2PosX = 12;
			obs2PosY = rand() % 7;
			obs2Dirc = 2;
		}
		else if (direction2 == 3) { //from bottom side, direction is 3
			obs2PosX = rand() % 13;
			obs2PosY = 6;
			obs2Dirc = 3;
		}
		if(invis)
			moveObjectLeft(playerPosX, playerPosY, starPlayer, 1);
		else
			moveObjectLeft(playerPosX, playerPosY, player, 1);
	}
	
		
	if (obs3PosX == -1 && obs3PosY == -1) {
		short direction3 = rand() % 4;
		if (direction3 == 0) { //from left side, direction is 0
			obs3PosX = 0;
			obs3PosY = rand() % 7;
			obs3Dirc = 0;
		}
		else if (direction3 == 1) { //from top side, direction is 1
			obs3PosX = rand() % 13;
			obs3PosY = 0;
			obs3Dirc = 1;
		}
		else if (direction3 == 2) { //from right side, direction is 2
			obs3PosX = 12;
			obs3PosY = rand() % 7;
			obs3Dirc = 2;
		}
		else if (direction3 == 3) { //from bottom side, direction is 3
			obs3PosX = rand() % 13;
			obs3PosY = 6;
			obs3Dirc = 3;
		}	
		drawObject(obs3PosX, obs3PosY, obstacle);
	}
	
}

void moveObstacles() {
	obsTimer = (obsTimer + 1) % 300;
	if (obsTimer == 0) {
		if (obs1Dirc == 0) // from left side, move right
			moveObjectRight(&obs1PosX, &obs1PosY, obstacle, 0);
		else if (obs1Dirc == 1) // from top side, move down
			moveObjectDown(&obs1PosX, &obs1PosY, obstacle, 0);
		else if (obs1Dirc == 2) // from right side, move left
			moveObjectLeft(&obs1PosX, &obs1PosY, obstacle, 0);
		else if (obs1Dirc == 3) //from bottom side, move up
			moveObjectUp(&obs1PosX, &obs1PosY, obstacle, 0);
			
		if (obs2Dirc == 0) // from left side, move right
			moveObjectRight(&obs2PosX, &obs2PosY, obstacle, 0);
		else if (obs2Dirc == 1) // from top side, move down
			moveObjectDown(&obs2PosX, &obs2PosY, obstacle, 0);
		else if (obs2Dirc == 2) // from right side, move left
			moveObjectLeft(&obs2PosX, &obs2PosY, obstacle, 0);
		else if (obs2Dirc == 3) //from bottom side, move up
			moveObjectUp(&obs2PosX, &obs2PosY, obstacle, 0);
			
		if (obs3Dirc == 0) // from left side, move right
			moveObjectRight(&obs3PosX, &obs3PosY, obstacle, 0);
		else if (obs3Dirc == 1) // from top side, move down
			moveObjectDown(&obs3PosX, &obs3PosY, obstacle, 0);
		else if (obs3Dirc == 2) // from right side, move left
			moveObjectLeft(&obs3PosX, &obs3PosY, obstacle, 0);
		else if (obs3Dirc == 3) //from bottom side, move up
			moveObjectUp(&obs3PosX, &obs3PosY, obstacle, 0);
		drawObject(playerPosX, playerPosY, player);
	}
}


void collisionCheck(){
	// erase existing rewards
	if (obs1PosX == diamondPosX && obs1PosY == diamondPosY )
		objectsOnGrid[0] = 0;
	if (obs1PosX == applePosX && obs1PosY == applePosY )
		objectsOnGrid[1] = 0;	
	if (obs2PosX == diamondPosX && obs2PosY == diamondPosY )
		objectsOnGrid[0] = 0;
	if (obs2PosX == applePosX && obs2PosY == applePosY )
		objectsOnGrid[1] = 0;
	if (obs3PosX == diamondPosX && obs3PosY == diamondPosY )
		objectsOnGrid[0] = 0;
	if (obs3PosX == applePosX && obs3PosY == applePosY )
		objectsOnGrid[1] = 0;
	
	
	if(objectsOnGrid[0] == 1 && playerPosX == diamondPosX && playerPosY == diamondPosY) {//collision with Diamond
		sendThreeScore();
		objectsOnGrid[0] = 0;
	}
	if(objectsOnGrid[1] == 1 && playerPosX == applePosX && playerPosY == applePosY) {//collision with Apple
		sendOneScore();
		objectsOnGrid[1] = 0;
	}
	
	if(!invis){
		if (playerPosX == obs1PosX && playerPosY == obs1PosY){
			gameOver();
		}
		else if (playerPosX == obs2PosX && playerPosY == obs2PosY) {
			gameOver();
		}
		else if (playerPosX == obs3PosX && playerPosY == obs3PosY) {
			gameOver();
		}
	}
	else{
		if (playerPosX == obs1PosX && playerPosY == obs1PosY){
			obs1PosX = -1;
			obs1PosY = -1;
			sendOneScore();
		}
		else if (playerPosX == obs2PosX && playerPosY == obs2PosY) {
			obs2PosX = -1;
			obs2PosY = -1;
			sendOneScore();
		}
		else if (playerPosX == obs3PosX && playerPosY == obs3PosY) {
			obs3PosX = -1;
			obs3PosY = -1;
			sendOneScore();
		}
	}
}


void gameOver() {
	sendGameOver();
	LcdClear();
	gotoXY(15,8);
	LcdString("Game-Over      >=/");
}

void resetGame(){
	LcdClear();
	resetBarTo0();
	invis = 0;
	starPower = 0;
	
	gameStart = 0;
	gameisOver = 0;
	applePosX = -1;
	applePosY = -1;

	diamondPosX = -1;
	diamondPosY = -1;

	obs1PosX = -1;
	obs1PosY = -1;
	obs1Dirc = -1;

	obs2PosX = -1;
	obs2PosY = -1;
	obs2Dirc = -1;

	obs3PosX = -1;
	obs3PosY = -1;
	obs3Dirc = -1;
	
	fruitTimer = 0;
	obsTimer = 0;
	starPowerTimer = 0;
	
	for(unsigned char x = 0; x < 2; ++x) 
		objectsOnGrid[x] = 0;
}

void checkStarPower()
{
	if(starPower == 0)
		invis = 0;
		
	if(!invis)
		return;
	else {
		starPowerTimer = (starPowerTimer + 1) % 300;
		
		if(starPowerTimer == 0)
			decrementBar();
		if(starPower == 0)
			endStarPower();
	}
}

void endStarPower(){
	sendEndStarPower();
}

int main(void){

	DDRA = 0xFF; PORTA = 0x00; //LCD Screen
	DDRB = 0x0F; PORTB = 0xF0; //slave
	DDRC = 0b11110110; PORTC = 0x00; //NES 1111 0110
	//reads in from PORTC
	//orange: VCC
	//white: GND
	//black: data (PC0)
	//blue: CLK (PC1)
	//yellow: latch (PC2)
	DDRD = 0xFF; PORTD = 0x00; //LED Bar
	
	over:
	LcdInitialise();
	LcdClear();
 	gotoXY(20,9);
 	LcdString("Byte-iT");
	gotoXY(6,12);
	LcdString("Push START!");
	TimerOn();
	
	while(!gameStart){
		GetNESControllerData();
		if(snapshot == 0x08) { //START is pressed
			if(!gameStart)
				sendStart();
		}
	}
	
	resetGame();
	gameInit(0);
	
	while(1){
		NESControllerTick();
		throwFruit();
		throwNewObstacles();
		moveObstacles();
		collisionCheck();
		checkStarPower();
		if(softReset){
			sendReset();
			softReset = 0;
			goto over;
		}
		if(gameisOver)
		{
			gameStart = 0;
			_delay_ms(1500);
			if(!gameStart){
				resetGame();
				goto over;
			}
		}
	}
}