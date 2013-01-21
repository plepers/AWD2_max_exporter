#include "awdutil.h"

#include <awd/libawd.h>

/*
awd_float64 * mesh_to_vstream_float64( Mesh* mesh, awd_float64* buf)
{
	Point3 *	verts;
	int nverts;
	unsigned int i;
	unsigned int c;


	verts = mesh->verts;
	nverts = mesh->numVerts;

	
	if (buf == NULL)
		buf = (awd_float64*)malloc(nverts * 3 * sizeof(awd_float64));


	c=0;

	for (i=0; i<nverts; i++) {
		buf[c] = (awd_float64) verts[i].x;
		buf[c+1]= (awd_float64) verts[i].z;
		buf[c+2] = (awd_float64) verts[i].y;
		c = c+3;
	}

	return buf;
  
}

awd_uint32 * mesh_to_vindex( Mesh* mesh, awd_uint32* buf, BOOL negScale)
{
	int i;
	int nfaces;
	unsigned int c;
	int vx1, vx2, vx3;
	

	// Order of the vertices. Get 'em counter clockwise if the objects is
	// negatively scaled.
	if (negScale) {
		vx1 = 0;
		vx2 = 1;
		vx3 = 2;
	}
	else {
		vx1 = 2;
		vx2 = 1;
		vx3 = 0;
	}

	nfaces = mesh->getNumFaces();

	if (buf == NULL)
		buf = (awd_uint32*)malloc(nfaces * 3 * sizeof(awd_uint32));


	c=0;
	for (i=0; i<nfaces; i++) {
		buf[c] = (awd_uint32) mesh->faces[i].v[vx1];
		buf[c+1]= (awd_uint32) mesh->faces[i].v[vx2];
		buf[c+2] = (awd_uint32) mesh->faces[i].v[vx3];
		c = c+3;
	}

	return buf;
  
}
*/

/*
switch y<->z matrix data to fit away3d's coordinate system
*/
void flipMatrix3( Matrix3 *m ) {
	Point3 r1 = m->GetRow(1);
	Point3 r2 = m->GetRow(2);
	
	m->SetRow(1, r2 );
	m->SetRow(2, r1 );

	Point4 c1 = m->GetColumn( 1 );
	Point4 c2 = m->GetColumn( 2 );
	
	m->SetColumn( 1, c2 );
	m->SetColumn( 2, c1 );
}


awd_float64 * get_node_transform_float64( INode* node, int t, awd_float64* buf)
{
	int i;
	Point3 row;
	Matrix3 pivot = node->GetNodeTM(t);
	BOOL isroot = node->IsRootNode();
	
	
	Matrix3 ppivot;


	// >GetNodeTM is world space transform, if parent, remove parent transform to retreive parent space transform
	if( !isroot ) {
		ppivot = node->GetParentTM(t);
		ppivot.Invert();
		pivot *= ppivot;
	}
	
	Matrix3 *matrix = &pivot;

	if (buf == NULL)
		buf = (awd_float64*)malloc(16 * sizeof(awd_float64));


	row = matrix->GetRow(0);
	buf[0] =  (awd_float64) row.x;
	buf[1] =  (awd_float64) row.z;
	buf[2] =  (awd_float64) row.y;
	buf[3] =  (awd_float64) 0.0;

	row = matrix->GetRow(2);
	buf[4] =  (awd_float64) row.x;
	buf[5] =  (awd_float64) row.z;
	buf[6] =  (awd_float64) row.y;
	buf[7] =  (awd_float64) 0.0;

	row = matrix->GetRow(1);
	buf[8] =  (awd_float64) row.x;
	buf[9] =  (awd_float64) row.z;
	buf[10] =  (awd_float64) row.y;
	buf[11] =  (awd_float64) 0.0;

	row = matrix->GetRow(3);
	buf[12] =  (awd_float64) row.x;
	buf[13] =  (awd_float64) row.z;
	buf[14] =  (awd_float64) row.y;
	buf[15] =  (awd_float64) 1.0;

	return buf;
  
}


awd_float64 * gmatrix_to_float64( GMatrix mat, awd_float64* buf)
{
	if (buf == NULL)
		buf = (awd_float64*)malloc(16 * sizeof(awd_float64));

	Point4 row;

	row = mat.GetRow(0);
	buf[0] =  (awd_float64) row.x;
	buf[1] =  (awd_float64) row.z;
	buf[2] =  (awd_float64) row.y;
	buf[3] =  (awd_float64) row.w;

	row = mat.GetRow(2);
	buf[4] =  (awd_float64) row.x;
	buf[5] =  (awd_float64) row.z;
	buf[6] =  (awd_float64) row.y;
	buf[7] =  (awd_float64) row.w;

	row = mat.GetRow(1);
	buf[8] =  (awd_float64) row.x;
	buf[9] =  (awd_float64) row.z;
	buf[10] =  (awd_float64) row.y;
	buf[11] =  (awd_float64) row.w;

	row = mat.GetRow(3);
	buf[12] =  (awd_float64) row.x;
	buf[13] =  (awd_float64) row.z;
	buf[14] =  (awd_float64) row.y;
	buf[15] =  (awd_float64) row.w;

	return buf;
}

awd_color		get_awd_color( Color input, float alpha ) {

	int r = (int) (input.r * 0xFF);
	int g = (int) (input.g * 0xFF); 
	int b = (int) (input.b * 0xFF);
	int a = (int) (alpha * 0xFF); 

	unsigned int res = (a << 24) +  (r << 16) + (g << 8) + b;

    return res;
}

/**
  * return sorted list of defined MatID for given mesh
  */
matid_list			get_matid_list( Mesh * mesh ) {

	MtlID temp[512] = {0};
	MtlID current;
	int i;
	int c;
	int len = 0;
	BOOL haszero = false;
	BOOL exist;



	
	for (i=0; i<mesh->getNumFaces(); i++) {
		current = mesh->faces[i].getMatID();
		if( current == 0 ) {
			haszero = TRUE;
		}
		else {
			c = 0;
			exist = FALSE;
			while( temp[c] ) {
				if( temp[c] == current ) {
					exist = TRUE;
					break;
				}
				c++;
			}
			if( !exist ) {
				temp[c] = current;
				len = c+1;
			}
		}
	}

	if( haszero ) 
		len++;
	
	// sort
	matid_list matlist;
	matlist.list = (MtlID*) malloc( len*sizeof( MtlID ) );


	
	int t;
	unsigned int min;
	unsigned int max = 0;
	i=0;

	if( haszero ){
		matlist.list[0] = 0;
		i=1;
	}

	for (; i<len; i++) {

		min = 0xFFFFFFFF;

		for (t=0; t<len; t++) {
			if( temp[t] > max && temp[t] < min) {
				min = temp[t];
			}
		}

		max = min;
		matlist.list[i] = min;
	}

	matlist.len = len;

	return matlist;
}

int get_skin_size( IGameSkin * s ) 
{
	if( s == NULL ) 
		return 0;

	int size = 1;
	int numbones;
	for(int x=0; x<s->GetNumOfSkinnedVerts();x++)
	{
		int type = s->GetVertexType(x);
		if(type != IGameSkin::IGAME_RIGID)
		{
			numbones = s->GetNumberOfBones(x);
			if( size < numbones ) size = numbones;
		}
		
	}

	return size;

}