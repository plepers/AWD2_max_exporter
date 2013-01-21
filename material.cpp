



/****************************************************************************

  Material and Texture Export
  
****************************************************************************/

void AWDExporter::PrepareMaterials(AWD *awd, awd_ncache *ncache)
{
	if (!GetIncludeMtl()) {
		return;
	}



	int numMtls = mtlList.Count();
	//fprintf(pStream, "\t%s %d\n", ID_MATERIAL_COUNT, numMtls);

	for (int i=0; i<numMtls; i++) {
		ExportMaterial( awd, ncache, mtlList.GetMtl(i), i, -1);
	}

	//fprintf(pStream, "}\n");
}

void AWDExporter::ExportMaterial( AWD *awd, awd_ncache *ncache, Mtl* mtl, int mtlID, int subNo) {

	TimeValue t = GetStaticFrame();
	int i;

	BOOL usediffusemap = FALSE;

	const char *name = mtl->GetName();
	int namelen = strlen( name );


	

	AWDMaterial *material = new AWDMaterial( AWD_MATTYPE_COLOR, name, namelen );

	AWDTexture *tex;

	for (i=0; i<mtl->NumSubTexmaps(); i++) {
		Texmap* subTex = mtl->GetSubTexmap(i);
		float amt = 1.0f;
		if (subTex) {
			// If it is a standard material we can see if the map is enabled.
			if (mtl->ClassID() == Class_ID(DMTL_CLASS_ID, 0)) {
				if (!((StdMat*)mtl)->MapEnabled(i))
					continue;
				amt = ((StdMat*)mtl)->GetTexmapAmt(i, 0);
				
			}
			tex = ExportTexture(awd, ncache, subTex, mtl->ClassID(), i, material );

			if( tex && i == ID_DI ) {
				usediffusemap = true;
			}
				
		}
	}

	if (mtl->ClassID() == Class_ID(DMTL_CLASS_ID, 0)) {
		StdMat* std = (StdMat*)mtl;
		material->color = get_awd_color( std->GetDiffuse(t), 1.0f );

	} else {
		material->color = get_awd_color( mtl->GetDiffuse(), 1.0f );

	}

	

	AWD_mat_type mattype;
	if( usediffusemap ) mattype = AWD_MATTYPE_BITMAP;
	else				mattype = AWD_MATTYPE_COLOR;


	material->set_type( mattype );
	
	awd_ncache_add(ncache, (InterfaceServer*)mtl, material);

	awd->add_material( material );



	if (mtl->NumSubMtls() > 0)  {
		
		
		for (i=0; i<mtl->NumSubMtls(); i++) {
			Mtl* subMtl = mtl->GetSubMtl(i);
			if (subMtl && mtlList.GetMtlID( subMtl ) == -1 ) {
				ExportMaterial(awd, ncache, subMtl, 0, i);
			}
		}
	}

}

AWDTexture *
	AWDExporter::ExportTexture(AWD *awd, awd_ncache *ncache,Texmap* tex, Class_ID cid, int subNo, AWDMaterial * mat ) {

	AWDTexture *awd_tex;

	const char* name;
	int name_len;
	bool hasAlpha = false;

	MSTR path;

	awd_uint8 * buf;
	int buf_len;

	

	if (!tex) return NULL;
	if (tex->ClassID() != Class_ID(BMTEX_CLASS_ID, 0x00) ) return NULL;

	
	// texture already exist in cache
	awd_tex = (AWDTexture *)awd_ncache_get( ncache, tex );
	if( awd_tex ) return awd_tex;

	BitmapTex *bmptex = (BitmapTex*)tex;

	MaxSDK::AssetManagement::AssetUser asset = bmptex->GetMap();

	hasAlpha = bmptex->GetBitmap( GetStaticFrame() )->HasAlpha();

	if( !asset.GetFullFilePath(path) ) {
		fprintf( logfile, " export !asset.GetFullFilePath(path) : %i \n", asset.GetType() );
		fflush( logfile );

		//return NULL;
	}

	fprintf( logfile, " export  : %s \n", path );
		fflush( logfile );

	AWD_tex_type textype = EXTERNAL;
	
	if( GetIncludeMaps() &&
		asset.GetType() == MaxSDK::AssetManagement::kBitmapAsset 
	) {
		
		
		const char * dot;
		dot = strrchr(path,'.');
		



		if( !strcmp(dot, ".jpg")||
			!strcmp(dot, ".JPG")|| 
			!strcmp(dot, ".jpeg")||
			!strcmp(dot, ".JPEG")
			){
				textype = EMBEDDED_JPEG;
		} else if (
			!strcmp(dot, ".png")||
			!strcmp(dot, ".PNG")
			) {
			textype = EMBEDDED_PNG;
		}



		if( textype == 0 ) {
				
			fprintf( logfile, " export texture : %s \n", path );
			fflush( logfile );
			// try to extract data
			Bitmap *bmp = bmptex->GetBitmap( GetStaticFrame() );

			BitmapInfo bi;

			MaxSDK::Util::Path *temppath;

			bi.SetWidth( bmp->Width() );
			bi.SetHeight( bmp->Height() );
			if( hasAlpha ) {
				bi.SetType( BMM_TRUE_32 );
				bi.SetFlags( MAP_HAS_ALPHA );
				path = "C:\\Users\\lepersp\\Desktop\\temp\\awdexporttempjpg.png";
				textype = EMBEDDED_PNG;
			} else {
				bi.SetType( BMM_TRUE_24 );
				path = "C:\\Users\\lepersp\\Desktop\\temp\\awdexporttempjpg.jpg";
				textype = EMBEDDED_JPEG;
			}

			temppath = new MaxSDK::Util::Path( path );
			bi.SetPath( *temppath );

			bmp->OpenOutput( & bi );	
			bmp->Write( & bi );
			bmp->Close(& bi);
		
		}

		if( path != NULL ) {
			
			size_t result;

			int fd;
			errno_t err = _sopen_s( &fd, path, _O_RDONLY|_O_BINARY, _SH_DENYNO, _S_IREAD );
	
			if( err == 0 ){ 
	
				struct stat *s;
				s = (struct stat *)malloc(sizeof(struct stat));
				fstat(fd, s);

				buf_len = s->st_size;
				buf = (awd_uint8 *) malloc (buf_len * sizeof( awd_uint8 ) );

				lseek(fd, 0, SEEK_SET);
				result = read(fd, buf, buf_len);


				if (result != buf_len) {
					textype = EXTERNAL;
				}

				_close( fd );
			} else {
				textype = EXTERNAL;
			}
		}
	}

	name = tex->GetName();
	name_len = strlen( name );
	char* namecpy = (char*) malloc( name_len*sizeof( char ) ) ;
	strcpy( namecpy, name );

	awd_tex = new AWDTexture( textype, namecpy, name_len );

	if( textype != 0 ) {
		awd_tex->set_embed_data(buf, buf_len);
	}

	

	char * pathcpy = (char *) malloc( (path.length()+1) * sizeof( char ) ); 
	strcpy( pathcpy, path.data() );


	awd_tex->set_url( pathcpy, strlen( pathcpy ) );

	awd->add_texture( awd_tex );

	awd_ncache_add( ncache, tex, awd_tex );

	if( subNo == ID_DI ) {
		mat->set_texture( awd_tex );
		mat->alpha_blending = hasAlpha;
	}

	return awd_tex;
}
