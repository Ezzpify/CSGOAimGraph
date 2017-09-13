#include "tier2/tier2.h"
#include "engine/iserverplugin.h"
#include "game/server/iplayerinfo.h"
#include "convar.h"

//I have to do this to make it load as a plugin :|

class pluginCrap : public IServerPluginCallbacks {
public:
	pluginCrap();

	virtual bool			Load( CreateInterfaceFn interfaceFactory, CreateInterfaceFn gameServerFactory );
	virtual void			Unload( void );
	virtual void			Pause( void );
	virtual void			UnPause( void );
	virtual const char     *GetPluginDescription( void );      
	virtual void			LevelInit( char const *pMapName );
	virtual void			ServerActivate( edict_t *pEdictList, int edictCount, int clientMax );
	virtual void			GameFrame( bool simulating );
	virtual void			LevelShutdown( void );
	virtual void			ClientActive( edict_t *pEntity );
	virtual void			ClientFullyConnect( edict_t *pEntity );
	virtual void			ClientDisconnect( edict_t *pEntity );
	virtual void			ClientPutInServer( edict_t *pEntity, char const *playername );
	virtual void			SetCommandClient( int index );
	virtual void			ClientSettingsChanged( edict_t *pEdict );
	virtual PLUGIN_RESULT	ClientConnect( bool *bAllowConnect, edict_t *pEntity, const char *pszName, const char *pszAddress, char *reject, int maxrejectlen );
	virtual PLUGIN_RESULT	ClientCommand( edict_t *pEntity, const CCommand &args );
	virtual PLUGIN_RESULT	NetworkIDValidated( const char *pszUserName, const char *pszNetworkID );
	virtual void			OnQueryCvarValueFinished( QueryCvarCookie_t iCookie, edict_t *pPlayerEntity, EQueryCvarValueStatus eStatus, const char *pCvarName, const char *pCvarValue );
	virtual void			OnEdictAllocated( edict_t *edict );
	virtual void			OnEdictFreed( const edict_t *edict  );
	virtual bool			BNetworkCryptKeyCheckRequired(uint32 unFromIP, uint16 usFromPort, uint32 unAccountIdProvidedByClient,
		bool bClientWantsToUseCryptKey);

	// BNetworkCryptKeyValidate allows the server to validate client's over the wire encrypted payload cookie and return
	// false if the client cookie is malformed to prevent connection to the server. If this function returns true then
	// the plugin allows the client to connect with the encryption key, and upon return the engine expects the plugin
	// to have copied 16-bytes of client encryption key into the buffer pointed at by pbPlainTextKeyForNetChan. That key
	// must match the plaintext key supplied by 3rd party client software to the client game process, not the client cookie
	// transmitted unencrypted over the wire as part of the connection packet.
	// Any plugin in the chain that returns true will stop evaluation of other plugins and the 16-bytes encryption key
	// copied into pbPlainTextKeyForNetchan will be used. If a plugin returns false then evaluation of other plugins will
	// continue and buffer data in pbPlainTextKeyForNetchan will be preserved from previous calls.
	// If no plugin returns true and the encryption key is required then the client connection will be rejected with
	// an invalid certificate error.
	virtual bool			BNetworkCryptKeyValidate(uint32 unFromIP, uint16 usFromPort, uint32 unAccountIdProvidedByClient,
		int nEncryptionKeyIndexFromClient, int numEncryptedBytesFromClient, byte *pbEncryptedBufferFromClient,
		byte *pbPlainTextKeyForNetchan);
};

pluginCrap::pluginCrap() {
}

pluginCrap iHateValve;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR( pluginCrap, IServerPluginCallbacks, INTERFACEVERSION_ISERVERPLUGINCALLBACKS, iHateValve );

ConVar cl_demo_show_aim( "cl_demo_show_aim", "0" );
ConVar cl_demo_aim_zoom( "cl_demo_aim_zoom", "8.0" );
ConVar cl_demo_show_history("cl_demo_show_history", "0.01");

bool ClDemoShowAim() {
	return cl_demo_show_aim.GetBool();
}

float ClDemoAimZoom() {
	return cl_demo_aim_zoom.GetFloat();
}

float ClDemoShowHistory() {
	return (int)(2.0f / cl_demo_show_history.GetFloat()) > 255 ? 0.01 : cl_demo_show_history.GetFloat();
}

extern void Startup();
bool pluginCrap::Load( CreateInterfaceFn interfaceFactory, CreateInterfaceFn gameServerFactory ) {
	ConnectTier1Libraries( &interfaceFactory, 1 );
	//ConnectTier2Libraries( &interfaceFactory, 1 );
	ConVar_Register( 0 );

	Msg( "Demo analysis tool loaded.\n"
		"Use cl_demo_show_aim 1 to activate.\n" );

	Startup();

	return true;
}

extern void Shutdown();
void pluginCrap::Unload( void ) {
	Shutdown();

	ConVar_Unregister();
	DisconnectTier1Libraries();
}
void pluginCrap::Pause( void ) {}
void pluginCrap::UnPause( void ) {}

const char *pluginCrap::GetPluginDescription( void ) {
	return "Demo analysis tool for CS:GO";
}

//"char const*" WHO DOES THIS SHIT
void pluginCrap::LevelInit( char const *pMapName ) {}
void pluginCrap::ServerActivate( edict_t *pEdictList, int edictCount, int clientMax ) {}
void pluginCrap::GameFrame( bool simulating ) {}
void pluginCrap::LevelShutdown( void ) {}
void pluginCrap::ClientActive( edict_t *pEntity ) {}
void pluginCrap::ClientFullyConnect( edict_t *pEntity ) {}
void pluginCrap::ClientDisconnect( edict_t *pEntity ) {}
void pluginCrap::ClientPutInServer( edict_t *pEntity, char const *playername ) {}
void pluginCrap::SetCommandClient( int index ) {}
void pluginCrap::ClientSettingsChanged( edict_t *pEdict ) {}
PLUGIN_RESULT pluginCrap::ClientConnect( bool *bAllowConnect, edict_t *pEntity, const char *pszName, const char *pszAddress, char *reject, int maxrejectlen ) { return PLUGIN_CONTINUE; }
PLUGIN_RESULT pluginCrap::ClientCommand( edict_t *pEntity, const CCommand &args ) { return PLUGIN_OVERRIDE; }
PLUGIN_RESULT pluginCrap::NetworkIDValidated( const char *pszUserName, const char *pszNetworkID ) { return PLUGIN_CONTINUE; }
void pluginCrap::OnQueryCvarValueFinished( QueryCvarCookie_t iCookie, edict_t *pPlayerEntity, EQueryCvarValueStatus eStatus, const char *pCvarName, const char *pCvarValue ) {}
void pluginCrap::OnEdictAllocated( edict_t *edict ) {}
void pluginCrap::OnEdictFreed( const edict_t *edict  ) {}
bool pluginCrap::BNetworkCryptKeyCheckRequired(uint32 unFromIP, uint16 usFromPort, uint32 unAccountIdProvidedByClient, bool bClientWantsToUseCryptKey) {return true;}
bool pluginCrap::BNetworkCryptKeyValidate(uint32 unFromIP, uint16 usFromPort, uint32 unAccountIdProvidedByClient, int nEncryptionKeyIndexFromClient, int numEncryptedBytesFromClient, byte *pbEncryptedBufferFromClient, byte *pbPlainTextKeyForNetchan){return true;}