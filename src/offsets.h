/*
 *  offsets.h
 *  WebSpec project
 *  Modified for AdvSpec
 *  
 *  Copyright (c) 2013 Matthew McNamara
 *  BSD 2-Clause License
 *  http://opensource.org/licenses/BSD-2-Clause
 *
 */

#ifndef OFFSETS_H
#define OFFSETS_H

#include "cdll_client_int.h"
#include "client_class.h"

extern IBaseClientDLL *pClient;

class WSOffsets {
public:
	static int pCBaseCombatCharacter__m_nSequence;

	static void PrepareOffsets();
	static int FindOffsetOfClassProp(const char *className, const char *propName);
	static int FindOffsetOfArrayEnt(const char *classname, const char *arrayName, int element);

	static void DumpClasses();
	static void DumpSendTable(RecvTable *pTable, int Offset = 0, int SpaceCount = 0, bool FirstArrayPropOnly = false);

private:
	static bool CrawlForPropOffset(RecvTable *sTable, const char *propName, int &offset);
	static bool CrawlForArrayEnt(RecvTable *sTable, const char *propName, int element, int &offset);
};

#define MakePtr( cast, ptr, addValue ) (cast)( (unsigned long)(ptr) + (unsigned long)(addValue))

#endif
