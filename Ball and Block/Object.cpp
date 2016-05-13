// Object.cpp
// implement the Object methods

#include "Object.h"
#include <time.h>

// Game_Map Class //////////////////////////////////////////////////////////////////
GameMap::GameMap()
{
	lb.lbColor = RGB(50, 200, 50);

	hbrBG = CreateBrushIndirect( &lb );

	hOldBrush = 0;
}
//----------------------------------------------------------------------------------
void GameMap::draw_BG(HDC hbbDC)
{
	hOldBrush = (HBRUSH)SelectObject(hbbDC, GetStockObject(BLACK_BRUSH));

	Rectangle(hbbDC, min_clip_x,
		             min_clip_y,
					 max_clip_x,
					 max_clip_y+40);

	SelectObject(hbbDC, hbrBG);

	// draw the back ground rectangle
	Rectangle(hbbDC, min_clip_x,
		             min_clip_y,
					 max_clip_x,
					 max_clip_y);
	// select the originally loaded brush
	SelectObject(hbbDC, hOldBrush);
}
//----------------------------------------------------------------------------------
GameMap::~GameMap()
{
	DeleteObject(hbrBG);
	DeleteObject(hOldBrush);
}

// Paddle Class ////////////////////////////////////////////////////////////////////
// static variables
int Paddle::paddle_X=0;
int Paddle::paddle_Y=0;
Paddle::Paddle()
{
	hbrPaddle = (HBRUSH)GetStockObject(DKGRAY_BRUSH);

	hOldBrush = NULL;

	// set the paddle position to the middle bottom
	paddle_X = PADDLE_START_X;
	paddle_Y = PADDLE_START_Y;
}
//-----------------------------------------------------------------------------------
Paddle::~Paddle()
{
	DeleteObject(hbrPaddle);
	DeleteObject(hOldBrush);
}
//-----------------------------------------------------------------------------------
void Paddle::draw_paddle(HDC hbbDC)
{
	// first, select the paddle brush
	hOldBrush = (HBRUSH)SelectObject(hbbDC, hbrPaddle);

	if( paddle_X < min_clip_x )
		paddle_X = 0;
	if( paddle_X+PADDLE_WIDTH > max_clip_x )
		paddle_X = GAME_WIDTH-PADDLE_WIDTH;
	Rectangle(hbbDC, paddle_X,
		             paddle_Y,
					 paddle_X + PADDLE_WIDTH,
					 paddle_Y + PADDLE_HEIGHT);

	SelectObject(hbbDC, hOldBrush);
}

