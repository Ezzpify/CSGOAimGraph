#include "globalVars.h"
#include "userCmd.h"

#include "sigscan.h"

#include <Windows.h>
#include <Psapi.h>
#include <cinttypes>
#include <detours.h>
#include <d3d9.h>
#include <math.h>

#define HISTORY_TIME 2.0f
int historyTicks = 1;
float angleDistXHistory[ ( int )( 128 * HISTORY_TIME ) ];
float angleDistYHistory[ ( int )( 128 * HISTORY_TIME ) ];
angle lastViewAngles = { 0 };
int historySeq = 0;
extern float ClDemoShowHistory();

void hReadUsercmd_Wrapped( const userCmd *move ) {
	historyTicks = (int)(HISTORY_TIME / ClDemoShowHistory());// globals->intervalPerTick );

	angleDistXHistory[ historySeq % historyTicks ] = fmodf( fmodf( move->viewAngles[ 0 ] - lastViewAngles[ 0 ] + 180.0f, 360.0f ) + 360.0f, 360.0f ) - 180.0f;
	angleDistYHistory[ historySeq % historyTicks ] = fmodf( fmodf( move->viewAngles[ 1 ] - lastViewAngles[ 1 ] + 180.0f, 360.0f ) + 360.0f, 360.0f ) - 180.0f;
	lastViewAngles[ 0 ] = move->viewAngles[ 0 ];
	lastViewAngles[ 1 ] = move->viewAngles[ 1 ];
	historySeq++;
}

void *oReadUsercmd;
//Like __fastcall but caller stack cleanup
__declspec(naked) void /*__fastcall*/ hReadUsercmd( /*void *buf, userCmd *move, userCmd *from*/) {
	__asm {
		mov eax, [esp + 4]

			push edx
			push ecx

			push eax
			call oReadUsercmd
			add esp, 4

			pop ecx
			pop edx

			push edx
			call hReadUsercmd_Wrapped
			add esp, 4

			retn
	}
}

#define AIM_RADAR_SIZE 500

extern bool ClDemoShowAim();
extern float ClDemoAimZoom();

struct CUSTOMVERTEX
{
	float x, y, z;
};

typedef HRESULT( __stdcall *tEndScene )( IDirect3DDevice9 *device );
tEndScene oEndScene;
void __stdcall hEndScene( IDirect3DDevice9 *device ) {
	if( !ClDemoShowAim() ) {
		oEndScene( device );
		return;
	}
		 
	D3DRECT bgRect = {
		0,
		0,
		AIM_RADAR_SIZE,
		AIM_RADAR_SIZE,
	};
	device->Clear(1, &bgRect, D3DCLEAR_TARGET, D3DCOLOR_ARGB(255, 0, 0, 0), 0.0f, 0);

	int idx = historySeq % historyTicks;
	for( int i = 0; i < historyTicks; i++ ) {
		int offsetX = max( -AIM_RADAR_SIZE / 2, min( AIM_RADAR_SIZE / 2, ( int )( angleDistYHistory[ idx ] * AIM_RADAR_SIZE / 180.0f * ClDemoAimZoom() ) * -1 ) );
		int offsetY = max( -AIM_RADAR_SIZE / 2, min( AIM_RADAR_SIZE / 2, ( int )( angleDistXHistory[ idx ] * AIM_RADAR_SIZE / 180.0f * ClDemoAimZoom() ) ) );
		D3DRECT rect = {
			AIM_RADAR_SIZE / 2 + offsetX - 1,
			AIM_RADAR_SIZE / 2 + offsetY - 1,
			AIM_RADAR_SIZE / 2 + offsetX + 2,
			AIM_RADAR_SIZE / 2 + offsetY + 2,
		};
		CUSTOMVERTEX Vertices[] =
		{
			{ -5.0, -5.0, 0.0 },
		};

		if( i == historyTicks - 1 ) {
			device->Clear( 1, &rect, D3DCLEAR_TARGET, D3DCOLOR_ARGB( 255, 255, 255, 255 ), 0.0f, 0 );
		}
		else {
			uint8_t green = ( int )( ( float )( i ) / historyTicks * 255.0f );
			device->Clear( 1, &rect, D3DCLEAR_TARGET, D3DCOLOR_ARGB( 255, 0, green, 0 ), 0.0f, 0 );
		}

		idx = ( idx + 1 ) % historyTicks;
	}

	D3DRECT dotRect = {
		AIM_RADAR_SIZE / 2 - 1,
		AIM_RADAR_SIZE / 2 - 1,
		AIM_RADAR_SIZE / 2 + 2,
		AIM_RADAR_SIZE / 2 + 2,
	};
	device->Clear( 1, &dotRect, D3DCLEAR_TARGET, D3DCOLOR_ARGB( 255, 0, 0, 255 ), 0.0f, 0 );

	oEndScene( device );
}

