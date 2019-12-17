#ifndef _XBOX_CONTROLLER_H_
#define _XBOX_CONTROLLER_H_

// No MFC
#define WIN32_LEAN_AND_MEAN

// We need the Windows Header and the XInput Header
#include <windows.h>
#include <XInput.h>

// Now, the XInput Library
#pragma comment(lib, "XInput.lib")

// XBOX Controller Class Definition
class XboxController
{
private:
	XINPUT_STATE controllerState;
public:
	XINPUT_STATE GetState();
	bool IsConnected();
	void ShortVibrate();
};

#endif