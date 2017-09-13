#pragma once

#include <cinttypes>

#define PREPROCESSOR_REALLY_SUCKS( x, y ) x##y
#define PREPROCESSOR_SUCKS( x, y ) PREPROCESSOR_REALLY_SUCKS( x, y )

#define PAD( size ) \
private: \
	uint8_t PREPROCESSOR_SUCKS( pad, __COUNTER__ )[ size ]; \
public: