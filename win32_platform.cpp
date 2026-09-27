//Windows API
#include <windows.h>
// Required for GET_X_LPARAM and GET_Y_LPARAM
#include <windowsx.h>
//Include utility file
#include "utility.cpp"
//Include string library
#include <string>
//Include levels file with decleration
#include "levels.h"

//Drawable area != window area. This helps set the actual drawable area
struct Drawable_Area {
	int width, height;
};

//Bool that determines if the game is running or not
static bool running = true;

//A struct for the rendering state
struct Render_State {
	//Height and width of the window
	int height, width;
	//Memory assigned
	void* memory;

	//Bitmap that holds info like compression, bitsize of pixels etc.
	BITMAPINFO bitmap_info;
};

//Stores render info
static Render_State render_state;

//Stores drawable area info
static Drawable_Area drawable_area;

//Include platform common files
#include "platform_common.cpp"
//Include levels file
#include "levels.cpp"
//Include the renderer here instead of a linker
#include"renderer.cpp"
//Include player file
#include "player.cpp"
//Include camera file
#include "camera.cpp"
//Include game file
#include "game.cpp"

//A callback that is called after an even occurs by Windows
/*
LRESULT - The return type, custom from Windows API
CALLBACK - Macro, calling convention that tells Windows to send any messages to this function (window_callback)
HWND (Handle to a Window) - identifier for the specific window
UINT (unsigned integer type) - records message sent by Windows
WPARAM - contains specific info on an event
E.g. UINT uMsg may say - 'A key has been pressed' and WPARAM will say 'its the D key'
LPARAM - Same as above, only for different cases
*/
LRESULT CALLBACK window_callback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	//Simply defines a result with the type def given by Windows, is zero if the message is handled in the switch case
	LRESULT result = 0;

	//Allows us to do thigs based on specific messages Windows sends us
	switch (uMsg) {
		//We want to stop running for both - if window closes or gets destroyed
		case WM_CLOSE:
		case WM_DESTROY: {
			running = false;
		} break;
		//If detects a size change
		case WM_SIZE: {
			//Stores window size (left, right, top, bottom)
			RECT rect;
			GetClientRect(hwnd, &rect); //Get the actual size of window if reshaped
			//Set the buffer sizes to the actual window size
			render_state.width = rect.right - rect.left;
			render_state.height = rect.bottom - rect.top;

			//A buffer size that stores the amount of bytes the entire pixel buffer takes
			int size = render_state.width * render_state.height * sizeof(unsigned int);

			//Check if buffer memory already has some value
			if (render_state.memory)

				//If yes, clear it
				VirtualFree(render_state.memory, 0, MEM_RELEASE);
			//Allocates memory
			/*
			MEM_RESERVE - Reserves memory for the buffer
			MEM_COMMIT - backs the reserved memory, basically zeroes the data thats reserved
			PAGE_READWRITE - Enforces memory protection, sets limits on what can be done with this memory
			*/
			render_state.memory = VirtualAlloc(0, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

			//Records size of the bmiHeader struct
			render_state.bitmap_info.bmiHeader.biSize = sizeof(render_state.bitmap_info.bmiHeader);
			//Sets height and width of the BitMap
			render_state.bitmap_info.bmiHeader.biWidth = render_state.width;
			render_state.bitmap_info.bmiHeader.biHeight = -render_state.height;
			//Colour planes, should always be 1
			render_state.bitmap_info.bmiHeader.biPlanes = 1;
			//Each pixel is defined by 32 bits
			render_state.bitmap_info.bmiHeader.biBitCount = 32;
			//Specifies that the pixel data is uncompressed
			render_state.bitmap_info.bmiHeader.biCompression = BI_RGB;

		} break;
		//Any non-explicitly handled case
		default: {
			//If there is a Windows message not handled by the code I don't want to return result = 0, and thus we just return the message with the parameters
			result = DefWindowProc(hwnd, uMsg, wParam, lParam);
		}

	}
	//Returns 0 if message handled, returns the Windows message if not
	return result;
}

