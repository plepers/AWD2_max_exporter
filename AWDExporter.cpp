
//**************************************************************************/
// DESCRIPTION: AWD exporter
// AUTHOR: 
//***************************************************************************/

#include "AWDExporter.h"
#include "ncache.h"
#include "awdutil.h"
#include "vertextable.h"
#include "compositeGeom.h"
#include <cstdio>
#include <io.h>
#include <awd/libawd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <share.h>
#include <WindowsX.h>
#include <MeshNormalSpec.h>
#include <vector>
#include <algorithm>

using namespace std;



#define AWDExporter_CLASS_ID	Class_ID(0x340dcedf, 0x2fa26b39)
#define NAMESPACE_URI			_T("http://code.google.com/r/pierrelepers-max/");
#define MAX_VERTEX_PER_BUFF		0x2FFFD

#define CFGFILENAME				_T("AWDEXPORTER.CFG")	// Configuration file
#define VERSION					70
#define CFG_VERSION				70
#define LOG						TRUE
#define MAX_VSIZE				0xffff


#ifdef WIN32
static BOOL showPrompts;
static BOOL exportSelected;
#endif

class AWDExporter : public SceneExport {
	public:
		
		static HWND hParams;
		
		int				ExtCount();					// Number of extensions supported
		const TCHAR *	Ext(int n);					
		const TCHAR *	LongDesc();					
		const TCHAR *	ShortDesc();				
		const TCHAR *	AuthorName();				
		const TCHAR *	CopyrightMessage();			
		const TCHAR *	OtherMessage1();			
		const TCHAR *	OtherMessage2();			
		unsigned int	Version();					
		void			ShowAbout(HWND hWnd);		

		// Interface to member variables
		inline BOOL	GetIncludeMesh()			{ return bIncludeMesh; }
		inline BOOL	GetIncludeAnim()			{ return bIncludeAnim; }
		inline BOOL	GetIncludeMtl()				{ return bIncludeMtl; }
		inline BOOL	GetIncludeMeshAnim()		{ return bIncludeMeshAnim; }
		inline BOOL	GetIncludeCamLightAnim()	{ return bIncludeCamLightAnim; }
		inline BOOL	GetIncludeIKJoints()		{ return bIncludeIKJoints; }
		inline BOOL	GetIncludeNormals()			{ return bIncludeNormals; }
		inline BOOL	GetIncludeTextureCoords()	{ return bIncludeTextureCoords; }
		inline BOOL	GetIncludeVertexColors()	{ return bIncludeVertexColors; }
		inline BOOL	GetIncludeObjGeom()			{ return bIncludeObjGeom; }
		inline BOOL	GetIncludeObjShape()		{ return bIncludeObjShape; }
		inline BOOL	GetIncludeObjCamera()		{ return bIncludeObjCamera; }
		inline BOOL	GetIncludeObjLight()		{ return bIncludeObjLight; }
		inline BOOL	GetIncludeObjHelper()		{ return bIncludeObjHelper; }
		inline BOOL	GetWideFormat()				{ return wideformat; }
		inline BOOL	GetStreaming()				{ return streaming; }
		inline BOOL	GetIncludeMaps()			{ return bIncludeMaps; }
		inline AWD_compression GetCompression()	{ return compression; }
		inline TimeValue GetStaticFrame()		{ return nStaticFrame; }
		inline Interface*	GetInterface()		{ return ip; }
		inline AWDNamespace* GetNamespace()		{ return _namespace; }


		inline void	SetIncludeMesh(BOOL val)			{ bIncludeMesh = val; }
		inline void	SetIncludeAnim(BOOL val)			{ bIncludeAnim = val; }
		inline void	SetIncludeMtl(BOOL val)				{ bIncludeMtl = val; }
		inline void	SetIncludeMeshAnim(BOOL val)		{ bIncludeMeshAnim = val; }
		inline void	SetIncludeCamLightAnim(BOOL val)	{ bIncludeCamLightAnim = val; }
		inline void	SetIncludeIKJoints(BOOL val)		{ bIncludeIKJoints = val; }
		inline void	SetIncludeNormals(BOOL val)			{ bIncludeNormals = val; }
		inline void	SetIncludeTextureCoords(BOOL val)	{ bIncludeTextureCoords = val; }
		inline void	SetIncludeVertexColors(BOOL val)	{ bIncludeVertexColors = val; }
		inline void	SetIncludeObjGeom(BOOL val)			{ bIncludeObjGeom = val; }
		inline void	SetIncludeObjShape(BOOL val)		{ bIncludeObjShape = val; }
		inline void	SetIncludeObjCamera(BOOL val)		{ bIncludeObjCamera = val; }
		inline void	SetIncludeObjLight(BOOL val)		{ bIncludeObjLight = val; }
		inline void	SetIncludeObjHelper(BOOL val)		{ bIncludeObjHelper = val; }
		inline void	SetIncludeMaps(BOOL val)			{ bIncludeMaps = val; }
		inline void	SetWideFormat(BOOL val)				{ wideformat = val; }
		inline void	SetStreaming(BOOL val)				{ streaming = val; }
		inline void SetStaticFrame(TimeValue val)		{ nStaticFrame = val; }
		inline void SetCompression(AWD_compression val)	{ compression = val; }
		inline void SetNamespace(AWDNamespace* val)		{ _namespace = val; }


		BOOL SupportsOptions(int ext, DWORD options);
		int				DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts=FALSE, DWORD options=0);
		
		void			Export(INode* pNode, int iTreeDepth);
		void			PreProcess(INode* node, int& nodeCount);
		void			PrepareScene(awd_ncache *ncache, AWD *awd );
		void			PrepareBlocks( awd_ncache *ncache, AWD *awd );
		void			PrepareMaterials( AWD *awd, awd_ncache *ncache );
		void			PrepareNamespace( AWD *awd );
		void			PrepareMeta( AWD *awd );
		BOOL			nodeEnum(INode* node, AWD *awd, awd_ncache *ncache );

		
		// Configuration methods
		TSTR	GetCfgFilename();
		BOOL	ReadConfig();
		void	WriteConfig();

		//Constructor/Destructor
		AWDExporter();
		~AWDExporter();		

	private:

		void			_prepareSceneBlock(INode *node , AWD *awd, awd_ncache *ncache);
		AWDMeshData*	_prepareMeshData(INode *node , AWD *awd, awd_ncache *ncache);
		AWDSceneBlock*	_prepareMeshInst(INode *node , AWD *awd, awd_ncache *ncache);
		AWDSceneBlock*	_prepareHelper(INode *node , AWD *awd, awd_ncache *ncache);
		AWDSceneBlock*	_prepareCamera(INode *node , AWD *awd, awd_ncache *ncache);

		
		void			ExportMaterial(AWD *awd, awd_ncache *ncache, Mtl* mtl, int mtlID, int subNo);
		void			ExportSkinData( IGameSkin * skin, int vertexIndex, int size, SkinData * skindata );
		void			ExportSkeleton( IGameSkin * skin, AWD *awd, awd_ncache *ncache, AWDMeshData* awd_mesh );
		void			ExportSkeletonAnim( IGameSkin * skin, AWD *awd, awd_ncache *ncache, AWDMeshData* awd_mesh );
		IGameSkin *		GetMeshSkin( IGameNode * ignode );
		void			ApplyMeshMaterials( AWDMeshInst * awdmesh, INode *node, awd_ncache *ncache );
		AWDTexture *	ExportTexture(AWD *awd, awd_ncache *ncache,Texmap* tex, Class_ID cid, int subNo, AWDMaterial * mat);

		void			_nameElement( INode* node, AWDNamedElement* element );
		Point3			getVertexNormal(Mesh* mesh, int faceNo, RVertex* rv);
		BOOL			TMNegParity(Matrix3 &m);
		TriObject*		GetTriObjectFromNode(INode *node, TimeValue t, int &deleteIt);

		AWDNamespace*	_namespace;

		BOOL	bIncludeMesh;
		BOOL	bIncludeAnim;
		BOOL	bIncludeMtl;
		BOOL	bIncludeMeshAnim;
		BOOL	bIncludeCamLightAnim;
		BOOL	bIncludeIKJoints;
		BOOL	bIncludeNormals;
		BOOL	bIncludeTextureCoords;
		BOOL	bIncludeObjGeom;
		BOOL	bIncludeObjShape;
		BOOL	bIncludeObjCamera;
		BOOL	bIncludeObjLight;
		BOOL	bIncludeObjHelper;
		BOOL	bIncludeMaps;
		BOOL	bIncludeVertexColors;
		BOOL	wideformat;
		BOOL	streaming;

		AWD_compression		compression;
 		TimeValue	nStaticFrame;


		Interface*		ip;
		IGameScene *	pIgame;
		AWD				*docawd;
		int				nTotalNodeCount;
		int				nCurNode;
		awd_ncache*		geomcache;

		MtlKeeper		mtlList;

		FILE *			logfile;
};