uintptr_t readUserCmdPtr;
void *endScenePtr;

void Startup() {
	MODULEINFO clientModInfo;
	MODULEINFO engineModInfo;
	const HMODULE clientModule = GetModuleHandle( "client.dll" );
	const HMODULE engineModule = GetModuleHandle( "engine.dll" );
	GetModuleInformation( GetCurrentProcess(), clientModule, &clientModInfo, sizeof( MODULEINFO ) );
	GetModuleInformation( GetCurrentProcess(), engineModule, &engineModInfo, sizeof( MODULEINFO ) );
	const uintptr_t clientStart = ( uintptr_t )( clientModInfo.lpBaseOfDll );
	const uintptr_t engineStart = ( uintptr_t )( engineModInfo.lpBaseOfDll );
	const uintptr_t clientEnd = clientStart + ( uintptr_t )( clientModInfo.SizeOfImage );
	const uintptr_t engineEnd = engineStart + (uintptr_t)(engineModInfo.SizeOfImage);

	//globals = **( globalVars*** )( Sigscan( engineStart, engineEnd,
	//	( const uint8_t* )( "\xA1\x00\x00\x00\x00\x89\x5E\x04" ),
	//	( const uint8_t* )( "x????xxx" ) ) + 1 );
	
	/*globals = **( globalVars*** )( Sigscan( engineStart, engineEnd,
		( const uint8_t* )( "\x68\x00\x00\x00\x00\xFF\xD7" ),
		( const uint8_t* )( "x????xx" ) ) + 5 );*/

	readUserCmdPtr = Sigscan( clientStart, clientEnd,
		( const uint8_t* )( "\x55\x8B\xEC\x83\xEC\x08\x53\x8B\x5D\x08\x8B\xC2\x56\x57\x8B\xF1" ),
		( const uint8_t* )( "xxxxxxxxxxxxxxxx" ) );

	oReadUsercmd = DetourFunction( ( byte* )( readUserCmdPtr ), ( byte* )( hReadUsercmd ) );

	MODULEINFO d3dModInfo;
	const HMODULE d3dModule = GetModuleHandle( "d3d9.dll" );
	GetModuleInformation( GetCurrentProcess(), d3dModule, &d3dModInfo, sizeof( MODULEINFO ) );
	const uintptr_t d3dStart = ( uintptr_t )( d3dModInfo.lpBaseOfDll );
	const uintptr_t d3dEnd = d3dStart + ( uintptr_t )( d3dModInfo.SizeOfImage );
		
	void **deviceVtable = *( void*** )( Sigscan( d3dStart, d3dEnd,
		( const uint8_t* )( "\xC7\x06\x00\x00\x00\x00\x89\x86\x00\x00\x00\x00\x89\x86" ),
		( const uint8_t* )( "xx????xx????xx" ) ) + 2 );

	endScenePtr = deviceVtable[ 42 ];
	oEndScene = ( tEndScene )( DetourFunction( ( byte* )( endScenePtr ), ( byte* )( hEndScene ) ) );
}

void Shutdown() {
	DetourRemove( ( byte* )( oReadUsercmd ), ( byte* )( readUserCmdPtr ) );
	DetourRemove( ( byte* )( oReadUsercmd ), ( byte* )( endScenePtr ) );
}