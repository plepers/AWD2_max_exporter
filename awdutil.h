#ifndef _AWDEXPORTER_UTIL_H
#define _AWDEXPORTER_UTIL_H

#include <awd/libawd.h>
#include "Max.h"
#include "AWDExporter.h"

//awd_float64 *	mesh_to_vstream_float64( Mesh *mesh, awd_float64 *buf );
//awd_uint32 *	mesh_to_vindex( Mesh* mesh, awd_uint32* buf, BOOL negScale);
awd_float64 *	get_node_transform_float64( INode* node, int t, awd_float64* buf);

awd_color		get_awd_color( Color input, float alpha );

matid_list		get_matid_list( Mesh * mesh );

awd_float64 *	gmatrix_to_float64( GMatrix mat, awd_float64* buf);

int				get_skin_size( IGameSkin* s );

void			flipMatrix3( Matrix3 * m );

#endif