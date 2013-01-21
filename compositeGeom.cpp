#include "compositeGeom.h"



void 
	CompositeGeom::pushFace( unsigned int i1, unsigned int i2, unsigned int i3, int group ) {
		int head = headMap[group];
		cvBuffer[head] = i1;
		cvBuffer[head+1] = i2;
		cvBuffer[head+2] = i3;
		headMap[group]+=3;
}

void
	CompositeGeom::reset() {

		if( cvBuffer ) free( cvBuffer );

		for ( int i=0; i<NUMIDS; i++) {
			headMap[i] = 0;
			lenMap[i] = 0;
		}
}

int 
	CompositeGeom::getNumGroups( ){
		unsigned int num = 0;
		for (unsigned int i=0; i<NUMIDS; i++) {
			if( lenMap[i] > 0 )
				num ++;
		}
		return num;
}

int 
	CompositeGeom::getFaceGroup( Mesh * mesh, int index ){

		TVFace* tvFaces = mesh->mapFaces(MAP_CHAN);

		int vertindex = tvFaces[ index ].t[ 0 ];
		int a0 = int( mesh->mapVerts(MAP_CHAN)[vertindex].z * 100.0f + 0.001f );
		vertindex = tvFaces[ index ].t[ 1 ];
		int a1 = int( mesh->mapVerts(MAP_CHAN)[vertindex].z * 100.0f + 0.001f );
		vertindex = tvFaces[ index ].t[ 2 ];
		int a2 = int( mesh->mapVerts(MAP_CHAN)[vertindex].z * 100.0f + 0.001f );

		if( a0 != a1 || a0 != a2 ) 
			a0 = 0;

		if( a0 > NUMIDS-1 ) a0 = NUMIDS-1;
		if( a0 <0 ) a0 = 0;

		return a0;
}

void
	CompositeGeom::allocate( Mesh * mesh, MtlID currentmtl ) {

		unsigned int i;
		unsigned int vertindex;
		unsigned int group;

		int numComposite = 0;
		int numfaces = 0;




		for (i=0; i<mesh->getNumFaces(); i++) {

			if( mesh->faces[i].getMatID() == currentmtl ) {
				
				numfaces ++;

				group = getFaceGroup( mesh, i );

				if( lenMap[ group ] == 0 ) numComposite++;

				lenMap[ group ]++;
			}

		}

		bufferLen = numfaces * 3 + numComposite*2 ;

		cvBuffer = (awd_uint32*) malloc( bufferLen * sizeof(awd_uint32) );

		unsigned int head = 0;

		for (i=0; i<NUMIDS; i++) {
			
			if( lenMap[i] > 0 ){
				cvBuffer[head] = i;
				cvBuffer[head+1] = lenMap[i]*3;
				headMap[i] = head+2;
				head += lenMap[i]*3+2;
				
			}
		}

}
