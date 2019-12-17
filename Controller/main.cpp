#include "XboxController.h"
#include <iostream>
#include <mmdeviceapi.h>
#include <endpointvolume.h>

constexpr auto DEAD_ZONE = 6000;
constexpr auto FPS = 60;
constexpr auto SPEED = 0.000125f;
constexpr auto VOLUME_UP = 0.2f;
constexpr auto VOLUME_DOWN = -0.2f;

void ChangeVolume(const float& value)
{
	IMMDeviceEnumerator* deviceEnumerator = NULL;
	IMMDevice* defaultDevice = NULL;
	IAudioEndpointVolume* endpointVolume = NULL;
	float currentVolume = 0;
	auto clean = [](auto& pointer) {pointer->Release(); pointer = NULL; };
	
	HRESULT x = CoInitialize(NULL);
	x = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator), (LPVOID*)&deviceEnumerator);
	deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultDevice);
	defaultDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, NULL, (LPVOID*)&endpointVolume);

	endpointVolume->GetMasterVolumeLevelScalar(&currentVolume);
	endpointVolume->SetMasterVolumeLevelScalar(currentVolume + value, NULL);

	clean(deviceEnumerator);
	clean(defaultDevice);
	clean(endpointVolume);
	CoUninitialize();
}

float CalculatePosition(const short& inputThumb, const float& length)
{
	return length > DEAD_ZONE* DEAD_ZONE ? (inputThumb * SPEED * (sqrt(length) - DEAD_ZONE)) / ((MAXSHORT - DEAD_ZONE) * FPS) : 0.0f;
}

int main()
{
	ShowWindow(GetConsoleWindow(), SW_MINIMIZE);

	#pragma region Variables

	float restX = 0.0f;
	float restY = 0.0f;

	bool mouseEnabled = false;

	INPUT mouseEvents[2] = { 0 };
	mouseEvents[0].type = INPUT_MOUSE;
	mouseEvents[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
	mouseEvents[1].type = INPUT_MOUSE;
	mouseEvents[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;

	INPUT spacebarEvents[2] = { 0 };
	spacebarEvents[0].type = INPUT_KEYBOARD;
	spacebarEvents[0].ki.wVk = VK_SPACE;
	spacebarEvents[1].ki.dwFlags = KEYEVENTF_KEYUP;

	std::unique_ptr<XboxController> xboxController = std::make_unique<XboxController>();
	
	#pragma endregion

	while (!(xboxController->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_BACK))
	{
		if (xboxController->IsConnected())
		{
			//Change mouseEnabled
			if (xboxController->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB)
			{
				xboxController->ShortVibrate();
				Sleep(250);
				mouseEnabled = !mouseEnabled;
			}

			//Volume Down
			if (xboxController->GetState().Gamepad.bLeftTrigger > 0)
			{
				ChangeVolume(VOLUME_DOWN);
				Sleep(250);
			}

			//Volume Up
			if (xboxController->GetState().Gamepad.bRightTrigger > 0)
			{
				ChangeVolume(VOLUME_UP);
				Sleep(250);
			}

			//Left Mouse Click
			if (xboxController->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_A)
			{
				SendInput(2, mouseEvents, sizeof(INPUT));
				Sleep(250);
			}

			//Spacebar Click
			if (xboxController->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_B)
			{
				SendInput(2, spacebarEvents, sizeof(INPUT));
				Sleep(250);
			}    

			//Mouse Move
			if (mouseEnabled)
			{
				POINT cursor;
				GetCursorPos(&cursor);

				short thumbPosX = xboxController->GetState().Gamepad.sThumbLX;
				short thumbPosY = xboxController->GetState().Gamepad.sThumbLY;
				float length = float(thumbPosX * thumbPosX + thumbPosY * thumbPosY);

				float x = cursor.x + restX + CalculatePosition(thumbPosX, length);
				float y = cursor.y + restY - CalculatePosition(thumbPosY, length);

				restX = x - (float)((int)x);
				restY = y - (float)((int)y);

				SetCursorPos((int)x, (int)y);
			}
		}
		else
		{
			Sleep(1000);
		}
	}
	
	return 0;
}