class AWDExporterClassDesc : public ClassDesc2 
{
public:
	virtual int IsPublic() 							{ return TRUE; }
	virtual void* Create(BOOL /*loading = FALSE*/) 		{ return new AWDExporter(); }
	virtual const TCHAR *	ClassName() 			{ return GetString(IDS_CLASS_NAME); }
	virtual SClass_ID SuperClassID() 				{ return SCENE_EXPORT_CLASS_ID; }
	virtual Class_ID ClassID() 						{ return AWDExporter_CLASS_ID; }
	virtual const TCHAR* Category() 				{ return GetString(IDS_CATEGORY); }

	virtual const TCHAR* InternalName() 			{ return _T("AWDExporter"); }	// returns fixed parsable name (scripter-visible name)
	virtual HINSTANCE HInstance() 					{ return hInstance; }					// returns owning module handle
	

};


ClassDesc2* GetAWDExporterDesc() { 
	static AWDExporterClassDesc AWDExporterDesc;
	return &AWDExporterDesc; 
}





INT_PTR CALLBACK AWDExporterOptionsDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) {
	static AWDExporter *exp = NULL;

	Interval animRange;
	ISpinnerControl  *spin;
	HWND cb;

	switch(message) {
		case WM_INITDIALOG:
			exp = (AWDExporter *)lParam;
			//DLSetWindowLongPtr(hWnd, lParam); 
			CenterWindow(hWnd, GetParent(hWnd)); 
			CheckDlgButton(hWnd, IDC_MESHDATA, exp->GetIncludeMesh()); 
			CheckDlgButton(hWnd, IDC_ANIMKEYS, exp->GetIncludeAnim()); 
			CheckDlgButton(hWnd, IDC_MATERIAL, exp->GetIncludeMtl());
			CheckDlgButton(hWnd, IDC_MESHANIM, exp->GetIncludeMeshAnim()); 
			CheckDlgButton(hWnd, IDC_CAMLIGHTANIM, exp->GetIncludeCamLightAnim()); 
	
			CheckDlgButton(hWnd, IDC_IKJOINTS, exp->GetIncludeIKJoints()); 
	
			CheckDlgButton(hWnd, IDC_NORMALS,  exp->GetIncludeNormals()); 
			CheckDlgButton(hWnd, IDC_TEXCOORDS,exp->GetIncludeTextureCoords()); 
			CheckDlgButton(hWnd, IDC_VERTEXCOLORS,exp->GetIncludeVertexColors()); 
			CheckDlgButton(hWnd, IDC_OBJ_GEOM,exp->GetIncludeObjGeom()); 
			CheckDlgButton(hWnd, IDC_OBJ_SHAPE,exp->GetIncludeObjShape()); 
			CheckDlgButton(hWnd, IDC_OBJ_CAMERA,exp->GetIncludeObjCamera()); 
			CheckDlgButton(hWnd, IDC_OBJ_LIGHT,exp->GetIncludeObjLight()); 
			CheckDlgButton(hWnd, IDC_OBJ_HELPER,exp->GetIncludeObjHelper());
			CheckDlgButton(hWnd, IDC_TEXTURES,exp->GetIncludeMaps());

			cb = GetDlgItem(hWnd, IDC_COMP_COMBO);
			
			ComboBox_AddString(cb, _T("Uncompressed"));
			ComboBox_AddString(cb, _T("Deflate"));
			ComboBox_AddString(cb, _T("Lzma"));
			//GetAppData(exp->mIp, POLYGON_TYPE_ID, _T(GetString(IDS_OUT_TRIANGLES)), text, MAX_PATH);

			TCHAR * text;
			switch( exp->GetCompression() ) {
			case UNCOMPRESSED:
				text = _T("Uncompressed");
			case DEFLATE:
				text = _T("Deflate");
			case LZMA:
				text = _T("Lzma");
			}
			ComboBox_SetCurSel( cb, exp->GetCompression() );
			

			// Setup the spinner controls for the floating point precision 
			CheckDlgButton(hWnd, IDC_WIDE,  exp->GetWideFormat()); 

			// Setup the spinner control for the static frame#
			// We take the frame 0 as the default value
			animRange = exp->GetInterface()->GetAnimRange();
			spin = GetISpinner(GetDlgItem(hWnd, IDC_STATIC_FRAME_SPIN)); 
			spin->LinkToEdit(GetDlgItem(hWnd,IDC_STATIC_FRAME), EDITTYPE_INT ); 
			spin->SetLimits(animRange.Start() / GetTicksPerFrame(), animRange.End() / GetTicksPerFrame(), TRUE); 
			spin->SetScale(1.0f);
			spin->SetValue(0, FALSE);
			ReleaseISpinner(spin);


			// Enable / disable mesh options
			EnableWindow(GetDlgItem(hWnd, IDC_NORMALS), IsDlgButtonChecked(hWnd,
				IDC_MESHDATA));
			EnableWindow(GetDlgItem(hWnd, IDC_TEXCOORDS), IsDlgButtonChecked(hWnd,
				IDC_MESHDATA));
			EnableWindow(GetDlgItem(hWnd, IDC_VERTEXCOLORS), FALSE );
			// Enable / disable mesh options
			EnableWindow(GetDlgItem(hWnd, IDC_TEXTURES), IsDlgButtonChecked(hWnd,
				IDC_MATERIAL));
			return TRUE;

		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case IDC_MESHDATA:
					// Enable / disable mesh options
					EnableWindow(GetDlgItem(hWnd, IDC_NORMALS), IsDlgButtonChecked(hWnd,
						IDC_MESHDATA));
					EnableWindow(GetDlgItem(hWnd, IDC_TEXCOORDS), IsDlgButtonChecked(hWnd,
						IDC_MESHDATA));
					// Vertex colors not yet supported
					EnableWindow(GetDlgItem(hWnd, IDC_VERTEXCOLORS), FALSE );
					break;
				case IDC_MATERIAL:
					// Enable / disable mesh options
					EnableWindow(GetDlgItem(hWnd, IDC_TEXTURES), IsDlgButtonChecked(hWnd,
						IDC_MATERIAL));
					
					break;
				case IDOK:
					exp->SetIncludeMesh(IsDlgButtonChecked(hWnd, IDC_MESHDATA)); 
					exp->SetIncludeAnim(IsDlgButtonChecked(hWnd, IDC_ANIMKEYS)); 
					exp->SetIncludeMtl(IsDlgButtonChecked(hWnd, IDC_MATERIAL)); 
					exp->SetIncludeMeshAnim(IsDlgButtonChecked(hWnd, IDC_MESHANIM)); 
					exp->SetIncludeCamLightAnim(IsDlgButtonChecked(hWnd, IDC_CAMLIGHTANIM)); 
		#ifndef DESIGN_VER
					exp->SetIncludeIKJoints(IsDlgButtonChecked(hWnd, IDC_IKJOINTS)); 
		#endif // !DESIGN_VER
					exp->SetIncludeNormals(IsDlgButtonChecked(hWnd, IDC_NORMALS));
					exp->SetIncludeTextureCoords(IsDlgButtonChecked(hWnd, IDC_TEXCOORDS)); 
					exp->SetIncludeVertexColors(IsDlgButtonChecked(hWnd, IDC_VERTEXCOLORS)); 
					exp->SetIncludeObjGeom(IsDlgButtonChecked(hWnd, IDC_OBJ_GEOM)); 
					exp->SetIncludeObjShape(IsDlgButtonChecked(hWnd, IDC_OBJ_SHAPE)); 
					exp->SetIncludeObjCamera(IsDlgButtonChecked(hWnd, IDC_OBJ_CAMERA)); 
					exp->SetIncludeObjLight(IsDlgButtonChecked(hWnd, IDC_OBJ_LIGHT)); 
					exp->SetIncludeObjHelper(IsDlgButtonChecked(hWnd, IDC_OBJ_HELPER));
					exp->SetIncludeMaps(IsDlgButtonChecked(hWnd, IDC_TEXTURES));
					

					exp->SetWideFormat(IsDlgButtonChecked(hWnd, IDC_WIDE));

					// Compression COMBO
					exp->SetCompression( (AWD_compression) ComboBox_GetCurSel( GetDlgItem(hWnd, IDC_COMP_COMBO) ) );
					

		
					spin = GetISpinner(GetDlgItem(hWnd, IDC_STATIC_FRAME_SPIN)); 
					exp->SetStaticFrame(spin->GetIVal() * GetTicksPerFrame());
					ReleaseISpinner(spin);
			
					EndDialog(hWnd, 1);
					break;
				case IDCANCEL:
					EndDialog(hWnd, 0);
					break;
			}
		
	}
	return 0;
}


