/*
 *  vfuncs.h
 *  WebSpec project
 *  
 *  Copyright (c) 2013 Matthew McNamara
 *  BSD 2-Clause License
 *  http://opensource.org/licenses/BSD-2-Clause
 *
 */

#ifndef VFUNCS_H
#define VFUNCS_H

#define CLIENT_DLL

#include "tier0/platform.h"
#include "cbase.h"

class WSEmptyClass {};

//class CBaseCombatCharacter;

#if defined( _POSIX )
#define Index_CBaseCombatCharacter_UpdateGlowEffect 284 // client.so & .dylib
#else
#define Index_CBaseCombatCharacter_UpdateGlowEffect 222 // client.dll
#define Index_CBaseAnimating_SetSequence 179 //mayb
#endif

extern void CBaseAnimating_SetSequence(C_BaseAnimating *pThisPtr, int nSequence);

#if !defined( _POSIX )
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <sys/mman.h>
//typedef unsigned long DWORD;
#endif

extern DWORD *HookVFunc(DWORD *vtable, int index, DWORD *newFunction);

#endif
