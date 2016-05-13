//=========================================================
// Includes
//=========================================================
#include <windows.h>
#include <BackBuffer.h>
#include <strstream>
#include <mmsystem.h>
#include "Define.h"
#include "Object.h"
#include "resource.h"
#pragma comment(lib, "winmm.lib")
using namespace std;
//=========================================================
// Globals
//=========================================================
HWND        ghMainWnd  = 0; // Main window handle.
HINSTANCE   ghAppInst  = 0; // Application instance handle.
HMENU       ghMainMenu = 0; // Menu handle.

// The backbuffer we will render onto.
BackBuffer* gBackBuffer = 0;

// The text that will appear in the main window's caption bar.
char gWndCaption[] = "Window Caption --Frames Per Second = ";

Paddle  paddle;
GameMap gameMap;
Block   block;
Ball    ball;

//=========================================================
// Function Prototypes
//=========================================================

bool InitMainWindow();
int  Run();
void DrawFramesPerSecond(float deltaTime);

LRESULT CALLBACK 
WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

BOOL CALLBACK
DlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);

//=========================================================
// Name: WinMain
// Desc: Program execution starts here.
//=========================================================

int WINAPI 
WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
		PSTR cmdLine, int showCmd)
{
	ghAppInst = hInstance;

	// Create the main window.
	if( !InitMainWindow() )
	{
		MessageBoxA(0, "Window Creation Failed.", "Error", MB_OK);
		return 0;
	}

	// Enter the message loop.
	return Run();
}

//=========================================================
// Name: InitMainWindow
// Desc: Creates the main window upon which we will
//       draw the game graphics onto.
//=========================================================
bool InitMainWindow()
{
	WNDCLASS wc; 
	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = WndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = ghAppInst;
	wc.hIcon         = ::LoadIcon(0, IDI_APPLICATION);
	wc.hCursor       = ::LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)::GetStockObject(NULL_BRUSH);
	wc.lpszMenuName  = 0;
	wc.lpszClassName = TEXT("MyWndClassName");

	RegisterClass( &wc );

	// WS_OVERLAPPED | WS_SYSMENU: Window cannot be resized
	// and does not have a min/max button.  
	ghMainMenu = LoadMenu(ghAppInst, MAKEINTRESOURCE(IDR_MENU1));
	ghMainWnd = ::CreateWindowA("MyWndClassName", 
		gWndCaption, WS_OVERLAPPED | WS_SYSMENU,
		250, 50,
		WINDOW_WIDTH,  
        WINDOW_HEIGHT+40, 
		0,
	    ghMainMenu, ghAppInst, 0);

	if(ghMainWnd == 0)
	{
		::MessageBoxA(0, "CreateWindow - Failed", 0, 0);
		return 0;
	}

	ShowWindow(ghMainWnd, SW_NORMAL);
	UpdateWindow(ghMainWnd);
//	ShowCursor(false);

	return true;
}

//=========================================================
// Name: Run
// Desc: Encapsulates the message loop.
//=========================================================
int Run()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	// Get the performance timer frequency.
	__int64 cntsPerSec = 0;
	bool perfExists = QueryPerformanceFrequency((LARGE_INTEGER*)&cntsPerSec)!=0;
	if( !perfExists )
	{
		MessageBox(0, L"Performance timer does not exist!", 0, 0);
		return 0;
	}

	double timeScale = 1.0 / (double)cntsPerSec;
	// Get the current time.
	__int64 lastTime = 0;
	QueryPerformanceCounter((LARGE_INTEGER*)&lastTime);

	while(msg.message != WM_QUIT)
	{
		// IF there is a Windows message then process it.
		if(PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		// ELSE, do game stuff.
		else
		{
			// Get the time now.
			
			__int64 currTime = 0;
			QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

			// Compute the differences in time from the last
			// time we checked.  Since the last time we checked
			// was the previous loop iteration, this difference
			// gives us the time between loop iterations...
			// or, I.e., the time between frames.
			double deltaTime = (double)(currTime - lastTime) * timeScale;

			// Get the backbuffer DC.
			HDC hbbDC = gBackBuffer->getDC();

			// draw the game backGround

			DrawFramesPerSecond((float)deltaTime);

			// display the backGround color
			gameMap.draw_BG(hbbDC);

			// then, draw the blocks
			block.draw_blocks(hbbDC);

			if( GetAsyncKeyState(VK_LEFT) )
				paddle.paddle_X -= 6;
			else
				if( KEY_DOWN(VK_RIGHT) )
					paddle.paddle_X += 6;

			// display the paddle
			paddle.draw_paddle(hbbDC);

			// and, display the ball
			ball.draw_ball(hbbDC);

			// Now present the backbuffer contents to the main
			// window client area.
			gBackBuffer->present();
				
			// We are at the end of the loop iteration, so
			// prepare for the next loop iteration by making
			// the "current time" the "last time."
			lastTime = currTime;

			// Free 20 miliseconds to Windows so we don't hog
			// the system resources.
			Sleep(15);
        }
    }
	// Return exit code back to operating system.
	return (int)msg.wParam;
}

//=========================================================
// Name: DrawFramesPerSecond
// Desc: This function is called every frame and updates
//       the frame per second display in the main window
//       caption.
//=========================================================
void DrawFramesPerSecond(float deltaTime)
{
	// Make static so the variables persist even after
	// the function returns.
	static int   frameCnt    = 0;
	static float timeElapsed = 0.0f;
	char buff[256];             //memory buffer that will have our new caption

	// Function called implies a new frame, so increment
	// the frame count.
	++frameCnt;

	// Also increment how much time has passed since the
	// last frame.  
	timeElapsed += deltaTime;

	// Has one second passed?
	if( timeElapsed >= 1.0f )
	{
		// Yes, so compute the frames per second.
		// FPS = frameCnt / timeElapsed, but since we
		// compute only when timeElapsed = 1.0, we can 
		// reduce to:
		// FPS = frameCnt / 1.0 = frameCnt.

		ostrstream ostr(buff, 256);
		ostr << gWndCaption << frameCnt << ends;

		// Now set the new caption to the main window.
		SetWindowTextA(ghMainWnd, buff);
	
		// Reset the counters to prepare for the next time
		// we compute the frames per second.
		frameCnt    = 0;
		timeElapsed = 0.0f;
	}
}

//=========================================================
// Name: WndProc
// Desc: The main window procedure.
//=========================================================

LRESULT CALLBACK 
WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{	
	switch( msg )
	{	
	// Create application resources.
	case WM_CREATE:
		// here is the initialization staff
		gBackBuffer = new BackBuffer(hWnd, GAME_WIDTH, GAME_HEIGHT+100);
		return 0;

	case WM_COMMAND:
		if( LOWORD(wParam) == ID_FILE_EXIT )
			DestroyWindow(hWnd);

		if( LOWORD(wParam) == ID_HELP_ABOUT )
			DialogBox(ghAppInst, 
			          MAKEINTRESOURCE(IDD_DIALOG1) ,
			          hWnd, 
			          DlgProc);
		return 0;

	case WM_KEYDOWN:
		return 0;
	// Destroy application resources.
	case WM_DESTROY: 	
		delete gBackBuffer;
		PostQuitMessage(0);
		return 0;	
	}	
	// Forward any other messages we didn't handle to the
	// default window procedure.
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

BOOL CALLBACK
DlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch( msg )
	{
	case WM_COMMAND:
		if( LOWORD(wParam) == IDOK )
			EndDialog(hDlg, 0);
		return true;
	}
	return false;
}