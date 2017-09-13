#pragma once
#include <cinttypes>

uintptr_t Sigscan( const uintptr_t startAddr, const uintptr_t endAddr, const uint8_t *pattern, const uint8_t *mask );