// Ball Class ///////////////////////////////////////////////////////////////////////
// static variables
Ball::Ball()
{
	hbrBall   = (HBRUSH)GetStockObject(BLACK_BRUSH);
	hOldBrush = NULL;

	SCORE = 0;

	// initialize seed random number generator
	srand( time(0) );

	// initialize the positions and velocity of the ball
	ball_x  = 250;
	ball_y  = BALL_START_Y;
	ball_dx = 4;
	ball_dy = 5;

	// initialize the object pointers
	ptrBlock  = new Block;
	ptrPaddle = new Paddle;
}
//-----------------------------------------------------------------------------------
Ball::~Ball()
{                              
	                                  //release allocated resources
	DeleteObject(hbrBall);
	DeleteObject(hOldBrush);

	delete ptrBlock;
	delete ptrPaddle;
}
//----------------------------------------------------------------------------------
void Ball::draw_ball(HDC hbbDC)
{
	check_paddle();

	if( ball_dx > 8 )
		ball_dx = 8;
	if( ball_dx < -8 )
		ball_dx = -8;
	// update ball position
	ball_x += ball_dx;
	ball_y += ball_dy;

	// the ball must be in the game area, if it hits the edge of
	// the screen, then bounce it by reflecting its velocity
	if( (ball_x > (max_clip_x - BALL_WIDTH)) || ball_x < min_clip_x )
	{
		// reflect the x-axis velocity
		ball_dx = -ball_dx;

		// update position
		ball_x  += ball_dx;
	}

	if( ball_y < (min_clip_y) )
	{
		// reflect the y-axis velocity
		ball_dy = -ball_dy;

		// update ball position
		ball_y += ball_dy;
	}

	// if the ball has hitted the bottom screen, then 
	// reflect its y-axis velocity and decrease score
	if( ball_y > (GAME_HEIGHT - BALL_HEIGHT) )
	{
		// reflect y-axis velocity
		ball_dy = -ball_dy;

		// update ball position
		ball_y  += ball_dy;

		// decrese game score by 50
		SCORE -= 50;
		PlaySound(TEXT("Error.wav"), 0, SND_ASYNC);
	}

	// select the ball brush
	hOldBrush = (HBRUSH)SelectObject(hbbDC, hbrBall);
	Rectangle(hbbDC, ball_x, ball_y, 
		             (ball_x+BALL_WIDTH), (ball_y+BALL_HEIGHT));

	char buff[256];
	std::ostrstream ostr(buff, 256);
	ostr << "SCORE  " << SCORE << "          Developed by Omar Saeed" << std::ends;
	TextOutA(hbbDC, 20, GAME_HEIGHT + 10, buff, strlen(buff));

	// check if the ball hitted the paddle or the blocks
	check_block();
}
//-----------------------------------------------------------------------------------
void Ball::check_block()
{
	// check if the ball has hitted a block

	for(int row=0; row<NUM_BLOCK_ROWS; row++)           //for each block rows
		for(int col=0; col<NUM_BLOCK_COLUMNS; col++)    //and each column
		{
			if( (ball_x >= ptrBlock->blockArr[row][col].x && 
				 ball_x <= (ptrBlock->blockArr[row][col].x + BLOCK_WIDTH) ) &&
				 (ball_y >= (ptrBlock->blockArr[row][col].y + BLOCK_HEIGHT-3) &&
				 ball_y  <= ptrBlock->blockArr[row][col].y + BLOCK_HEIGHT) )
			{
				 // reflect ball y-axis velocity
				 ball_dy = -ball_dy;
				 ptrBlock->blockArr[row][col].x = 0;
				 ptrBlock->blockArr[row][col].y = 0;
				 PlaySound(L"music2.wav", 0, SND_ASYNC);
				 SCORE += 50;
			}
			if( (ball_x >= ptrBlock->blockArr[row][col].x && 
				ball_x  <= (ptrBlock->blockArr[row][col].x + BLOCK_WIDTH) ) &&
				ball_y >= ptrBlock->blockArr[row][col].y &&
				ball_y <= ptrBlock->blockArr[row][col].y+3)
			{
				// reflect y-axis velocity
				ball_dy = -ball_dy;
				ptrBlock->blockArr[row][col].x = 0;
				ptrBlock->blockArr[row][col].y = 0;
				PlaySound(L"music2.wav", 0, SND_ASYNC);
				SCORE += 50;
			}
			if( ((ball_x+BALL_WIDTH) >= ptrBlock->blockArr[row][col].x &&
				(ball_x+BALL_WIDTH) <= ptrBlock->blockArr[row][col].x+3) &&
				(ball_y >= ptrBlock->blockArr[row][col].y && 
				 ball_y <= ptrBlock->blockArr[row][col].y+BLOCK_HEIGHT) )
			{
				// reflect x-axis velocity
				ball_dx = -ball_dx;
				ptrBlock->blockArr[row][col].x = 0;
				ptrBlock->blockArr[row][col].y = 0;
				PlaySound(L"music2.wav", 0, SND_ASYNC);
				SCORE += 50;
			}
			if( (ball_x >= (ptrBlock->blockArr[row][col].x+BLOCK_WIDTH-3) &&
				 ball_x <= ptrBlock->blockArr[row][col].x+BLOCK_WIDTH) &&
				(ball_y >= ptrBlock->blockArr[row][col].y && 
				 ball_y <= ptrBlock->blockArr[row][col].y+BLOCK_HEIGHT) )
			{
				// reflect x-axis velocity
				ball_dx = -ball_dx;
				ptrBlock->blockArr[row][col].x = 0;
				ptrBlock->blockArr[row][col].y = 0;
				PlaySound(L"music2.wav", 0, SND_ASYNC);
				SCORE += 50;
			}
		}
}
//-----------------------------------------------------------------------------------
void Ball::check_paddle()
{
	if( ball_x >= ptrPaddle->paddle_X && ball_x <= (ptrPaddle->paddle_X+PADDLE_WIDTH )
		&& (ball_y >= ptrPaddle->paddle_Y && ball_y <= ptrPaddle->paddle_Y+PADDLE_HEIGHT))
		if( ball_dy >=0 )
		{
			// reflect the y-axis velocity with 
			ball_dy = -ball_dy;
			PlaySound(L"music1.wav", 0, SND_ASYNC);
		}

	if( (ball_x >= (ptrPaddle->paddle_X+PADDLE_WIDTH-3) && 
		ball_x <= (ptrPaddle->paddle_X+PADDLE_WIDTH) ) && (
		ball_y >=  ptrPaddle->paddle_Y && ball_y <= ptrPaddle->paddle_Y+PADDLE_WIDTH) )
	{
		// reflect the x-axis velocity
		if( ball_dy >=0 )
		ball_dx = -ball_dx;
		PlaySound(L"music1.wav", 0, SND_ASYNC);
	}

	if( (ball_x+BALL_WIDTH >= ptrPaddle->paddle_X && 
		ball_x <= ptrPaddle->paddle_X+3) && (
		ball_y >=  ptrPaddle->paddle_Y && ball_y <= ptrPaddle->paddle_Y+PADDLE_WIDTH) )
	{
		// reflect the x-axis velocity
		if( ball_dy >=0 )
		ball_dx = -ball_dx;
		PlaySound(L"music1.wav", 0, SND_ASYNC);
	}

	if( ball_x >= ptrPaddle->paddle_X && ball_x <= (ptrPaddle->paddle_X+20 )
		&& (ball_y >= ptrPaddle->paddle_Y && ball_y <= ptrPaddle->paddle_Y+PADDLE_HEIGHT))
			ball_dx--;

	if( ball_x >= (ptrPaddle->paddle_X+40) && ball_x <= (ptrPaddle->paddle_X+PADDLE_WIDTH )
		&& (ball_y >= ptrPaddle->paddle_Y && ball_y <= ptrPaddle->paddle_Y+PADDLE_HEIGHT))
			ball_dx++;
}
// Block Class //////////////////////////////////////////////////////////////////////
// static variables
POINT Block::blockArr[NUM_BLOCK_ROWS][NUM_BLOCK_COLUMNS] = {0};
int  Block::BLOCK_COUNT = 0;

