/**********************************************************************
 *<
	FILE: normtab.h

	DESCRIPTION:  Normal Hash Table class defs

	CREATED BY: Scott Morrison

 *>	Copyright (c) 1996, All Rights Reserved.
 **********************************************************************/
#include "3dsmaxsdk_preinclude.h"
#include "Max.h"
#include <awd/libawd.h>


#define NUM_NORMS 10000.0f
#define DIFF_EPSILON 1.0e-10
#define TABLE_SIZE 1024

#define norm(w) ((float) ((int) (NUM_NORMS * (w))))

//inline float norm( float val ) {
//	return (float) ((int) (NUM_NORMS * (w)))
//}

// Truncate normals into the specified range.
inline 
Point3 NormalizeNorm( Point3 norm )
{
    Point3 p;
    p.x = norm(norm.x);
    p.y = norm(norm.y);
    p.z = norm(norm.z);
    return p;
}

typedef struct {
	awd_float64 * jweights;
	awd_uint32 * jindices;
} SkinData;

// A normal table hash bucket.
class VertexDesc {
public:
    VertexDesc(Point3 coords, Point3 norm, Point3 uvs, Point3 colors, SkinData* sdata ) {
        
		n =		new Point3(norm.x, norm.y, norm.z);
		pos =	new Point3(coords.x, coords.y, coords.z);
		color =	new Point3(colors.x, colors.y, colors.z);
		uv =	new Point3(uvs.x, 1.0f - uvs.y, 0.0f);

		
		if( sdata != NULL ) {
			jweights = sdata->jweights;
			jindices = sdata->jindices;
		} else {
			jweights = NULL;
			jindices = NULL;
		}

        index = 0;
        next = NULL;
		bnext = NULL;
    }

	BOOL isEqual( VertexDesc *other );

	
    ~VertexDesc() {
		delete n;
		delete pos;
		delete uv;

		
		free( jindices );
		free( jweights );


		//delete next;
		bnext = NULL;
		next = NULL;
	}

    Point3 *n;           // The normalize normal
	Point3 *pos;         // xyz pos
	Point3 *uv;			// UV coords
	Point3 *color;			// UV coords

	awd_float64 * jweights;
	awd_uint32 * jindices;

    unsigned int    index;       // The index in the index face set
    VertexDesc* next;   // Next complete list.
	VertexDesc* bnext;   // Next hash bucket.

};

// Un-comment this line to get data on the normal hash table
// #define DEBUG_NORM_HASH

// Hash table for rendering normals
class VertexTable 
{
public:
    VertexTable( );
    ~VertexTable();

	unsigned int AddVertex(Point3 coords, Point3 norm, Point3 uv,Point3 color, SkinData * skindata);

	unsigned int GetNumVertex();

	DWORD		HashCode(VertexDesc *desc);

	Matrix3*	pivot;

	int skinSize;

	void reset();

	void		PrintStats(FILE* mStream);

   // int GetIndex(Point3& norm);
   // VertexDesc* Get(int i) { return tab[i]; };

	awd_float64* getVertexStream( awd_float64* buf);
	awd_float64* getNormalStream( awd_float64* buf);
	awd_float64* getColorsStream( awd_float64* buf);
	awd_float64* getUvsStream( awd_float64* buf);
	awd_float64* getSweightStream( awd_float64* buf);
	awd_uint32* getSindicesStream( awd_uint32* buf);
	
    
private:
    Tab<VertexDesc*> tab;
	VertexDesc *_first;
	VertexDesc *_last;
};

