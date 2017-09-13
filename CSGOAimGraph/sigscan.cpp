#include <cinttypes>

uintptr_t Sigscan( const uintptr_t startAddr, const uintptr_t endAddr, const uint8_t *pattern, const uint8_t *mask ) {
	int patternIdx = 0;
	uintptr_t scanAddr = startAddr;
	do {
		if( mask[ patternIdx ] == 0 ) {
			return scanAddr - patternIdx;
		}
		else if( mask[ patternIdx ] != '?' ) {
			if( *( uint8_t* )( scanAddr ) == pattern[ patternIdx ] ) {
				patternIdx++;
			}
			else {
				patternIdx = 0;
			}
		}
		else {
			patternIdx++;
		}
	} while( ++scanAddr < endAddr );

	return 0;
}