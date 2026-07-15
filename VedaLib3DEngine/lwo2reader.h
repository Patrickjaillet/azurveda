/*! \file 
	\author victorien ferry
	\brief This file applies the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 , read file COPYING.
		independant lightwave object file format reader.	
				by krb (vic ferry), 02/11/2006, based on previous works.
				features:
					- read LWOB and LWO2 versions.
					- apply the right VMAP/VMAD vertex info for uv texture and rgb, for each polygon (LWO2).
					- manage creation of new vertexes for discontinuous mapping, the best way
					 (no extra creation of vertex for a same discontinuity).
				    - can give a triangle list with polygons sorted by surface, for GL rendering...
				restrictions:
					- all object polygons must be convex. n-gons with n>3 supported.
					- patch spline surface unsuported.
*/
#ifndef LWO2READER_H_
#define LWO2READER_H_
#ifdef __cplusplus
extern "C" {
#endif
/*!
	\brief	you must provide this allocator:
*/
extern void	*lw_CleanAlloc( unsigned int _bytesize );
/*!
	\brief	you must provide this desallocator:
*/
extern void	lw_FreeAlloc( void	*_pAllocToClose );

/*!
	\enum	lwo2_error
	\brief	init error enum.
*/
typedef	enum
{
	lwoerr_Ok=0,
	lwoerr_AllocFailed,
	lwoerr_FileProblem
} lwo2_error;


//! from LW envelope.h

#define SHAPE_TCB   0
#define SHAPE_HERM  1
#define SHAPE_BEZI  2
#define SHAPE_LINE  3
#define SHAPE_STEP  4
#define SHAPE_BEZ2  5

#define BEH_RESET      0
#define BEH_CONSTANT   1
#define BEH_REPEAT     2
#define BEH_OSCILLATE  3
#define BEH_OFFSET     4
#define BEH_LINEAR     5

typedef struct st_Key {
   struct st_Key *next;
   struct st_Key *prev;
   float  value;
   float  time;
   int    shape;
   float  tension;
   float  continuity;
   float  bias;
   float  param[ 4 ];
} Key;

typedef struct st_Envelope {
   Key   *key;
   int    nkeys;
   int    behavior[ 2 ];
} Envelope;

float evalEnvelope( Envelope *env, float time );

/*---------------------------------- LWOB-LWO2 structs  ---------------*/
struct  LwoMatrix{

    float   T1X;    /* pivot point (unused ?) */
    float   T1Y;
    float   T1Z;
};
/*!
	\struct sLwoVertex
	\brief Manages a Vertex read from a lwo file.
			as a position vector, a computed normal vector,
			Texture UV coordinates, etc
*/
typedef struct _LwoVertex{
	//! Vertex position vector
    float   XI,YI,ZI;
	//! vertex color (0.0f,1.0f)
    float   r,g,b;
	//! vertex computed normal vector
    float   XNI,YNI,ZNI;
	//! vertex UV texture coordinates.
    float   U,V;
	//! private use: trace VMAD discontinuity
    struct  LwoPolyToVertex     *FirstDiscontinuousUse;
} sLwoVertex ;
/*!
	\struct sLwoPolyToVertex
	\brief Describes the reference of a polygon to a given vertex
			in the vertex base. Informations can be discontinuous to the vertex,
			in which case they are here, or on the vertex, in which case they are in sLwoVertex.
*/
typedef struct _LwoPolyToVertex{
	//! Vertex number in vertex list
    int     V;
	//! Discontinuous UV texture coordinates, relative to the polygon.
    float   tU;   
	//! Discontinuous UV texture coordinates, relative to the polygon.
	float   tV;
	//! private use: 0 if normal ,used if >0, val -1 please
	int		DiscontinuousMappingOffset;
	//! private use.
    struct  LwoPolyToVertex     *NextDiscontinuousUse;
} sLwoPolyToVertex;
/*!
	\struct sLwoPolygon
	\brief Describes a polygon a general way, as a vertex reference table
			that describe a convex polygon.
*/
typedef struct  _LwoPolygon{
	//! number of vertex reference.
    unsigned int m_NumberOfVertex;
	//! reference table with length=nbV
    sLwoPolyToVertex *V;
	//! face normal
    float   XN,YN,ZN;
	//! texture used for this polygon.
    struct  LwTexture *surface;
} sLwoPolygon;
/*----------------------------------------*/
struct  LwoTriangle{
    //float   XN,YN,ZN;   /* face normal */
	unsigned int	v0;
	unsigned int	v1;
	unsigned int	v2;
};
/*----------------------------------------*/
struct  LwoSortedTriangleSurface{
	unsigned int	LwoFileSurfaceIndex;
	unsigned int	FirstTriangle;
	unsigned int	NumberOfTriangle;
};
/*----------------------------------------*/
struct  LwoMorphVertex{
        float   X,Y,Z;
};

struct MorphSet{
    struct MorphSet         *next;
    char                    name[88];
    struct  LwoMorphVertex  *vertex;
    int                     morf0spot1;
};
/*!
	\struct sLwoLayer
	\brief Describes an Object Layer, which means a given shape in the file.
			Then the geometry is deribed under it.
*/
typedef struct  _LwoLayer{
	//! file which manage this struct.
    struct  _LwoObject  *LWO_file;
	//! real number of layer in LW's interface.
	int     m_LayerNumber;
	//! vertex list, used by LWO_Polygon and LWO_triangle lists. 
    sLwoVertex  *LWO_Vertex;
	//! the n-gon polygons.
    sLwoPolygon    *LWO_Polygon;
	unsigned int      MaxNbVertex;
    unsigned int      MaxNbPolygon;
	unsigned int	m_ColorFlag;
	// valid if ReadLwoFlag_SortTriangleBySurfaces:
		//! total of tri in m_LWO_Triangle
		int     m_MaxNbTriangle;
		//! triangle version of LWO_Polygon. active if ReadLwoFlag_SortTriangleBySurfaces.
		struct  LwoTriangle    *m_LWO_Triangle;
		//! number of surface used in m_LWO_Triangle, and length of m_pSurfaceSortedTriangleListIndex.
		unsigned int	m_NumberOfSurfaceUsed;
		//! point which surface use which part of the triangle list.
		struct LwoSortedTriangleSurface	*m_pSurfaceSortedTriangleListIndex;

	int     NumberOfDiscontinousVertexToAdd;
 
    struct LwoMatrix objectPos; /* not a pointer */

    int     flags;      /* 16b direct from lwo */
    int     parent;   /* facultative parent layer (lwo) */

	//! cube coordinates where the object is bound:
    float       m_boundMin[3]; 
	//! cube coordinates where the object is bound:
    float       m_boundMax[3]; 

    char        *name;

    int             nbmorphtable;
    struct MorphSet *firstmorph;
    struct  LwoMorphVertex  *baseMorph; /* facultative.exist if at least 1 morphset "morf"   */

	struct RememberVMAPChunk	*m_pFirstVMAP;
	struct RememberVMAPChunk	*m_pFirstVMAD;

} sLwoLayer;
/*-----------------------------------*/
// for LwTexture.typebits
// for LwTexture.typebits
typedef enum
{
	TBit_MIPMap=1,
	TBit_ClampWidth=2,
	TBit_ClampHeight=4,
	TBit_NoSmooth=8,
	//TBit_Environment=16, now given by reflectimage.
} e_typebits;
    /*----- texture object -------*/
struct  LwTexture{
	unsigned int	IndexInFile;
    float            RGBAcolor[4];
    char            *name;       //used to bluit it.
    char            *txuv_used;	// valid VMAP/VMAD name for this texture layer
    char            *mapimage;      // NULL if no image, or clip name.
    char            *reflectimage;  // NULL if no image, or clip name.
    char            *bumpimage; // NULL if no image, or clip name.
    int             typebits; //what type of rendering ? enum e_typebits.
    float             additiveRate; //if additive layer.
    float            transparencyRate;
//re    struct  LwsMatrixValue  transparency;
};
/*-----------------------------------*/
struct  LwoClip{
    int                 LWClipindex;
    //struct  LwoClip     *nextInStaticChain; // we
    char                *imagename;

};
/*-----------------------------------*/
struct  LwoIndexedEnvelope{
    struct KTable *lwEnvlope;
    int         index;
};
/*!
	\struct sLwoObject
	\brief	Describe the whole lwo file To be used with ReadLwo()
*/
typedef struct _LwoObject {
	//!	
    char        *idname;
	//! Number of layers in m_LWO_Layer
    unsigned int         m_NumberOfLayers;
	//! table of layers. Beware, layers numbers are m_LWO_Layer->number
    sLwoLayer    *m_LWO_Layer;

    struct LwTexture   *surfaces;
    unsigned int         nbsurf;
    struct LwoIndexedEnvelope   *envlp;
	unsigned int         nbenvlp;
    unsigned int        nbclip;
//    int        *clipindextable; // used to link clipimages to textures.
    struct LwoClip *clipimagetable;

    /*------ LightWave LWO or LWO2 ? -------*/
    int     LWO2_true;        // != 0 if LWO2
	//! private use, link to binary while reading
    const unsigned char *m_pMainChunk;
	//! private use, main binary size
	unsigned int         m_mainsize;

} sLwoObject ;
/*----------------------------------*/
	//! flag bits for ReadLwo. Affect the way the objects are read.
	typedef enum
	{
		//! if present, the per-poly discontiuous texture coords are set back on new vertexes.
		//! this is needed if your engine only use vertex texture coord,( like usual hardware renderers.)
		ReadLwoFlag_DiscontinuousUVToNewVertex=1,
		//! Sort polygons by texture used in the polygon list.
		ReadLwoFlag_SortTriangleBySurfaces=2
	} ReadLwoFlag;

	/*!
		\brief	Read a File, open all layers in tables. CloseLwo() must be called at destruction and will close all allocs.
		\param	_pLwoObject	reference to an object that manage the LWO.
		\param	_pLwoFileChunk	pointer to the whole file chunk.
		\param	_lwoFileByteLength the length of _pLwoFileChunk in bytes.
		\param	_flags	look ReadLwoFlag enum up there. 
	*/
	lwo2_error	ReadLwo(sLwoObject *_pLwoObject, const unsigned char *_pLwoFileChunk, unsigned int _lwoFileByteLength, const unsigned int _flags );
	/*!
		\brief	Close all allocs from ReadLwo().
		\param	_pLwoObject	reference to an object that manage the LWO.
	*/
	void	CloseLwo(sLwoObject *_pLwoObject);

	/*!
		\brief	Close all allocs from ReadLwo().
		\param	_pLwoObjectlwo pointer to struct computed with ReadLwo.
		\param _layerIndex
		\return a sLwoLayer layer shape description.
	*/
	const sLwoLayer *LwoGetLayer(const sLwoObject *_pLwoObject, const unsigned int _layerIndex);

#ifdef __cplusplus
};
#endif
#endif