Block::Block()
{
	LOGBRUSH lb;
	lb.lbColor = RGB(255, 0, 40);
	lb.lbHatch = NULL;
	lb.lbStyle = BS_SOLID;
	// initialize the block color
	hbrBlock1 = CreateBrushIndirect( &lb );

	lb.lbColor = RGB(0, 0, 255);
	lb.lbHatch = NULL;
	lb.lbStyle = BS_SOLID;
	hbrBlock2 = CreateBrushIndirect( &lb );

	lb.lbColor = RGB(255, 255, 0);
	lb.lbHatch = NULL;
	lb.lbStyle = BS_SOLID;
	// initialize the block color
	hbrBlock3 = CreateBrushIndirect( &lb );

	lb.lbColor = RGB(0, 255, 255);
	lb.lbHatch = NULL;
	lb.lbStyle = BS_SOLID;
	// initialize the block color
	hbrBlock4 = CreateBrushIndirect( &lb );

	lb.lbColor = RGB(100, 100, 0);
	lb.lbHatch = NULL;
	lb.lbStyle = BS_SOLID;
	// initialize the block color
	hbrBlock5 = CreateBrushIndirect( &lb );

	lb.lbColor = RGB(100, 0, 100);
	lb.lbHatch = NULL;
	lb.lbStyle = BS_SOLID;
	// initialize the block color
	hbrBlock6 = CreateBrushIndirect( &lb );

	hOldBrush = NULL;
	// number of blocks
	BLOCK_COUNT = NUM_BLOCK_ROWS * NUM_BLOCK_COLUMNS;

	POINT pt;
	// initializes the array of blocks
	for(int row = 0; row<NUM_BLOCK_ROWS; row++)        //for each rows
		for(int col=0; col<NUM_BLOCK_COLUMNS; col++)   //and each column
		{
			pt.x = BLOCK_X_START + ((BLOCK_WIDTH + BLOCK_X_GAP) * col);
			pt.y = BLOCK_Y_START + ((BLOCK_HEIGHT + BLOCK_Y_GAP) * row);

			blockArr[row][col] = pt;
		}
}
//-----------------------------------------------------------------------------------
void Block::draw_blocks(HDC hbbDC)
{
	// draw each block
	for(int row = 0; row<NUM_BLOCK_ROWS; row++)
		for(int col=0; col<NUM_BLOCK_COLUMNS; col++)
		{

			// if the block is already hitted by the ball
			// then don't draw it
			if( blockArr[row][col].x == 0 )
				continue;
			// select a color for the block
			switch( row )
			{
			case 0: hOldBrush = (HBRUSH)SelectObject(hbbDC, hbrBlock1);
				break;
			case 1: hOldBrush = (HBRUSH)SelectObject(hbbDC, hbrBlock2);
				break;
			case 2: hOldBrush = (HBRUSH)SelectObject(hbbDC, hbrBlock3);
				break;
			case 3: hOldBrush = (HBRUSH)SelectObject(hbbDC, hbrBlock4);
				break;
			case 4: hOldBrush = (HBRUSH)SelectObject(hbbDC, hbrBlock5);
				break;
			case 5: hOldBrush = (HBRUSH)SelectObject(hbbDC, hbrBlock6);
			}

			Rectangle(hbbDC, blockArr[row][col].x, blockArr[row][col].y, 
				             blockArr[row][col].x + BLOCK_WIDTH, 
							 blockArr[row][col].y + BLOCK_HEIGHT);

		} // end inner for
	SelectObject(hbbDC, hOldBrush);
}
//----------------------------------------------------------------------------------
Block::~Block()
{
	DeleteObject(hbrBlock1);
	DeleteObject(hbrBlock2);
	DeleteObject(hbrBlock3);
	DeleteObject(hbrBlock4);
	DeleteObject(hbrBlock5);
	DeleteObject(hbrBlock6);
	DeleteObject(hOldBrush);
}