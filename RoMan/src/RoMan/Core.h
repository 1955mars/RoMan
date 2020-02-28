#pragma once

#ifdef RM_PLATFORM_WINDOWS
	#ifdef RM_BUILD_DLL
		#define ROMAN_API _declspec(dllexport)
	#else
		#define ROMAN_API _declspec(dllimport)
	#endif
#else
	#error RoMan supports only Windows!
#endif
