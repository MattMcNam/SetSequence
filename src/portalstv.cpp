/*
 *  portalstv.cpp
 *  PortalSTV project
 *  
 *  Copyright (c) 2013 Matthew McNamara
 *  BSD 2-Clause License
 *  http://opensource.org/licenses/BSD-2-Clause
 *
 */

#include "portalstv.h"

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
ConVar force_sequence("tpose_force_sequence", "0", 0, "always use this sequence, not just in tpose");

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

static void get_sequence();
static ConCommand get_sequence_command("get_sequence", get_sequence, "a");
static void set_sequence();
static ConCommand set_sequence_command("tpose_hook_set_sequence", set_sequence, "Hook into the SetSequence method, required by all other tpose_ commands!");
static void dump_offsets();
static ConCommand dump_offsets_command("dump_offsets", dump_offsets, "a");
static void dump_classes();
static ConCommand dump_classes_command("dump_classes", dump_classes, "a");

// Print all netvars to console
static void dump_classes() {
	WSOffsets::DumpClasses();
}

// Print useful netvars to console
static void dump_offsets() {
	Msg("[T-POSE] WSOffets:\n");
	Msg("CBaseCombatCharacter::m_nSequence = %X\n", WSOffsets::pCBaseCombatCharacter__m_nSequence);
}

// DEBUG: Read value of nSequence from each player
static void get_sequence() {
	int iEntCount = pEntityList->GetHighestEntityIndex();
	IClientEntity *cEntity;

	for (int i = 0; i < iEntCount; i++) {
		cEntity = pEntityList->GetClientEntity(i);

		// Ensure valid player entity
		if (cEntity == NULL || !(GetGameResources()->IsConnected(i)))
			continue;

		int sequence = *MakePtr(int*, cEntity, WSOffsets::pCBaseCombatCharacter__m_nSequence);

		Msg("[T-POSE] Player entity %d has sequence %d\n", i, sequence);
	}
}

static void set_sequence() {
	int iEntCount = pEntityList->GetHighestEntityIndex();
	IClientEntity *cEntity;

	for (int i = 0; i < iEntCount; i++) {
		cEntity = pEntityList->GetClientEntity(i);

		// Ensure valid player entity
		if (cEntity == NULL || !(GetGameResources()->IsConnected(i)))
			continue;
		
		// Hook SetSequence
		if (origSetSequence == NULL) {
			origSetSequence = (void (__fastcall *)(void *, int, int))
				HookVFunc(*(DWORD**)cEntity, Index_CBaseAnimating_SetSequence, 
				(DWORD*) &hookedSetSequence);
			Msg("[T-POSE] Hooked SetSequence ????????????????\n");
		}

		//*MakePtr(bool*, cEntity, WSOffsets::pCBaseCombatCharacter__m_bClientSideAnimation) = true;
		//CBaseAnimating_SetSequence((C_BaseAnimating*)cEntity, 15);
		//*MakePtr(int*, cEntity, WSOffsets::pCBaseCombatCharacter__m_nSequence) = 15;

		//Msg("[T-POSE] Player entity %d should now have sequence 15\n", i);
	}
}

// The plugin is a static singleton that is exported as an interface
PortalSTVPlugin g_PortalSTVPlugin;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(PortalSTVPlugin, IServerPluginCallbacks, INTERFACEVERSION_ISERVERPLUGINCALLBACKS, g_PortalSTVPlugin );

/*
 * PortalSTVPlugin implementation
 * 
 */
PortalSTVPlugin::PortalSTVPlugin(){}
PortalSTVPlugin::~PortalSTVPlugin(){}

bool PortalSTVPlugin::Load( CreateInterfaceFn interfaceFactory, CreateInterfaceFn gameServerFactory )
{
	ConnectTier1Libraries( &interfaceFactory, 1 );

	// Get Source interfaces from client.dll
	void* hmClient = GetHandleOfModule("client");
	CreateInterfaceFn pfnClient = (CreateInterfaceFn) GetFuncAddress(hmClient, "CreateInterface");
	pClient     = (IBaseClientDLL*)    pfnClient("VClient017", NULL);
	pEntityList = (IClientEntityList*) pfnClient("VClientEntityList003", NULL);

	// Scan for netvar offsets
	WSOffsets::PrepareOffsets();

	// Register cvars & concommands
	ConVar_Register( 0 );
	return true;
}
void PortalSTVPlugin::Unload( void )
{
	ConVar_Unregister( );
	DisconnectTier1Libraries();
}

const char *PortalSTVPlugin::GetPluginDescription( void )
{
	return PLUGIN_DESC;
}

// Unused
void PortalSTVPlugin::Pause( void ){}
void PortalSTVPlugin::UnPause( void ){}
void PortalSTVPlugin::LevelInit( char const *pMapName ){}
void PortalSTVPlugin::ServerActivate( edict_t *pEdictList, int edictCount, int clientMax ){}
void PortalSTVPlugin::GameFrame( bool simulating ){}
void PortalSTVPlugin::LevelShutdown( void ){}
void PortalSTVPlugin::ClientActive( edict_t *pEntity ){}
void PortalSTVPlugin::ClientDisconnect( edict_t *pEntity ){}
void PortalSTVPlugin::ClientPutInServer( edict_t *pEntity, char const *playername ){}
void PortalSTVPlugin::SetCommandClient( int index ){}
void PortalSTVPlugin::ClientSettingsChanged( edict_t *pEdict ){}
PLUGIN_RESULT PortalSTVPlugin::ClientConnect( bool *bAllowConnect, edict_t *pEntity, const char *pszName, const char *pszAddress, char *reject, int maxrejectlen ){return PLUGIN_CONTINUE;}
PLUGIN_RESULT PortalSTVPlugin::ClientCommand( edict_t *pEntity, const CCommand &args ){return PLUGIN_CONTINUE;}
PLUGIN_RESULT PortalSTVPlugin::NetworkIDValidated( const char *pszUserName, const char *pszNetworkID ){return PLUGIN_CONTINUE;}
void PortalSTVPlugin::OnQueryCvarValueFinished( QueryCvarCookie_t iCookie, edict_t *pPlayerEntity, EQueryCvarValueStatus eStatus, const char *pCvarName, const char *pCvarValue ){}
void PortalSTVPlugin::OnEdictAllocated( edict_t *edict ){}
void PortalSTVPlugin::OnEdictFreed( const edict_t *edict ){}