//Main func but from the Windows API, WINAPI is the call convention of Windows' functions
/*
HINSTANCE 1 - Specific running instance of the program
HINSTANCE 2 - Always NULL
LPSTR lpCmdLine - Command line arguments with which the program launched
nCmdShow - How the window should be initially showes (e.g. Windowed, minimized, maximized, fullscreen)
*/
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	//Create Window Class
	//Actually resloves to WNDCLASSA or WNDCLASSW
	//Blueprint for a category of window (that we want to create), is currently empty
	WNDCLASS window_class = {};
	//Redraw the Height and Width every time it changes, bitwise OR since either can happen
	window_class.style = CS_HREDRAW | CS_VREDRAW;
	//Name of the template
	window_class.lpszClassName = "Game Window Class";
	//Long pointer to function, any window with this template will send all messages to the LRESULT CALLBACK window_callback we defined
	window_class.lpfnWndProc = window_callback;

	//Register Class
	//Registers the data and gives this template to Windows to call later, rather than passing all values in struct - we pass the address since the function looks up the values itself
	RegisterClass(&window_class);

	//Create Window
	/*
	lpszClassName - Tries to find a template with the same name to match
	lpWindowName - Name of the window
	dwStyle - What kind of window, here its a normal resizable
	x, y - initial points for the window pition
	nWidth, nHeight - Window size in pixels, including top bar
	hWndParent - If a parent window exists
	hMenu - If a menu exists
	hInstance - The instance the application is running on
	lpParam - Any extra data being passed to WM_CREATE
	*/

	//Set the drawable area
	drawable_area = { 1280, 720 };

	//RECT is needed to set drawable area, its a windows native dtype
	RECT rect = { 0, 0, drawable_area.width, drawable_area.height };

	//Adjust the window to fit the actual drawable area
	/*
	&rect - dimensions
	WS_OVERLAPPEDWINDOW - Window type
	FALSE - If there is a menu
	Style - 0
	*/
	AdjustWindowRectEx(
		&rect,
		WS_OVERLAPPEDWINDOW,
		FALSE,
		0
	);

	//HWND - handle to a window
	//Create a window
	/*
	ClassName = the unique class name of the windows class we defined
	Title of window
	X, Y - CW_USDEFAULT
	Dimensions
	Parent window - 0 since no parent window
	Menu - 0 since no menu
	handle instance - hInstance from WinMain
	lpParam - Any additional parameters to pass during creation, 0
	*/
	HWND window = CreateWindow(window_class.lpszClassName, 
		"Pong but scuffed", 
		WS_OVERLAPPED | WS_VISIBLE, 
		CW_USEDEFAULT, 
		CW_USEDEFAULT, 
		rect.right - rect.left, 
		rect.bottom - rect.top, 
		0, 0, hInstance, 0);
	//Retrieves device context, drwaing state + current drawing state/settings
	HDC hdc = GetDC(window);
	//ShowWindow(window, SW_MAXIMIZE);

	//Declare a input struct object
	Input input = {};

	//Delta time measures time in seconds between 2 frames, assuming 60FPS as default
	float delta_time = 0.016666f;

	//First frame data
	LARGE_INTEGER frame_begin_time;

	//Sets the begin time to the performance counter
	QueryPerformanceCounter(&frame_begin_time);

	//Records the number of performance per sec
	double performance_frequency;
	{
		LARGE_INTEGER perf;
		QueryPerformanceFrequency(&perf);
		performance_frequency = (double)perf.QuadPart;
	}

	//Game Setup
	setup_game();

	//Exists to send a string to the debug terminal, to e.g. check the value of a variable
	std::string message =
		"Width: " + std::to_string(render_state.width) +
		", Height: " + std::to_string(render_state.height) + "\n";

	//Outputs a string in the default Debug output
	OutputDebugStringA(message.c_str());

	//Game Loop
	while (running) {
		//Input
		//A struct that will be filled by PeekMessge() like lParam and wParam
		MSG message;
		//While loop to look at all pending messages rather than if
		/*
		&message - Where to store the message
		window - Only messages for this window
		min/max message type filter - What type of messages to filter
		PM_REMOVE - Remove message once read
		*/

		//Set all the button states as 'not changed'
		for (int i = 0; i < BUTTON_COUNT; i++) {
			input.button[i].changed = false;
		}

		//Look at the callback messages sent, while cause there could be multiple
		/*
		&message - where to store message
		widow - specific handle window where the messages are coming from
		0, 0 - no limits on the messages recived
		PM_REMOVE - Remove the message after it has been processed
		*/
		while (PeekMessage(&message, window, 0, 0, PM_REMOVE)) {
			//Do specific things based on the message recieved
			switch (message.message) {
				//If a key is (un?)pressed
				case WM_KEYUP:
				//If a key is pressed
				case WM_KEYDOWN: {
					//Store the actual key pressed (which is in the wParam) in a variable
					unsigned int vk_code = (unsigned int)message.wParam;
					//In lParam Windows stores if a key is pressed if bit 31 = 0, if its released, bit 31 = 1, '<<' exists cause we only want to check the state of bit 31
					bool is_down = ((message.lParam & (1 << 31)) == 0);

//better to define process button here to noot write this every time, chnaged means it went from is_down to not is_down or vice versa
#define process_button(b, vk)\
case vk: {\
input.button[b].changed = is_down != input.button[b].is_down;\
input.button[b].is_down = is_down;\
} break;
					//Process the specific button pressed
					switch (vk_code) {
						process_button(BUTTON_UP, VK_UP);
						process_button(BUTTON_DOWN, VK_DOWN);
						process_button(BUTTON_LEFT, VK_LEFT);
						process_button(BUTTON_RIGHT, VK_RIGHT);
						process_button(BUTTON_W, 'W');
						process_button(BUTTON_S, 'S');
						process_button(BUTTON_A, 'A');
						process_button(BUTTON_D, 'D');
						process_button(BUTTON_P, 'P');
						process_button(BUTTON_I, 'I');
						process_button(BUTTON_0, '0');
						process_button(BUTTON_1, '1');
						process_button(BUTTON_2, '2');
						process_button(BUTTON_3, '3');
						process_button(BUTTON_4, '4');
						process_button(BUTTON_5, '5');
						process_button(BUTTON_6, '6');
						process_button(BUTTON_7, '7');
						process_button(BUTTON_8, '8');
						process_button(BUTTON_9, '9');
					}
				} break;
				//Has mouse moved
				case WM_MOUSEMOVE: {
					//Get the X and Y, needs windowsx.h
					input.mouse_x = GET_X_LPARAM(message.lParam);
					input.mouse_y = GET_Y_LPARAM(message.lParam);
					break;
				}
				//Is left mouse pressed
				case WM_LBUTTONDOWN: {
					input.mouse_x = GET_X_LPARAM(message.lParam);
					input.mouse_y = GET_Y_LPARAM(message.lParam);
					input.is_mouse_down = true;
					input.mouse_just_pressed = true;
					break;
				}
				//Is left mouse (un?)pressed
				case WM_LBUTTONUP: {
					input.mouse_x = GET_X_LPARAM(message.lParam);
					input.mouse_y = GET_Y_LPARAM(message.lParam);
					input.is_mouse_down = false;
					break;
				}

			}
			//Preprocesses keyboard related messages
			TranslateMessage(&message);
			//Asks Windows to call the window_callback
			DispatchMessage(&message);
		}
		//Simulate
		simulate_game(&input, delta_time);

		//Render
		/*
		hdc - Where to draw (to DC)
		0, 0, render_state.width, render_state.height - Start drawing from 0, 0 to the entire window size
		0, 0, render_state.width, render_state.height 2 - Read from 0, 0 to the entire window size
		render_state.memory - The pixel data
		&render_state.bitmap_info - How to draw, the dimensions, bits and compression - the bitmap we made before
		DIB_RGB_COLORS - Literal RGB values
		SRCCOPY - How the pixels should be rendered w.r.t the pixels that already exist there
		*/
		StretchDIBits(hdc, 0, 0, render_state.width, render_state.height, 0, 0, render_state.width, render_state.height, render_state.memory, &render_state.bitmap_info, DIB_RGB_COLORS, SRCCOPY);
		
		//Check the frame time after rendering
		LARGE_INTEGER frame_end_time;
		QueryPerformanceCounter(&frame_end_time);

		//Calculate the time passed since last frame by subtracting the (end-begin)/the no. of performances in a second
		delta_time = (float)((double)frame_end_time.QuadPart - frame_begin_time.QuadPart) / performance_frequency;
		//Last end frame = next start frame
		frame_begin_time = frame_end_time;
	}
}