//--- AWDExporter -------------------------------------------------------
AWDExporter::AWDExporter()
{
	bIncludeMesh =	TRUE;
	bIncludeAnim =	FALSE;
	bIncludeMtl =	FALSE;
	bIncludeMaps=	FALSE;
	bIncludeMeshAnim =  FALSE;
	bIncludeCamLightAnim = FALSE;
	bIncludeIKJoints = TRUE;
	bIncludeNormals  =  TRUE;
	bIncludeTextureCoords = TRUE;
	bIncludeVertexColors = FALSE;
	bIncludeObjGeom = TRUE;
	bIncludeObjShape = FALSE;
	bIncludeObjCamera = FALSE;
	bIncludeObjLight = FALSE;
	bIncludeObjHelper = TRUE;
	nStaticFrame = 0;

	wideformat = FALSE;
	streaming = FALSE;
	compression = UNCOMPRESSED;
}

AWDExporter::~AWDExporter() 
{

}

// Dummy function for progress bar
DWORD WINAPI fn(LPVOID arg)
{
	return(0);
}

int AWDExporter::ExtCount()
{
	return 1;
}

const TCHAR *AWDExporter::Ext(int n)
{		
	switch(n) {
	case 0:
		return _T("AWD");
	}
	return _T("");
}

const TCHAR *AWDExporter::LongDesc()
{
	return _T("AWD 2.0(away3D) exporter");
}
	
const TCHAR *AWDExporter::ShortDesc() 
{			
	return _T("Away3D format (AWD2)");
}

const TCHAR *AWDExporter::AuthorName()
{
	return _T("Pierre Lepers");
}

const TCHAR *AWDExporter::CopyrightMessage() 
{
	return _T("");
}

const TCHAR *AWDExporter::OtherMessage1() 
{		
	//TODO: Return Other message #1 if any
	return _T("");
}

const TCHAR *AWDExporter::OtherMessage2() 
{		
	//TODO: Return other message #2 in any
	return _T("");
}

unsigned int AWDExporter::Version()
{				
	// Version number * 100 (i.e. v3.01 = 301)
	return VERSION;
}

static INT_PTR CALLBACK AboutBoxDlgProc(HWND hWnd, UINT msg, 
	WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_INITDIALOG:
		CenterWindow(hWnd, GetParent(hWnd)); 
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			EndDialog(hWnd, 1);
			break;
		}
		break;
		default:
			return FALSE;
	}
	return TRUE;
}  

void AWDExporter::ShowAbout(HWND hWnd)
{			
	DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, AboutBoxDlgProc, 0);
}

BOOL AWDExporter::SupportsOptions(int ext, DWORD options)
{
	//Decide which options to support.  Simply return true for each option supported by each Extension the exporter supports.
	//return(options == SCENE_EXPORT_SELECTED) ? TRUE : FALSE;
	// don't support selected only
	return FALSE;
}


int	AWDExporter::DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts, DWORD options)
{
	
	int fd;

	// Grab the interface pointer.
	ip = i;

	// Set a global prompt display switch
	showPrompts = suppressPrompts ? FALSE : TRUE;
	exportSelected = (options & SCENE_EXPORT_SELECTED) ? TRUE : FALSE;

	

	// Get the options the user selected the last time
	ReadConfig();

	if(showPrompts) {
		if (!DialogBoxParam(hInstance, 
				MAKEINTRESOURCE(IDD_AWDEXPORT_DLG), 
				GetActiveWindow(), 
				AWDExporterOptionsDlgProc, (LPARAM)this) ){
			return 1;
		}
	}

	WriteConfig();

	#ifdef LOG
	logfile = _tfopen("C:\\Users\\lepersp\\Desktop\\awdexplog.txt",_T("wt")); 
	#endif


	// Open file handle
	if( _sopen_s( &fd,name, _O_CREAT|_O_RDWR|_O_BINARY|_O_TRUNC, _SH_DENYNO, _S_IWRITE ) != 0 ){
		return FALSE;
	}

	// Startup the progress bar.
	ip->ProgressStart("Exporting...", TRUE, fn, NULL);

	docawd = new AWD( GetCompression(), (wideformat<<1)|streaming );

	// init caches
	awd_ncache *ncache;
	ncache = (awd_ncache *)malloc(sizeof(awd_ncache));
	awd_ncache_init(ncache);
	
	geomcache = (awd_ncache *)malloc(sizeof(awd_ncache));
	awd_ncache_init(geomcache);

	// Get a total node count by traversing the scene
	// We don't really need to do this, but it doesn't take long, and
	// it is nice to have an accurate progress bar.
	nTotalNodeCount = 0;
	nCurNode = 0;

	// init 3DXI system
	
	pIgame = GetIGameInterface();
	pIgame->InitialiseIGame( false );
	pIgame->SetStaticFrame(GetStaticFrame());
	

	PreProcess(ip->GetRootNode(), nTotalNodeCount);

	PrepareMeta( docawd );
	PrepareNamespace( docawd );
	PrepareMaterials( docawd, ncache );
	PrepareScene( ncache, docawd );
	PrepareBlocks( ncache, docawd );


	BOOL flushsucces = docawd->flush(fd);
	delete docawd;
	_close( fd );

	awd_ncache_free( ncache );
	awd_ncache_free( geomcache );

	pIgame->ReleaseIGame();

	// We're done. Finish the progress bar.
	ip->ProgressEnd();


	#ifdef LOG
	fclose(logfile);
	#endif

	return flushsucces;
}

