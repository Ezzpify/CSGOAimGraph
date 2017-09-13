#include "vftHooks.h"
#include <Windows.h>

vftHooks::vftHooks() {
	target = nullptr;
	origVtable = nullptr;
	newVtable = nullptr;
	tableSize = 0;
}

vftHooks::~vftHooks() {
	if( newVtable ) {
		delete[] newVtable;
	}
}

vftHooks::vftHooks( const void *hookTarget ) {
	target = ( void*** )( hookTarget );
	origVtable = *target;

	MEMORY_BASIC_INFORMATION mbi;
	tableSize = -1;
	do
	{
		tableSize++;
		VirtualQuery( origVtable[ tableSize ], &mbi, sizeof( MEMORY_BASIC_INFORMATION ) );
	}
	while( mbi.Protect == PAGE_EXECUTE_READ || mbi.Protect == PAGE_EXECUTE_READWRITE );

	newVtable = new void*[ tableSize + 1 ];
	newVtable[ 0 ] = origVtable[ -1 ]; //RTTI
	for( int i = 0; i < tableSize; i++ ) {
		newVtable[ i + 1 ] = origVtable[ i ];
	}

	*target = newVtable + 1;
}