#pragma once

class vftHooks {
public:
	void ***target;
	void **origVtable;
	void **newVtable;
	int tableSize;

	vftHooks();
	~vftHooks();
	vftHooks( const void *hook_target );

	inline void Hook( const int index, void *hookFunc ) {
		newVtable[ index + 1 ] = hookFunc;
	}

	inline void Unhook( const int index ) {
		newVtable[ index + 1 ] = origVtable[ index ];
	}

	inline void UnhookAll() {
		*target = origVtable;
	}

	inline bool Initialized() {
		return tableSize > 0;
	}

	template< typename FuncPrototype >
	FuncPrototype GetOrigFunction( const int index ) {
		return reinterpret_cast< FuncPrototype >( origVtable[ index ] );
	}
};