void AWDExporter::PreProcess(INode* node, int& nodeCount)
{
	nodeCount++;

	// Add the nodes material to out material list
	// Null entries are ignored when added...

	Mtl * mtl = node->GetMtl();

	mtlList.AddMtl(node->GetMtl());
	
	// recursively
	for (int c = 0; c < node->NumberOfChildren(); c++) {
		PreProcess(node->GetChildNode(c), nodeCount);
	}
}

void AWDExporter::PrepareMeta( AWD *awd ) {
	MSTR version;
	version.printf( "%1.2f", VERSION );

	AWDMetaData *lawd_meta = new AWDMetaData();
	lawd_meta->generator_name = "Max 3ds AWDEncoder";
	lawd_meta->generator_version = version.data();
	docawd->set_metadata(lawd_meta);
}

void AWDExporter::PrepareNamespace( AWD *awd ) {
	const char * uri = NAMESPACE_URI;
	SetNamespace( new AWDNamespace( uri, strlen( uri ) ) );
	awd->add_namespace( GetNamespace() );
}

void AWDExporter::PrepareScene(awd_ncache *ncache, AWD *awd )
{
	INode *rootnode = ip->GetRootNode();
	ObjectState os = rootnode->EvalWorldState(0);

	AWDSceneBlock *scene = new AWDScene( NULL, 0 );
	_nameElement( rootnode, scene );

	awd->add_scene_block( scene );
	awd_ncache_add( ncache, os.obj, scene );

	
}

void AWDExporter::PrepareBlocks(awd_ncache *ncache, AWD *awd )
{
	int numChildren = ip->GetRootNode()->NumberOfChildren();
	
	// Call our node enumerator.
	// The nodeEnum function will recurse into itself and 
	// export each object found in the scene.
	
	for (int idx=0; idx<numChildren; idx++) {
		if (ip->GetCancel())
			break;
		nodeEnum(ip->GetRootNode()->GetChildNode(idx), awd, ncache );
	}
}

void AWDExporter::Export(INode* pNode, int iTreeDepth)
{
    MCHAR* pNodeName = pNode->GetName();
    int nChildren = pNode->NumberOfChildren();

	//fprintf( pStream, "[%s] : %i children", pNodeName, nChildren);
    //Write(iTreeDepth, "[%s] : %i children", pNodeName, nChildren);

    iTreeDepth++;
    for (int i = 0; i < nChildren; i++)
    {
        Export(pNode->GetChildNode(i), iTreeDepth);
    }
}

BOOL AWDExporter::nodeEnum(INode* node, AWD *awd, awd_ncache *ncache ) 
{
	
	if(exportSelected && node->Selected() == FALSE)
		return TREE_CONTINUE;

	nCurNode++;
	ip->ProgressUpdate((int)((float)nCurNode/nTotalNodeCount*100.0f)); 

	// Stop recursing if the user pressed Cancel 
	if (ip->GetCancel())
		return FALSE;

	
	// Only export if exporting everything or it's selected
	if(!exportSelected || node->Selected()) {
		_prepareSceneBlock(node, awd, ncache );
	}	
	
	// For each child of this node, we recurse into ourselves 
	// until no more children are found.
	for (int c = 0; c < node->NumberOfChildren(); c++) {
		if (!nodeEnum(node->GetChildNode(c), awd, ncache ) )
			return FALSE;
	}
	

	return TRUE;
}

void AWDExporter::_prepareSceneBlock(INode* node, AWD *awd, awd_ncache *ncache ) {

	// The ObjectState is a 'thing' that flows down the pipeline containing
	// all information about the object. By calling EvalWorldState() we tell
	// max to eveluate the object at end of the pipeline.
	ObjectState os = node->EvalWorldState(0); 

	AWDSceneBlock *scene_block;

	awd_float64 *mtx;
    AWDSceneBlock *parent;

	INode *parentnode;
	ObjectState pos;

	// The obj member of ObjectState is the actual object we will export.
	if (os.obj) {

		switch(os.obj->SuperClassID()) {
			case GEOMOBJECT_CLASS_ID: 
				if (GetIncludeObjGeom()) scene_block = _prepareMeshInst( node, awd, ncache ); 
				break;
			case HELPER_CLASS_ID: 
				if (GetIncludeObjGeom()) scene_block = _prepareHelper( node, awd, ncache ); 
				break;
			//case CAMERA_CLASS_ID:
			//	if (GetIncludeObjCamera()) scene_block = _prepareCamera( node, awd, ncache ); 
			//	break;
			default :
				return;
		}



		if( scene_block ) {
			mtx = NULL;
			_nameElement( node, scene_block );
			mtx = get_node_transform_float64( node, GetStaticFrame(), NULL );

			scene_block->set_transform( mtx );

			awd->add_scene_block(scene_block);
			awd_ncache_add(ncache, (InterfaceServer*)os.obj, scene_block);

			// link parent
			parentnode = node->GetParentNode();
			if (parentnode != NULL) {
				pos = parentnode->EvalWorldState(0);
				if( pos.obj  != NULL ) {
					parent = (AWDSceneBlock *) awd_ncache_get( ncache, (InterfaceServer*)pos.obj );
					if (parent != NULL) {
						parent->add_child( scene_block );
					}
				}
			}
	

			
		}
	}

}

