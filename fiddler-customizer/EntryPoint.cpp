#include "pch.h"
#include <Windows.h>
#include "FiddlerCustomizer.h"

extern "C" __declspec(dllexport) void Initialize()
{
	FiddlerCustomizer^ fiddlerCustomizer = gcnew FiddlerCustomizer();
	fiddlerCustomizer->ApplyChanges();
}

DWORD WINAPI RunManaged()
{
	Initialize();
	return 0;
}

boolean initialized;

#pragma unmanaged
BOOL APIENTRY DllMain(
	HMODULE hModule,
	DWORD callReason,
	LPVOID lpReserved
)
{
	if (callReason == DLL_PROCESS_ATTACH)
	{
		if (!initialized)
		{
			initialized = true;
			CreateThread(
				nullptr,
				0,
				LPTHREAD_START_ROUTINE(RunManaged),
				nullptr,
				0,
				nullptr
			);
		}
	}
	else if (callReason == DLL_PROCESS_DETACH)
	{
		//
	}
	return TRUE;
}
