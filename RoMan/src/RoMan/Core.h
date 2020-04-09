#pragma once

#include <memory>

#ifdef RM_PLATFORM_WINDOWS
#ifdef RM_DYNAMIC_LINK
	#ifdef RM_BUILD_DLL
		#define ROMAN_API _declspec(dllexport)
	#else
		#define ROMAN_API _declspec(dllimport)
	#endif
#else
	#define ROMAN_API
#endif
#else
	#error RoMan supports only Windows!
#endif

#ifdef RM_DEBUG
	#define RM_ENABLE_ASSERTS
#endif

#ifdef RM_ENABLE_ASSERTS
	#define RM_ASSERT(x, ...) { if(!(x)) { RM_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#define RM_CORE_ASSERT(x, ...) { if(!(x)) { RM_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define RM_ASSERT(x, ...)
	#define RM_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)

#define RM_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

namespace RoMan
{
	template<typename T>
	using Scope = std::unique_ptr<T>;

	template<typename T>
	using Ref = std::shared_ptr<T>;
}