AWDMeshData* AWDExporter::_prepareMeshData(INode* node, AWD *awd, awd_ncache *ncache)
{
	int i;
	int m;
	Mtl* nodeMtl = node->GetMtl();
	Matrix3 tm = node->GetObjTMAfterWSM( GetStaticFrame() );
	BOOL negScale = TMNegParity(tm);

	IGameSkin *	skin = NULL;
	SkinData * skindata;
	int skinSize;

	//pivot point. Need to transform each vertices using pivot transform 
	Matrix3 piv(1);
	Point3 pos = node->GetObjOffsetPos();
	piv.PreTranslate(pos);
	Quat quat = node->GetObjOffsetRot();
	PreRotateMatrix(piv, quat);
	ScaleValue scaleValue = node->GetObjOffsetScale();
	ApplyScaling(piv, scaleValue);

	
	// AWD DATAS
	AWDMeshData *lawd_md;
	AWDSubMesh *lawd_sub;
	AWD_str_ptr lawd_data;

	fprintf( logfile, " _prepareMeshData : %s \n", node->GetName() );
	fflush( logfile );
	
	int vindices[3];

	if (negScale) {
		vindices[0] = 0;
		vindices[1] = 1;
		vindices[2] = 2;
	}
	else {
		vindices[0] = 2;
		vindices[1] = 1;
		vindices[2] = 0;
	}

	
	ObjectState os = node->EvalWorldState( GetStaticFrame() );
	if (!os.obj || os.obj->SuperClassID()!=GEOMOBJECT_CLASS_ID) {
		return NULL; // Safety net. This shouldn't happen.
	}

	// skip skeletons bones
	if( os.obj->ClassID() == BONE_OBJ_CLASSID )
		return NULL;

	lawd_md = (AWDMeshData *) awd_ncache_get( geomcache, os.obj );
	if( lawd_md )
		return lawd_md;
	
	BOOL needDel;
	TriObject* tri = GetTriObjectFromNode( node, GetStaticFrame(), needDel );
	if (!tri) {
		// sanity, should never happen
		return NULL;
	}
	
	Mesh* mesh = &tri->GetMesh();

	if(  mesh->numFaces == 0 ) {
		// empty geom, return null to force creation of AWDContainer instead of AWDMeshInst
		return NULL;
	}


	char* name = node->GetName();
	lawd_md = new AWDMeshData(name, strlen(name) );

	if ( ! GetIncludeMesh() ) {
		return lawd_md;
	}

	mesh->buildNormals();

	// 3DXI init
	IGameNode * ignode = pIgame->GetIGameNode( node );
	

	// BUFFERS
	unsigned int indexbufferlen;
	awd_uint32* indexbuffer;
	awd_uint32* indexstream;
	awd_uint32* bindexstream;
	awd_float64* vertexstream;


	// STREAMS DATAS
	VertexTable *vtable = new VertexTable( );
	vtable->pivot = &piv;

	
	

	DWORD vertindex;
	DWORD tvertindex;
	Point3 xyz;
	Point3 norms;
	VertColor vertCol;

	UVVert uv;
	BOOL hasUV;

	// MATERIAL IDS 
	int pmtlc = 0;
	MtlID currentmtl = mesh->getFaceMtlIndex(0);
	MtlID nextmtl = 0;
	BOOL hasnext = 0;

	int t;
	int data_len;

	int COMP_CHAN = -2; // alpha map channel
	
	
	//fprintf( logfile, "	indexbuffer mallo size : %i \n", mesh->getNumFaces() * 3 * sizeof(awd_uint32) );
	//fflush( logfile );

	// allocate max size index;
	indexbuffer = (awd_uint32*) malloc( mesh->getNumFaces() * 3 * sizeof(awd_uint32) );
	//vertexbuffer = (awd_float64*) malloc( MAX_VERTEX_PER_BUFF * 3 * sizeof(awd_float64) );
	

	// MATLIST
	matid_list matlist = get_matid_list( mesh );
	

	hasUV = ( mesh->getNumTVerts() != 0 );

	if( GetIncludeIKJoints() ) {
		skin = GetMeshSkin( ignode );
		skinSize = get_skin_size( skin );
		vtable->skinSize = skinSize;

		fprintf( logfile, "	skinSize : %i \n", skinSize );
		fflush( logfile );

		if( skin != NULL && skinSize > 0 ) 
			ExportSkeleton( skin, awd, ncache, lawd_md );
	}

	fprintf( logfile, "	num sm group : %i \n", matlist.len );
	fflush( logfile );

	MeshNormalSpec* ns = mesh->GetSpecifiedNormals();
	BOOL _specifiedNormals = (ns && ns->GetNumNormals());

	BOOL _compositeGeom = ( mesh->mapSupport( COMP_CHAN ) && mesh->getNumMapVerts( COMP_CHAN ) > 0 );

	BOOL _secifiedColors = ( mesh->numCVerts > 0 );

	
	// COMPOSITE
	CompositeGeom *composite;
	unsigned int faceIndices[3];
	
	if( _compositeGeom )
		composite = new CompositeGeom();

	for (m=0; m<matlist.len; m++) {
		
		indexbufferlen = 0;
		currentmtl = matlist.list[m];
		
		fprintf( logfile, "		sm group : %i \n", currentmtl );
		fflush( logfile );

		
		if( _compositeGeom ){
			composite->allocate( mesh, currentmtl );
		}


		for (i=0; i<mesh->getNumFaces(); i++) {

			

			if( mesh->faces[i].getMatID() == currentmtl ) {

				
				// #######################TEST

				
				
				// #######################TEST
				
				for (t=0; t<3; t++) {

					vertindex = mesh->faces[i].v[vindices[t]];
					xyz = mesh->verts[ vertindex ];

					if(_specifiedNormals)
					{
						norms = ns->GetNormal(i, vindices[t]);
					}
					else
					{
						norms = getVertexNormal(mesh, i, mesh->getRVertPtr(vertindex));
					}
					


					if( hasUV ) {
						tvertindex = mesh->tvFace[i].t[vindices[t]];
						uv = mesh->tVerts[ tvertindex ];
					}

					if( skin != NULL ) {
						ExportSkinData( skin, vertindex, skinSize, skindata );
						faceIndices[t] = vtable->AddVertex( xyz, norms, uv, skindata );
					} else {
						faceIndices[t] = vtable->AddVertex( xyz, norms, uv, NULL );
					}


					

					indexbuffer[ indexbufferlen ] = faceIndices[t];
					indexbufferlen += 1;

				}

				if( _compositeGeom ) {
					composite->pushFace( faceIndices[0], faceIndices[1], faceIndices[2], composite->getFaceGroup( mesh, i ) );
				}

			
			}

		}

		fprintf( logfile, "		indexbuffer size : %i \n", indexbufferlen );
		fflush( logfile );


		//fprintf( logfile, "		sm num verts : %i \n", vtable->GetNumVertex() );
		//fflush( logfile );


		// finalize subgeom
		composite->debug( logfile );

		lawd_sub = new AWDSubMesh();

		if( _compositeGeom && composite->getNumGroups() > 1 ) {
			indexstream = (awd_uint32*) malloc( composite->bufferLen * sizeof(awd_uint32) );
			memcpy( indexstream, composite->cvBuffer, composite->bufferLen * sizeof(awd_uint32) );
			lawd_data.ui32 = indexstream;
			lawd_sub->add_stream( COMPOSITE_TRIANGLES, lawd_data, composite->bufferLen);
		} else {
			indexstream = (awd_uint32*) malloc( indexbufferlen * sizeof(awd_uint32) );
			memcpy( indexstream, indexbuffer, indexbufferlen * sizeof(awd_uint32) );
			lawd_data.ui32 = indexstream;
			lawd_sub->add_stream( TRIANGLES, lawd_data, indexbufferlen);
		}
		

		data_len = vtable->GetNumVertex() * 3;
		vertexstream = vtable->getVertexStream( NULL );
		lawd_data.f64 = vertexstream;
		lawd_sub->add_stream(VERTICES, lawd_data, data_len);
		

		if (GetIncludeNormals() ) { 
			vertexstream = vtable->getNormalStream( NULL );
			lawd_data.f64 = vertexstream;
			lawd_sub->add_stream(VERTEX_NORMALS, lawd_data, data_len);
		}

		if( hasUV && GetIncludeTextureCoords() ) {
			data_len = vtable->GetNumVertex() * 2;
			vertexstream = vtable->getUvsStream( NULL );
			lawd_data.f64 = vertexstream;
			lawd_sub->add_stream(UVS, lawd_data, data_len);
		}

		if( skin != NULL ) {
			data_len = vtable->GetNumVertex() * skinSize;
			vertexstream = vtable->getSweightStream( NULL );
			bindexstream = vtable->getSindicesStream( NULL );
			lawd_data.f64 = vertexstream;
			lawd_sub->add_stream(VERTEX_WEIGHTS, lawd_data, data_len);
			lawd_data.ui32 = bindexstream;
			lawd_sub->add_stream(JOINT_INDICES, lawd_data, data_len);
		}

		//fprintf( logfile, "		add submesh \n" );
		//fflush( logfile );

		lawd_md->add_sub_mesh(lawd_sub);

		//vtable->PrintStats( logfile );

		fprintf( logfile, "		reset \n" );
		fflush( logfile );

		

		vtable->reset();

		if( _compositeGeom )
			composite->reset();
	}

	//fprintf( logfile, "		delete vtable \n" );
	//fflush( logfile );

	delete vtable;
	
	free( indexbuffer );

	ignode->ReleaseIGameObject();
	
	awd_ncache_add(geomcache, (InterfaceServer*)os.obj, lawd_md);

	awd->add_mesh_data(lawd_md);
	
	if( needDel ) {
		tri->DeleteMe();
	}
	//return NULL;
	return lawd_md;
	
	
}

