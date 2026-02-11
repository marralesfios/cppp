#pragma once
#ifdef NDEBUG
#define CPPP_ASSERT(...) [[assume(__VA_ARGS__)]]
#else
#include<cassert>
#define CPPP_ASSERT(...) assert(__VA_ARGS__)
#endif
