#include "vertextable.h"



// Initialize the vertex table
VertexTable::VertexTable( )
{
	
	_first = NULL;
	_last = NULL;

	tab.SetCount(TABLE_SIZE);
    for(int i = 0; i < TABLE_SIZE; i++)
        tab[i] = NULL;
}

VertexTable::~VertexTable()
{
	reset();
	/*delete _first;

	_first = NULL;
	_last = NULL;

	for(int i = 0; i < TABLE_SIZE; i++) {
		tab[i] = NULL;
	}*/

	//tab.~Tab();
}

void VertexTable::reset() {
	
	for(int i = 0; i < TABLE_SIZE; i++) {
		tab[i] = NULL;
	}

	VertexDesc* nd = _first;
	while (nd != NULL)
	{
		VertexDesc* const next_node = nd->next;
		delete nd;
		nd = next_node;
	}
	
	_first = NULL;
	_last = NULL;
	
}

unsigned int
VertexTable::GetNumVertex() {
	if( _last == NULL ) return 0;
	return _last->index + 1;
}

// Add a normal to the hash table
unsigned int
VertexTable::AddVertex(Point3 coords, Point3 norm, Point3 uv, Point3 color, SkinData * skindata)
{ 
	VertexDesc* newvert = new VertexDesc( coords, norm, uv, color, skindata );

	DWORD hash = HashCode( newvert );

	if( _first == NULL ) {
		_first = newvert;
		newvert->index = 0;
		
	} else {

		// Truncate normals to a value that brings close normals into
		// the same bucket.
		VertexDesc* nd;
		for(nd = tab[hash]; nd; nd = nd->bnext) {
			if (nd->isEqual( newvert ) )  {// Equality OK because of normalization procedure.
				delete newvert;
				return nd->index;
			}
		}

		newvert->bnext = tab[hash];
		newvert->index = _last->index + 1;
		_last->next = newvert;
	}
	tab[hash] = newvert;
    
	_last = newvert;

	return newvert->index;
}

// Print the hash table statistics for the vertex table
void
VertexTable::PrintStats(FILE* mStream)
{
    int slots = 0;
    int buckets = 0;
    int i;
    VertexDesc* nd;

	int bsize;
	int max = 0;

	int cstacklen = 0;
	for(nd = _first; nd; nd = nd->next) 
		cstacklen++;

    for(i = 0; i < TABLE_SIZE; i++) {
        if (tab[i]) {
            slots++;
			bsize = 0;
            for(nd = tab[i]; nd; nd = nd->bnext) {
                buckets++;
				bsize++;
			}

			if( bsize > max ) max = bsize;
        }
    }
    fprintf(mStream,_T("# slots = %d, buckets = %d, maxsize = %d, avg. chain length = %.5g, complete size %d\n"),
            slots, buckets, max, ((double) buckets / (double) slots), cstacklen );
            
}

// Produce a hash code for a vertex
DWORD
VertexTable::HashCode(VertexDesc *desc)
{
    union {
        float p[8];
        DWORD i[8];
    } u;

    u.p[0] = desc->pos->x;	u.p[1] = desc->pos->y;	u.p[2] = desc->pos->z;
	u.p[3] = desc->n->x;	u.p[4] = desc->n->y;	u.p[5] = desc->n->z;
	u.p[6] = desc->uv->x;	u.p[7] = desc->uv->y;

//	u.p[0] = norm( desc->pos.x );	u.p[1] = norm( desc->pos.y );	u.p[2] = norm( desc->pos.z );
//	u.p[3] = norm( desc->n.x );		u.p[4] = norm( desc->n.y );		u.p[5] = norm( desc->n.z );
//	u.p[6] = norm( desc->uv.x );	u.p[7] = norm( desc->uv.y );

    return ((u.i[0] >> 3) + (u.i[1] >> 3)  + (u.i[2] >> 3)+ (u.i[3] >> 3)+ (u.i[4] >> 3)+ (u.i[5] >> 3)+ (u.i[6] >> 3)+ (u.i[7] >> 3)) % TABLE_SIZE;
}

awd_float64* VertexTable::getVertexStream( awd_float64* buf) {
	if (buf == NULL)
		buf = (awd_float64*) malloc( GetNumVertex() * 3 * sizeof(awd_float64) );
	int c = 0;
	VertexDesc* nd;
	Point3		tpos;
	for(nd = _first; nd; nd = nd->next) {
		tpos = pivot->PointTransform( *nd->pos );
		buf[c] = tpos.x;
		buf[c+1] = tpos.z;
		buf[c+2] = tpos.y;
		c = c+3;
	}
	return buf;
}

awd_float64* VertexTable::getNormalStream( awd_float64* buf) {

	if (buf == NULL)
		buf = (awd_float64*) malloc( GetNumVertex() * 3 * sizeof(awd_float64) );
	int c = 0;
	VertexDesc* nd;
	Point3		tn;
	for(nd = _first; nd; nd = nd->next) {
		tn = pivot->VectorTransform( *nd->n );
		buf[c] = tn.x;
		buf[c+1] = tn.z;
		buf[c+2] = tn.y;
		c = c+3;
	}
	return buf;


}

awd_float64* VertexTable::getColorsStream( awd_float64* buf) {
	
	if (buf == NULL)
		buf = (awd_float64*) malloc( GetNumVertex() * 3 * sizeof(awd_float64) );
	int c = 0;
	VertexDesc* nd;
	for(nd = _first; nd; nd = nd->next) {
		buf[c] = nd->color->x;
		buf[c+1] = nd->color->y;
		buf[c+2] = nd->color->z;
		c = c+3;
	}
	return buf;
}

awd_float64* VertexTable::getUvsStream( awd_float64* buf) {
	if (buf == NULL)
		buf = (awd_float64*) malloc( GetNumVertex() * 2 * sizeof(awd_float64) );
	int c = 0;
	VertexDesc* nd;
	for(nd = _first; nd; nd = nd->next) {
		buf[c] = nd->uv->x;
		buf[c+1] = nd->uv->y;
		c = c+2;
	}
	return buf;
}


awd_float64* VertexTable::getSweightStream( awd_float64* buf ){
	if (buf == NULL)
		buf = (awd_float64*) malloc( GetNumVertex() * skinSize * sizeof(awd_float64) );

	int c = 0;
	VertexDesc* nd;
	for(nd = _first; nd; nd = nd->next) {
		for( int bi = 0; bi < skinSize; bi++ ) {
			buf[c] = nd->jweights[bi];
			c++;
		}
	}
	return buf;
}

awd_uint32* VertexTable::getSindicesStream( awd_uint32* buf ){
	if (buf == NULL)
		buf = (awd_uint32*) malloc( GetNumVertex() * skinSize * sizeof(awd_uint32) );

	int c = 0;
	VertexDesc* nd;
	for(nd = _first; nd; nd = nd->next) {
		for( int bi = 0; bi < skinSize; bi++ ) {
			buf[c] = nd->jindices[bi];
			c++;
		}
	}
	return buf;
}




BOOL
VertexDesc::isEqual( VertexDesc *other ) 
{
	if( !pos->Equals( *other->pos, DIFF_EPSILON ) )		return FALSE;
	if( !n->Equals( *other->n, DIFF_EPSILON ) )			return FALSE;
	if( !uv->Equals( *other->uv, DIFF_EPSILON ) )		return FALSE;
	return TRUE;
}