IGameSkin *	AWDExporter::GetMeshSkin( IGameNode * ignode ) {
	
	IGameModifier * m;
	IGameObject * obj = ignode->GetIGameObject();

	int numMod = obj->GetNumModifiers();

	
	for(int i=0;i<numMod;i++)
	{
		m = obj->GetIGameModifier(i);

		if( m->IsSkin() )
		{
			return (IGameSkin*)m;
		}

	}

	return NULL;

}

void
AWDExporter::ExportSkinData( IGameSkin * skin, int vertexIndex, int size, SkinData * skindata )
{
	int numbones = skin->GetTotalBoneCount() - 1;

	fprintf( logfile, "		ExportSkinData : vindex %i			size %i \n", vertexIndex, size );
	fflush( logfile );


	INode * bone;

	skindata->jindices = (awd_uint32*) malloc( size * sizeof( awd_uint32 ) );
	skindata->jweights = (awd_float64*) malloc( size * sizeof( awd_float64 ) );

	for(int t=0;t<size;t++ ) {
		skindata->jindices[t] = 0;
		skindata->jweights[t] = 0.0f;
	}

	int type = skin->GetVertexType( vertexIndex );

	fprintf( logfile, "		ExportSkinData : type %i \n", type );
	fflush( logfile );

	if(type==IGameSkin::IGAME_RIGID)
	{
		bone = skin->GetBone(vertexIndex,0);
		
		if( bone == NULL ) {
			fprintf( logfile, "			Bone NULL \n" );
			fflush( logfile );
		}
		fprintf( logfile, "			jindex %i \n", skin->GetBoneIndex( bone ) );
		fprintf( logfile, "			export %i %f\n", numbones - skin->GetBoneIndex( bone ), skin->GetWeight(vertexIndex,0));
		fflush( logfile );


		skindata->jindices[0] = numbones - skin->GetBoneIndex( bone );
		skindata->jweights[0] = skin->GetWeight(vertexIndex,0);
	}
	else //blended
	{
		
		fprintf( logfile, "		ExportSkinData : nbones %i \n", skin->GetNumberOfBones(vertexIndex) );
		fflush( logfile );

		for(int y=0;y<skin->GetNumberOfBones(vertexIndex);y++)
		{

			fprintf( logfile, "			export %i %f\n", numbones - skin->GetBoneIndex( bone ), skin->GetWeight(vertexIndex,y));

			bone = skin->GetBone(vertexIndex,y);
		
			skindata->jindices[y] = numbones - skin->GetBoneIndex( bone );
			skindata->jweights[y] = skin->GetWeight(vertexIndex,y);
		}
	}

}

void 
AWDExporter::ExportSkeleton( IGameSkin * skin, AWD *awd, awd_ncache *ncache, AWDMeshData* awd_mesh ) {

	int numBones = skin->GetTotalBoneCount();
	int index;
	GMatrix boneTM;

	AWDMaxSkeleton * skel = new AWDMaxSkeleton( "skeleton", 8, logfile );

	INode * bone;
	INode * parent;


	awd_jcache *bcache;
	bcache = (awd_jcache *)malloc(sizeof(awd_jcache));
	awd_jcache_init(bcache);

	AWDMaxSkeletonJoint * joint;
	AWDMaxSkeletonJoint * pjoint;

	for( int i = 0; i<numBones; i++ ) {

		index = numBones - i - 1;

		bone = skin->GetBone( index, false );

		if( bone == NULL ) return;

		skin->GetInitBoneTM( skin->GetIGameBone( index, false ), boneTM );
		
		joint = new AWDMaxSkeletonJoint( bone->GetName(), strlen(bone->GetName() ), gmatrix_to_float64(boneTM.Inverse(), NULL), logfile  );
		
		skel->add_joint( joint );

		awd_jcache_add( bcache, bone, joint );
	}

	fprintf( logfile, "ExportSkeleton pass 2 \n" );
	fflush( logfile );

	// 2nd Pass for parenting

	for( int i = 0; i<numBones; i++ ) {

		bone = skin->GetBone( i, false );
		parent = bone->GetParentNode();

		joint = (AWDMaxSkeletonJoint*) awd_jcache_get( bcache, bone );
		pjoint = (AWDMaxSkeletonJoint*) awd_jcache_get( bcache, parent );
		
		if( pjoint != NULL ) {
			pjoint->add_child_joint( joint );
		}

	}

	fprintf( logfile, "ExportSkeleton finale \n" );
	fflush( logfile );

	awd->add_skeleton( skel );
	awd_mesh->set_skeleton( skel );

	awd_jcache_free( bcache );


	ExportSkeletonAnim( skin, awd, ncache, awd_mesh );
}

