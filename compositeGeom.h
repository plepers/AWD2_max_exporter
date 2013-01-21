#pragma once

#include "3dsmaxsdk_preinclude.h"
#include "Max.h"
#include <awd/libawd.h>

#define NUMIDS 100
#define MAP_CHAN -2

class CompositeGeom
{
public:
	CompositeGeom(void){
		for( int i = 0; i<NUMIDS ;i++ )
			lenMap[i] = 0;
	}

	void pushFace( unsigned int i1, unsigned int i2, unsigned int i3, int group );
	
	void allocate( Mesh * mesh, MtlID currentmtl );
	
	void reset();

	void debug(FILE* logfile) {
		fprintf( logfile, "		bufferLen : %i \n", bufferLen );
		
		fprintf( logfile, "		cvBuffer[0] : %i \n", cvBuffer[0] );
		
		fprintf( logfile, "		cvBuffer[1] : %i \n", cvBuffer[1] );
		fprintf( logfile, "		cvBuffer[2] : %i \n", cvBuffer[2] );
		fprintf( logfile, "		cvBuffer[3] : %i \n", cvBuffer[3] );
		fprintf( logfile, "		cvBuffer[4] : %i \n", cvBuffer[4] );
		fprintf( logfile, "		cvBuffer[5] : %i \n", cvBuffer[5] );
		fprintf( logfile, "		cvBuffer[6] : %i \n", cvBuffer[6] );
		fflush( logfile );
	}

	int getFaceGroup( Mesh * mesh, int index );

	int getNumGroups( );

	~CompositeGeom(void){
		if( cvBuffer )
			free( cvBuffer );
	}


	awd_uint32* cvBuffer;
	unsigned int bufferLen;

private :
	
	unsigned int lenMap[ NUMIDS ];
	unsigned int headMap[ NUMIDS ];

};

