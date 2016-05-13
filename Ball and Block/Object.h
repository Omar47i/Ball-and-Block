// Object.h
// this file declares the main game objects with its members

#ifndef OBJECT_H
#define OBJECT_H

#include "Define.h"
#include <strstream>
//----------------------------------------------------------------------------------
// Name : Game_Map (Class)
// Desc : This class draws the game map
//----------------------------------------------------------------------------------
class GameMap
{
private:
	LOGBRUSH lb;                  //logbrush for customizing bg color 
	HBRUSH hbrBG;                 //handle to background brush
	HBRUSH hOldBrush;             //handle to old brush that was originally loaded

public:

	GameMap();                    //default constructor
	~GameMap();                   //destructor

	void draw_BG(HDC hbbDC);      //draw the background 
};
//----------------------------------------------------------------------------------
// Name : Paddle (Class)
// Desc : This class contain information about game paddle
//----------------------------------------------------------------------------------
class Ball;
class Paddle
{
public:
	HBRUSH hbrPaddle;             //handle to paddle brush
	HBRUSH hOldBrush;

	static int paddle_X;          //x-axis of the paddle
	static int paddle_Y;          //y-axis of the paddle
	Ball* ptrBall;

public:

	Paddle();                     //default constructor
	~Paddle();

	void draw_paddle(HDC hbbDC);  //draw the paddle

};
//----------------------------------------------------------------------------------
// Name : Ball (Class)
// Desc : This class contains information about the game ball
//----------------------------------------------------------------------------------
class Block;

class Ball
{
public:
	HBRUSH hbrBall;
	HBRUSH hOldBrush;

	int SCORE;

	int ball_x;     //x-axis of the ball
	int ball_y;
	int ball_dx;           //velocity of the ball
	int ball_dy;

	//a pointer to Block object so we can know wether we hitted the blocks or not
	Block*  ptrBlock;
	//a pointer to Paddle object to know if we hitted the paddle or not
	Paddle* ptrPaddle;

public:

	Ball();                       //default constructor
	~Ball();

	void draw_ball(HDC hbbDC);

private:
	void check_block();
	void check_paddle();
};
//----------------------------------------------------------------------------------
// Name : BLOCK (Class)
// Desc : This class contains information about blocks
//----------------------------------------------------------------------------------
class Block
{
private:
	HBRUSH hbrBlock1;
	HBRUSH hbrBlock2;
	HBRUSH hbrBlock3;
	HBRUSH hbrBlock4;
	HBRUSH hbrBlock5;
	HBRUSH hbrBlock6;
	HBRUSH hOldBrush;

public:
	//this array represents the blocks coordinates
	static POINT blockArr[NUM_BLOCK_ROWS][NUM_BLOCK_COLUMNS];
	static int BLOCK_COUNT;        //number of blocks

public:

	Block();
	~Block();

	void draw_blocks(HDC hbbDC);
};

#endif