void 
AWDExporter::ExportSkeletonAnim( IGameSkin * skin, AWD *awd, awd_ncache *ncache, AWDMeshData* awd_mesh ) {

	fprintf( logfile, "ExportSkeletonAnim  \n" );
	fflush( logfile );

	int numBones = skin->GetTotalBoneCount();
	int k;
	int i;
	int index;

	AWDSkeletonAnimation * skelanim = new AWDSkeletonAnimation( "skeletonAnim", 12 );

	IGameNode * bone;
	IGameObject * obj;

	IGameControl * pGameControl;

	IGameKeyTab Key;

	bool isAnimated = false;

	int animLen = 0;

	fprintf( logfile, "ExportSkeletonAnim numbones : %i \n", numBones );
	fflush( logfile );

	for( i = 0; i<numBones; i++ ) {

		
		

		bone = skin->GetIGameBone( i, false );
		obj = bone->GetIGameObject();

		fprintf( logfile, "		ExportSkeletonAnim bone : %i %s \n", i, bone->GetName() );
		fflush( logfile );
		
		pGameControl = bone->GetIGameControl();

		if( pGameControl == NULL ) {
			fprintf( logfile, "		pGameControl null\n" );
			fflush( logfile );
			continue;
		}

		fprintf( logfile, "		ExportSkeletonAnim  test1	%b\n", pGameControl->IsAnimated(IGAME_POS) );
		fflush( logfile );

		if ((pGameControl->IsAnimated(IGAME_POS)) || pGameControl->IsAnimated(IGAME_ROT) || pGameControl->IsAnimated(IGAME_SCALE)) {
			fprintf( logfile, "		ExportSkeletonAnim  test2	\n" );
			fflush( logfile );
			isAnimated = true;
		

			if( pGameControl->GetFullSampledKeys( Key, 1, IGAME_TM, true ) ) {
				if( animLen < Key.Count() ) animLen = Key.Count();

				fprintf( logfile, "		ExportSkeletonAnim  pass1	bone %i\n", i );
				fprintf( logfile, "									alen %i\n", Key.Count() );
				fflush( logfile );
			}
			Key.Delete( 0, Key.Count() );
		//Key.Delete();
		}

		

		bone->ReleaseIGameObject();
	}

	if( isAnimated == 0 )
		return;

	awd_float64 * tlist = (awd_float64*) malloc( animLen* numBones * 16 * sizeof( awd_float64 ) );
	awd_float64 * mtx = (awd_float64*) malloc(16 * sizeof(awd_float64));
	awd_float64 * nmtx = awdutil_id_mtx4x4( NULL );

	for( i = 0; i<numBones; i++ ) {

		bone = skin->GetIGameBone( numBones - i - 1, false );
		obj = bone->GetIGameObject();

		pGameControl = bone->GetIGameControl();

		k = 0;

		if ((pGameControl->IsAnimated(IGAME_POS)) || pGameControl->IsAnimated(IGAME_ROT) || pGameControl->IsAnimated(IGAME_SCALE)) {

			pGameControl->GetFullSampledKeys( Key, 1, IGAME_TM, true );
			
			for( k = 0; k < Key.Count(); k++ ) {
				memcpy( tlist+ ((k*numBones + i)*16),  gmatrix_to_float64( Key[k].sampleKey.gval, mtx ), 16 * sizeof( awd_float64 ) ); 
				fprintf( logfile, "		bone mtx	\n				%1.2f	%1.2f	%1.2f	%1.2f\n", Key[k].sampleKey.gval.GetRow(0).x, Key[k].sampleKey.gval.GetRow(0).y, Key[k].sampleKey.gval.GetRow(0).z, Key[k].sampleKey.gval.GetRow(0).w );
				fprintf( logfile, "				%1.2f	%1.2f	%1.2f	%1.2f\n", Key[k].sampleKey.gval.GetRow(1).x, Key[k].sampleKey.gval.GetRow(1).y, Key[k].sampleKey.gval.GetRow(1).z, Key[k].sampleKey.gval.GetRow(1).w );
				fprintf( logfile, "				%1.2f	%1.2f	%1.2f	%1.2f\n", Key[k].sampleKey.gval.GetRow(2).x, Key[k].sampleKey.gval.GetRow(2).y, Key[k].sampleKey.gval.GetRow(2).z, Key[k].sampleKey.gval.GetRow(2).w );
				fprintf( logfile, "				%1.2f	%1.2f	%1.2f	%1.2f\n", Key[k].sampleKey.gval.GetRow(3).x, Key[k].sampleKey.gval.GetRow(3).y, Key[k].sampleKey.gval.GetRow(3).z, Key[k].sampleKey.gval.GetRow(3).w );
			}
			Key.Delete( 0, Key.Count() );

		}
		for( ;k<animLen; k++ )
			memcpy( tlist+ ((k*numBones + i)*16),  nmtx, 16 * sizeof( awd_float64 ) ); 
		
		
		bone->ReleaseIGameObject();

	}

	AWDSkeletonPose * skelpose;

	for( k=0 ;k<animLen; k++ ) {

		skelpose = new AWDSkeletonPose( "pose", 4 );

		for( i = 0; i<numBones; i++ ) {
			skelpose->set_next_transform( tlist + ((k*numBones + i)*16) );
		}

		awd->add_skeleton_pose( skelpose );

		skelanim->set_next_frame_pose( skelpose, 1 );
	}
	
	awd->add_skeleton_anim( skelanim );
	
	awd_mesh->set_skeleton_anim( skelanim );


}

AWDSceneBlock*
AWDExporter::_prepareMeshInst(INode *node , AWD *awd, awd_ncache *ncache) {


	AWDMeshInst *lawd_inst;
    
	
	ObjectState os = node->EvalWorldState(0);
	if (!os.obj)
		return NULL;
	
	// Targets are actually geomobjects, but we will export them
	// from the camera and light objects, so we skip them here.
	if (os.obj->ClassID() == Class_ID(TARGET_CLASS_ID, 0))
		return NULL;
	
	AWDMeshData *meshdata;

	if (os.obj->CanConvertToType(Class_ID(TRIOBJ_CLASS_ID, 0))) {
		meshdata = _prepareMeshData( node, awd, ncache );
	}
	
	// Mesh is empty, so it replaced by a container object
	if( meshdata == NULL ) {
		return new AWDContainer(NULL, 0);
	}
	
	lawd_inst = new AWDMeshInst(NULL, 0, meshdata);

	if( meshdata->get_skeleton_anim() != NULL ) {
		lawd_inst->set_skeleton_anim(  meshdata->get_skeleton_anim() );
	}
	
	ApplyMeshMaterials( lawd_inst, node, ncache );

	return lawd_inst;
}

AWDSceneBlock*
AWDExporter::_prepareHelper(INode *node , AWD *awd, awd_ncache *ncache) {
	// Helpers are exported as empty containers
	AWDContainer *lawd_help = new AWDContainer(NULL, 0);
	return lawd_help;
}

AWDSceneBlock*
AWDExporter::_prepareCamera(INode *node , AWD *awd, awd_ncache *ncache) {

	AWDCamera *awd_cam;
	AWD_cam_type cam_type;
	AWD_lens_type lens_type;


	CameraState cs;
	Interval valid = FOREVER;
	TimeValue t = GetStaticFrame();

	ObjectState os = node->EvalWorldState(t);
	CameraObject *cam = (CameraObject *)os.obj;
	cam->EvalCameraState(t,valid,&cs);


	INode* target = node->GetTarget();
	if (target)	cam_type = AWD_CAM_TARGET;
	else		cam_type = AWD_CAM_FREE;

	if( &cs.isOrtho )	lens_type = AWD_LENS_ORTHO;
	else				lens_type = AWD_LENS_PERSPECTIVE;
	

	awd_cam = new AWDCamera( NULL, 0, cam_type, lens_type );

	// TODO 
	// handle target, seems to be missing in awdlib...

	return awd_cam;
}

void
AWDExporter::ApplyMeshMaterials( AWDMeshInst * awdmesh, INode *node, awd_ncache *ncache ) {
	Mtl* mtl = node->GetMtl();
	AWDMaterial * awdmat;

	const char* matprop = "materials";
	AWD_field_ptr propptr;
	MSTR matname;
	char * mncpy;
	

	int i;

	if (mtl) {

		if (mtl->NumSubMtls() == 0)  {	// Simple mat

			awdmat = (AWDMaterial *) awd_ncache_get( ncache, (InterfaceServer*)mtl );
			if (awdmat != NULL) {
				awdmesh->add_material( awdmat );
			}

			matname = mtl->GetName();
			
		} else {						// Multi mat
			matname = "";
			for (i=0; i<mtl->NumSubMtls(); i++) {
				Mtl* subMtl = mtl->GetSubMtl(i);
				if (subMtl) {
					awdmat = (AWDMaterial *) awd_ncache_get( ncache, (InterfaceServer*)subMtl );
					if (awdmat != NULL) {
						awdmesh->add_material( awdmat );
					}
					matname.append( subMtl->GetName() );
					if( i < mtl->NumSubMtls()-1 )
						matname.append( "," );
				}
			}
		}

		mncpy = (char *) malloc( (matname.length()+1)*sizeof( char ) ); 
		strcpy( mncpy, matname.data() );
		propptr.str = mncpy;
		awdmesh->set_attr( GetNamespace(), matprop, strlen( matprop ), propptr, strlen( mncpy ), AWD_FIELD_STRING );
	}
}

