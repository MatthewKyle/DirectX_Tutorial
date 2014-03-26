////////////////////////////////////////////////////////////////////////////////
// Filename: systemclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "systemclass.h"

SystemClass::SystemClass()
{
	m_Input=0;
	m_Graphics=0;
}

SystemClass::SystemClass(const SystemClass& other)
{

}

SystemClass::~SystemClass()
{

}

bool SystemClass::Initialize()
{
	int screenWidth, screenHeight;
	bool result;

	//Intialize the width and height of the screeen to zero before sending the variables into the function.
	screenWidth = 0;
	screenHeight=0;

	//Intialize the windows api.
	IntializeWindows(screenWidth,screenHeight);

	// Create the input object.  This object will be used to handle reading the keynoard inpit from the user.
	m_Input = new InputClass;
	if(!m_Input)
	{
		return false;
	}

	// Initialize the inout object.
	m_Input->Initialize();

	// Create the graphics onject.  This obkect will handle rendering all the groahics for this application.
	m_Graphics = new GraphicsClass;
	if(!m_Graphics)
	{
		return false;
	}

	// Intialize the grpahics object.
	result = m_Graphics->Initialize(screenWidth, screenHeight, m_hwnd);
	if(!result)
	{
		return false;
	}

	return true;


}

void SystemClass::Shutdown()
{
	//Release the graphics object.
	if(m_Graphics)
	{
		m_Graphics->Shutdown();
		delete m_Graphics;
		m_Graphics = 0;
	}

	//Release the inpit object.
	if(m_Input)
	{
		delete m_Input;
		m_Input=0;
	}

	// Shutdown the window.
	ShutdownWindows();

	return;
}

void SystemClass::Run()
{
	MSG msg;
	bool done, result;

	// Initialize the message structure.
	ZeroMemory(&msg, sizeof(MSG));

	// Loop until there is a quit message fromt he window or the user.
	done=false;
	while(!done)
	{
		// Handle the windowes messages.
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// If windwoes signales to end the application then exit out.
		if(msg.message == WM_QUIT)
		{
			done= true;
		}
		else
		{
			// Otherwise do the frame processing
			result = Frame();
			if(!result)
			{
				done=true;
			}
		}

	}
	
	return;
}

bool SystemClass::Frame()
{
	bool result;

	// Check if the user pressed escape and wants to exit teh application.
	if(m_Input->IsKeyDown(VK_ESCAPE))
	{
		return false;
	}

	// Do the grame processing for the graphics object.
	result =m_Graphics->Frame();
	if(!result)
	{
		return false;
	}

	return true;
}

LRESULT CALLBACK SystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch(umsg)
	{
		// Check if a jey has been pressed on the keyboard.
	case WM_KEYDOWN:
		{
			// If a keu is pressed send it to the inpit object so it can record that state.
			m_Input->KeyDown((unsigned int)wparam);
			return 0;
		}
		
	case WM_KEYUP:
		{
			//If a key is released the send it to the inpit object so it can unset the state of the key.
			m_Input->KeyUp((unsigned int)wparam);
			return 0;
		}

		// Any other messages send to the default message handker as our application won't make use of them.
	default:
		{
			return DefWindowProc(hwnd, umsg, wparam, lparam); 
		}

	}
}

void SystemClass::IntializeWindows(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;

	// Get an exterma; pointer to this object.
	ApplicationHandle = this;

	// Get the instance of this application.
	m_hinstance = GetModuleHandle(NULL);

	// Give the application a name.
	m_applicationName = L"Engine";

	// Setup the windows class with default settings.
	wc.style = CS_HREDRAW|CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc =WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	// Register the window class.
	RegisterClassEx(&wc);

	//Determine the resolution of the clients desktop screen.
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// Setup the screen settings depending ion whether it is running in full screen or in windowed mode.
	if(FULL_SCREEN)
	{
		// If Full screen set the screem to maximum size of the isers desktop and 32bit.
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL|DM_PELSWIDTH | DM_PELSHEIGHT;

		// Change the displau settings to full screen.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// Set the pisition of the window to the top left corner.
		posX=posY=0;
	}
	else
	{
		// If windowed then set it to 800x600 resolution.
		screenWidth = 800;
		screenHeight = 600;

		// Place the window in the middle of the screen.
		posX=(GetSystemMetrics(SM_CXSCREEN)-screenWidth)/2;
		posY = (GetSystemMetrics(SM_CYSCREEN)-screenHeight)/2;
	}

	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
		WS_CLIPSIBLINGS |WS_CLIPCHILDREN|WS_POPUP,
		posX, posY, screenWidth, screenHeight,NULL, NULL, m_hinstance, NULL);

	// Create the window with the screen and set it as main focus.
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	// Hide the mouse cursor.
	ShowCursor(false);

	return;
}

void SystemClass::ShutdownWindows()
{
	// Show the mouse cursor.
	ShowCursor(true);

	// Fix the display settings if leaving full screen mode.
	if(FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	// Remvoe the window/
	DestroyWindow(m_hwnd);
	m_hwnd=NULL;

	// Remove the applicaiton instance.
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance=NULL;

	// Release the pointer to this class.
	ApplicationHandle = NULL;

	return;

}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch(umessage)
	{

		// Check if the window is being destroyed.
	case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}

		// Check if the window is being closed.
	case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}

		// All other messages pass to the message handler in the system class.
	default:
		{
			return ApplicationHandle->MessageHandler(hwnd,umessage, wparam, lparam);
		}
	}
}