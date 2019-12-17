#include "XboxController.h"

XINPUT_STATE XboxController::GetState()
{
	ZeroMemory(&controllerState, sizeof(XINPUT_STATE));
	XInputGetState(0, &controllerState);
	return controllerState;
}

bool XboxController::IsConnected()
{
	ZeroMemory(&controllerState, sizeof(XINPUT_STATE));
	return XInputGetState(0, &controllerState) == ERROR_SUCCESS;
}

void XboxController::ShortVibrate()
{
	XINPUT_VIBRATION Vibration;

	Vibration.wLeftMotorSpeed = 65535;
	Vibration.wRightMotorSpeed = 65535;
	XInputSetState(0, &Vibration);

	Sleep(250);

	Vibration.wLeftMotorSpeed = 0;
	Vibration.wRightMotorSpeed = 0;
	XInputSetState(0, &Vibration);
}
