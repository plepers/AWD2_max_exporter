#pragma once

//**************************************************************************/
// Copyright (c) 1998-2007 Autodesk, Inc.
// All rights reserved.
// 
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information written by Autodesk, Inc., and are
// protected by Federal copyright law. They may not be disclosed to third
// parties or copied or duplicated in any form, in whole or in part, without
// the prior written consent of Autodesk, Inc.
//**************************************************************************/
// DESCRIPTION: Includes for Plugins
// AUTHOR: 
//***************************************************************************/

#include "3dsmaxsdk_preinclude.h"
#include "Max.h"
#include "stdmat.h"
#include "ncache.h"
#include "jcache.h"
#include "maxskeleton.h"
#include <awd/libawd.h>
#include "resource.h"
#include "istdplug.h"
#include "iparamb2.h"
#include "iparamm2.h"
#include "bitmap.h"


#include "IGame/IGame.h"
#include "IGame/IGameModifier.h"
#include "IGame/IGameObject.h"
//SIMPLE TYPE

typedef struct {

    awd_float64 *x;
    awd_float64 *y;
    awd_float64 *z;

    awd_float64 *nx;
    awd_float64 *ny;
    awd_float64 *nz;
	
    awd_float64 *u;
    awd_float64 *v;

} _awd_vertex;

typedef struct {

    unsigned int len;
	MtlID	*	list;

} matid_list;


class MtlKeeper {
public:
	BOOL	AddMtl(Mtl* mtl);
	int		GetMtlID(Mtl* mtl);
	int		Count();
	Mtl*	GetMtl(int id);

	Tab<Mtl*> mtlTab;
};

extern TCHAR *GetString(int id);

extern HINSTANCE hInstance;
