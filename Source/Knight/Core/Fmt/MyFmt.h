#pragma once

#define FMT_HEADER_ONLY 1
#define FMT_EXCEPTIONS 0

#if _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4702 ) // unreachable code
#endif
#include "fmt/core.h"

#if _MSC_VER
#pragma warning( pop )
#endif

#define MY_FMT(toFormat, ...) \
		FString(fmt::format(toFormat, ##__VA_ARGS__).c_str())