#pragma once

#include "structMacros.h"
#include <cstddef>

#define MULTIPLAYER_BACKUP 150

typedef float angle[ 3 ];

struct userCmd {
	PAD( 0x4 );
	int cmdNum;
	int tickCount;
	angle viewAngles;
	PAD( 0xC );
	float forwardMove;
	float sideMove;
	float upMove;
	int buttons;
	int impulse;
	int weapSelect;
	int weapSubtype;
	int randSeed;
	short mouseDx;
	short mouseDy;
	PAD( 0x1C );
};

static_assert( offsetof( userCmd, cmdNum ) == 0x4, "" );
static_assert( offsetof( userCmd, tickCount ) == 0x8, "" );
static_assert( offsetof( userCmd, viewAngles ) == 0xC, "" );
static_assert( offsetof( userCmd, forwardMove ) == 0x24, "" );
static_assert( offsetof( userCmd, sideMove ) == 0x28, "" );
static_assert( offsetof( userCmd, upMove ) == 0x2C, "" );
static_assert( offsetof( userCmd, buttons ) == 0x30, "" );
static_assert( offsetof( userCmd, impulse ) == 0x34, "" );
static_assert( offsetof( userCmd, weapSelect ) == 0x38, "" );
static_assert( offsetof( userCmd, weapSubtype ) == 0x3C, "" );
static_assert( offsetof( userCmd, randSeed ) == 0x40, "" );
static_assert( offsetof( userCmd, mouseDx ) == 0x44, "" );
static_assert( offsetof( userCmd, mouseDy ) == 0x46, "" );
static_assert( sizeof( userCmd ) == 0x64, "" );