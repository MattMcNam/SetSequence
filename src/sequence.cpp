/*
 *  sequence.cpp
 *  SetSequence project
 *  
 *  Copyright (c) 2013 Matthew McNamara
 *  BSD 2-Clause License
 *  http://opensource.org/licenses/BSD-2-Clause
 *
 */

#include "sequence.h"

/* 
 * Utitlity methods
 * 
 */

inline bool DataCompare( const BYTE* pData, const BYTE* bSig, const char* szMask )
{
    for( ; *szMask; ++szMask, ++pData, ++bSig)
    {
        if( *szMask == 'x' && *pData != *bSig)
            return false;
    }
	
    return ( *szMask ) == NULL;
}

// Finds a pattern at the specified address
inline DWORD FindPattern ( DWORD dwAddress, DWORD dwSize, BYTE* pbSig, const char* szMask )
{
    for( DWORD i = NULL; i < dwSize; i++ )
    {
        if( DataCompare( (BYTE*) ( dwAddress + i ), pbSig, szMask ) )
            return (DWORD)( dwAddress + i );
    }
    return 0;
}

IGameResources* GetGameResources() {
	//IGameResources* res;
    static DWORD funcadd = NULL;
    if( !funcadd )
        funcadd = FindPattern( (DWORD) GetHandleOfModule( _T("client") ), 0x2680C6, (PBYTE) "\xA1\x00\x00\x00\x00\x85\xC0\x74\x06\x05", "x????xxxxx" );
        
    typedef IGameResources* (*GGR_t) (void);
    GGR_t GGR = (GGR_t) funcadd;
    return GGR();
}

/*
 * cvars & hooked SetSequence method
 */
ConVar console_logging("tpose_enable_console_logging", "0", 0, "When Chell's animation sequence changes, print ID to console?");
ConVar use_sequence("tpose_use_sequence", "17", 0, "ID to overwrite T-pose with, defaults to standing idle");
ConVar force_sequence("tpose_force_sequence", "0", 0, "Always override the sequence, not just Tpose");

void (__fastcall *origSetSequence)(void* thisptr, int edx, int);
void __fastcall hookedSetSequence( void *thisPtr, int edx, int nSequence ) {
	int seq = nSequence;

	if (console_logging.GetBool())
		Msg("[T-POSE] Game wants to set sequence to %d\n", seq);

	if (seq == 0 || force_sequence.GetBool()) {
		seq = use_sequence.GetInt();
	}
	
	origSetSequence(thisPtr, edx, seq);
}

/*
 * commands, just hook command for now
 */

static void hook_set_sequence();
static ConCommand set_sequence_command("tpose_hook_set_sequence", hook_set_sequence, "Hook into the SetSequence method, required by all other tpose_ commands!");

static void hook_set_sequence() {
	int iEntCount = pEntityList->GetHighestEntityIndex();
	IClientEntity *cEntity;

	if (origSetSequence != NULL) {
		Msg("[T-POSE] SetSequence already hooked\n");
		return;
	}

	for (int i = 0; i < iEntCount; i++) {
		cEntity = pEntityList->GetClientEntity(i);

		// Ensure valid player entity
		if (cEntity == NULL || !(GetGameResources()->IsConnected(i)))
			continue;
		
		// Hook SetSequence
		origSetSequence = (void (__fastcall *)(void *, int, int))
			HookVFunc(*(DWORD**)cEntity, Index_CBaseAnimating_SetSequence, 
			(DWORD*) &hookedSetSequence);
		Msg("[T-POSE] Hooked SetSequence\n");
		return;
	}

	Msg("[T-POSE] Didn't hook SetSequence, are you in a map or demo?\n");
}

// The plugin is a static singleton that is exported as an interface
SequencePlugin g_SequencePlugin;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(SequencePlugin, IServerPluginCallbacks, INTERFACEVERSION_ISERVERPLUGINCALLBACKS, g_SequencePlugin );

/*
 * SequencePlugin implementation
 * 
 */
SequencePlugin::SequencePlugin(){}
SequencePlugin::~SequencePlugin(){}

bool SequencePlugin::Load( CreateInterfaceFn interfaceFactory, CreateInterfaceFn gameServerFactory )
{
	ConnectTier1Libraries( &interfaceFactory, 1 );

	// Get Source interfaces from client.dll
	void* hmClient = GetHandleOfModule("client");
	CreateInterfaceFn pfnClient = (CreateInterfaceFn) GetFuncAddress(hmClient, "CreateInterface");
	pClient     = (IBaseClientDLL*)    pfnClient("VClient017", NULL);
	pEntityList = (IClientEntityList*) pfnClient("VClientEntityList003", NULL);

	// Register cvars & concommands
	ConVar_Register( 0 );
	return true;
}
void SequencePlugin::Unload( void )
{
	// TODO: Reset hook
	ConVar_Unregister( );
	DisconnectTier1Libraries();
}

const char *SequencePlugin::GetPluginDescription( void )
{
	return PLUGIN_DESC;
}

// Unused
void SequencePlugin::Pause( void ){}
void SequencePlugin::UnPause( void ){}
void SequencePlugin::LevelInit( char const *pMapName ){}
void SequencePlugin::ServerActivate( edict_t *pEdictList, int edictCount, int clientMax ){}
void SequencePlugin::GameFrame( bool simulating ){}
void SequencePlugin::LevelShutdown( void ){}
void SequencePlugin::ClientActive( edict_t *pEntity ){}
void SequencePlugin::ClientDisconnect( edict_t *pEntity ){}
void SequencePlugin::ClientPutInServer( edict_t *pEntity, char const *playername ){}
void SequencePlugin::SetCommandClient( int index ){}
void SequencePlugin::ClientSettingsChanged( edict_t *pEdict ){}
PLUGIN_RESULT SequencePlugin::ClientConnect( bool *bAllowConnect, edict_t *pEntity, const char *pszName, const char *pszAddress, char *reject, int maxrejectlen ){return PLUGIN_CONTINUE;}
PLUGIN_RESULT SequencePlugin::ClientCommand( edict_t *pEntity, const CCommand &args ){return PLUGIN_CONTINUE;}
PLUGIN_RESULT SequencePlugin::NetworkIDValidated( const char *pszUserName, const char *pszNetworkID ){return PLUGIN_CONTINUE;}
void SequencePlugin::OnQueryCvarValueFinished( QueryCvarCookie_t iCookie, edict_t *pPlayerEntity, EQueryCvarValueStatus eStatus, const char *pCvarName, const char *pCvarValue ){}
void SequencePlugin::OnEdictAllocated( edict_t *edict ){}
void SequencePlugin::OnEdictFreed( const edict_t *edict ){}
