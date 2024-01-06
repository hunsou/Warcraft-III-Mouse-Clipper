// Warcraft III Mouse Clipper by B4rt0 v1.0.0
// Code licensed under CC BY 4.0 (http://creativecommons.org/licenses/by/4.0/)

#include <windows.h>
#include <iostream>

#pragma comment(lib,"User32.lib")

RECT clipArea;
HWND activeWindow, W3window;
bool active = false;
int offsetTop, offsetRight, offsetBottom, offsetLeft, refreshRate, silentMode;

void readINI() {
	offsetTop = GetPrivateProfileInt((LPCSTR)"ClipAreaOffset",(LPCSTR)"offsetTop", 31, (LPCSTR)".\\settings.ini");
	offsetLeft = GetPrivateProfileInt((LPCSTR)"ClipAreaOffset",(LPCSTR)"offsetLeft", 8, (LPCSTR)".\\settings.ini");
	offsetRight = GetPrivateProfileInt((LPCSTR)"ClipAreaOffset",(LPCSTR)"offsetRight", -8, (LPCSTR)".\\settings.ini");	
	offsetBottom = GetPrivateProfileInt((LPCSTR)"ClipAreaOffset",(LPCSTR)"offsetBottom", -8, (LPCSTR)".\\settings.ini");
	refreshRate = GetPrivateProfileInt((LPCSTR)"GeneralSettings",(LPCSTR)"refreshRate", 500, (LPCSTR)".\\settings.ini");
	silentMode = GetPrivateProfileInt((LPCSTR)"GeneralSettings",(LPCSTR)"silentMode", 1, (LPCSTR)".\\settings.ini");
}

void LockMouse() {
	GetWindowRect( W3window, &clipArea);
	clipArea.top = clipArea.top + offsetTop;
	clipArea.left = clipArea.left + offsetLeft;
	clipArea.right = clipArea.right + offsetRight;
	clipArea.bottom = clipArea.bottom - offsetBottom;
	
	//20240103参考WC3-Mousecapture------------------------
	RECT windowRect, clientRect;
	HWND hWnd = W3window;
	GetWindowRect(hWnd, &windowRect);
	GetClientRect(hWnd, &clientRect);

	// Calculate border size
	int borderSize = (windowRect.right - windowRect.left - clientRect.right) / 2;

	// Calculate titlebar size
	int titlebarSize = windowRect.bottom - windowRect.top - clientRect.bottom - borderSize;

	// Calculate new coordinates
	windowRect.top += titlebarSize;
	windowRect.left += borderSize;
	windowRect.bottom -= borderSize;
	windowRect.right -= borderSize;
	//--------------------------------------------------
	
	SetForegroundWindow(W3window);
	//ClipCursor( &clipArea );
	ClipCursor( &windowRect );
	active = true;
}

void UnlockMouse() {
	ClipCursor( NULL );
	active = false;
}

int main() {
	readINI();
	if(silentMode==1) {
		FreeConsole(); // Hide console window
	}
	else {
		std::cout<<"Warcraft III Mouse Clipper by B4rt0\n";
	}
	
	const char szUniqueNamedMutex[] = "b4rt0_w3_mouse_clipper";
   	HANDLE hHandle = CreateMutex( NULL, TRUE, szUniqueNamedMutex );
	if( ERROR_ALREADY_EXISTS == GetLastError() )
	{
	  MessageBox(NULL, (LPCSTR)"An instance of Warcraft III Mouse Clipper is already running.", (LPCSTR)"Warcraft III Mouse Clipper", MB_ICONWARNING);
	  return(1); // Exit program
	}
		
	W3window = FindWindow(NULL, "Warcraft III");
	
	if(W3window == NULL)
	{
		MessageBox(NULL, (LPCSTR)"Warcraft III window not found.\nThe application will close itself.", (LPCSTR)"Warcraft III Mouse Clipper", MB_ICONERROR);
		ReleaseMutex( hHandle ); // Explicitly release mutex
   		CloseHandle( hHandle ); // close handle before terminating
		return 0;
	}
	
	
	
	for (;;) {
		activeWindow = GetForegroundWindow();
		if(activeWindow == W3window && active == false) {
			LockMouse();
		}
		else if(activeWindow != W3window && active == true) {
			UnlockMouse();
		}
		if(IsWindow(W3window) == 0) // Close itself if game isn't open
		{
			ReleaseMutex( hHandle ); // Explicitly release mutex
   			CloseHandle( hHandle ); // close handle before terminating
			return 0;
		}
		Sleep(refreshRate);
	}

	ReleaseMutex( hHandle ); // Explicitly release mutex
	CloseHandle( hHandle ); // close handle before terminating
	return 0;
}