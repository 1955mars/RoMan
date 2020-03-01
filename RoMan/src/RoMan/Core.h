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

#ifdef RM_ENABLE_ASSERTS
	#define RM_ASSERT(x, ...) { if(!(x)) { HZ_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#define RM_CORE_ASSERT(x, ...) { if(!(x)) { HZ_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define RM_ASSERT(x, ...)
	#define RM_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)