void
AWDExporter::_nameElement( INode* node, AWDNamedElement* element )
{
	char* name = node->GetName();
	int len = strlen( name );
	element->set_name(name, len );
}


// Determine is the node has negative scaling.
// This is used for mirrored objects for example. They have a negative scale factor
// so when calculating the normal we should take the vertices counter clockwise.
// If we don't compensate for this the objects will be 'inverted'.
BOOL AWDExporter::TMNegParity(Matrix3 &m)
{
	return (DotProd(CrossProd(m.GetRow(0),m.GetRow(1)),m.GetRow(2))<0.0)?1:0;
}

// Return a pointer to a TriObject given an INode or return NULL
// if the node cannot be converted to a TriObject
TriObject* AWDExporter::GetTriObjectFromNode(INode *node, TimeValue t, int &deleteIt)
{
	deleteIt = FALSE;
	Object *obj = node->EvalWorldState(t).obj;
	if (obj->CanConvertToType(Class_ID(TRIOBJ_CLASS_ID, 0))) { 
		TriObject *tri = (TriObject *) obj->ConvertToType(t, 
			Class_ID(TRIOBJ_CLASS_ID, 0));
		// Note that the TriObject should only be deleted
		// if the pointer to it is not equal to the object
		// pointer that called ConvertToType()
		if (obj != tri) deleteIt = TRUE;
		return tri;
	}
	else {
		return NULL;
	}
}

/*MtlID *
getMaterialsIds( Mesh mesh ) {
	MtlID ids[256];
	int i;

	for (i=0; i<mesh->getNumFaces(); i++) {

	}

	return ids;
}*/

Point3 AWDExporter::getVertexNormal(Mesh* mesh, int faceNo, RVertex* rv)
{
	Face* f = &mesh->faces[faceNo];
	DWORD smGroup = f->smGroup;
	int numNormals = 0;
	Point3 vertexNormal;

	
	

	
	// Is normal specified
	// SPCIFIED is not currently used, but may be used in future versions.
	if (rv->rFlags & SPECIFIED_NORMAL) {
		vertexNormal = rv->rn.getNormal();
	}
	// If normal is not specified it's only available if the face belongs
	// to a smoothing group
	else if ((numNormals = rv->rFlags & NORCT_MASK) != 0 && smGroup) {
		// If there is only one vertex is found in the rn member.
		if (numNormals == 1) {
			vertexNormal = rv->rn.getNormal();
		}
		else {
			// If two or more vertices are there you need to step through them
			// and find the vertex with the same smoothing group as the current face.
			// You will find multiple normals in the ern member.
			for (int i = 0; i < numNormals; i++) {
				if (rv->ern[i].getSmGroup() & smGroup) {
					vertexNormal = rv->ern[i].getNormal();
				}
			}
		}
	}
	else {
		// Get the normal from the Face if no smoothing groups are there
		vertexNormal = mesh->getFaceNormal(faceNo);
	}
	
	return vertexNormal;
}



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

/* -----------------------------------------------------------------------------------------------
																		MtlKeeper
-----------------------------------------------------------------------------------------------*/

BOOL MtlKeeper::AddMtl(Mtl* mtl)
{
	if (!mtl) {
		return FALSE;
	}

	int numMtls = mtlTab.Count();
	for (int i=0; i<numMtls; i++) {
		if (mtlTab[i] == mtl ) {
			return FALSE;
		}
	}
	mtlTab.Append(1, &mtl, 25);

	return TRUE;
}

int MtlKeeper::GetMtlID(Mtl* mtl)
{
	int numMtls = mtlTab.Count();
	for (int i=0; i<numMtls; i++) {
		if (mtlTab[i] == mtl) {
			return i;
		}
	}
	return -1;
}

int MtlKeeper::Count()
{
	return mtlTab.Count();
}

Mtl* MtlKeeper::GetMtl(int id)
{
	return mtlTab[id];
}




/****************************************************************************

 Configuration.
 To make all options "sticky" across sessions, the options are read and
 written to a configuration file every time the exporter is executed.

 ****************************************************************************/

TSTR AWDExporter::GetCfgFilename()
{
	TSTR filename;
	
	filename += ip->GetDir(APP_PLUGCFG_DIR);
	filename += "\\";
	filename += CFGFILENAME;

	return filename;
}

// NOTE: Update anytime the CFG file changes


BOOL AWDExporter::ReadConfig()
{
	TSTR filename = GetCfgFilename();
	FILE* cfgStream;

	cfgStream = fopen(filename, "rb");
	if (!cfgStream)
		return FALSE;

	// First item is a file version
	int fileVersion = _getw(cfgStream);

	if (fileVersion > CFG_VERSION) {
		// Unknown version
		fclose(cfgStream);
		return FALSE;
	}

	

	SetIncludeMesh(fgetc(cfgStream));
	SetIncludeAnim(fgetc(cfgStream));
	SetIncludeMtl(fgetc(cfgStream));
	SetIncludeMeshAnim(fgetc(cfgStream));
	SetIncludeCamLightAnim(fgetc(cfgStream));
	SetIncludeIKJoints(fgetc(cfgStream));
	SetIncludeNormals(fgetc(cfgStream));
	SetIncludeTextureCoords(fgetc(cfgStream));
	SetIncludeVertexColors(fgetc(cfgStream));
	SetIncludeObjGeom(fgetc(cfgStream));
	SetIncludeObjShape(fgetc(cfgStream));
	SetIncludeObjCamera(fgetc(cfgStream));
	SetIncludeObjLight(fgetc(cfgStream));
	SetIncludeObjHelper(fgetc(cfgStream));
	SetWideFormat(fgetc(cfgStream));
	SetStreaming(fgetc(cfgStream));
	SetIncludeMaps(fgetc(cfgStream));

	SetCompression((AWD_compression)_getw(cfgStream));
	

	fclose(cfgStream);

	return TRUE;
}

void AWDExporter::WriteConfig()
{
	TSTR filename = GetCfgFilename();
	FILE* cfgStream;

	cfgStream = fopen(filename, "wb");
	if (!cfgStream)
		return;

	// Write CFG version
	_putw(CFG_VERSION,				cfgStream);

	fputc(GetIncludeMesh(),			cfgStream);
	fputc(GetIncludeAnim(),			cfgStream);
	fputc(GetIncludeMtl(),			cfgStream);
	fputc(GetIncludeMeshAnim(),		cfgStream);
	fputc(GetIncludeCamLightAnim(),	cfgStream);
	fputc(GetIncludeIKJoints(),		cfgStream);
	fputc(GetIncludeNormals(),		cfgStream);
	fputc(GetIncludeTextureCoords(),cfgStream);
	fputc(GetIncludeVertexColors(),	cfgStream);
	fputc(GetIncludeObjGeom(),		cfgStream);
	fputc(GetIncludeObjShape(),		cfgStream);
	fputc(GetIncludeObjCamera(),	cfgStream);
	fputc(GetIncludeObjLight(),		cfgStream);
	fputc(GetIncludeObjHelper(),	cfgStream);
	fputc(GetWideFormat(),		cfgStream);
	fputc(GetStreaming(),		cfgStream);
	fputc(GetIncludeMaps(),		cfgStream);
	
	_putw(GetCompression(),			cfgStream);

	fclose(cfgStream);
}
