// SPDX-License-Identifier: LGPL-2.1-only

#ifndef LWO2READER_H_
#define LWO2READER_H_
#ifdef __cplusplus
extern "C" {
#endif

extern void	*lw_CleanAlloc( unsigned int _bytesize );

extern void	lw_FreeAlloc( void	*_pAllocToClose );

typedef	enum
{
	lwoerr_Ok=0,
	lwoerr_AllocFailed,
	lwoerr_FileProblem
} lwo2_error;

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

struct  LwoMatrix{

    float   T1X;
    float   T1Y;
    float   T1Z;
};

typedef struct _LwoVertex{

    float   XI,YI,ZI;

    float   r,g,b;

    float   XNI,YNI,ZNI;

    float   U,V;

    struct  _LwoPolyToVertex     *FirstDiscontinuousUse;
} sLwoVertex ;

typedef struct _LwoPolyToVertex{

    int     V;

    float   tU;

	float   tV;

	int		DiscontinuousMappingOffset;

    struct  _LwoPolyToVertex     *NextDiscontinuousUse;
} sLwoPolyToVertex;

typedef struct  _LwoPolygon{

    unsigned int m_NumberOfVertex;

    sLwoPolyToVertex *V;

    float   XN,YN,ZN;

    struct  LwTexture *surface;
} sLwoPolygon;

struct  LwoTriangle{

	unsigned int	v0;
	unsigned int	v1;
	unsigned int	v2;
};

struct  LwoSortedTriangleSurface{
	unsigned int	LwoFileSurfaceIndex;
	unsigned int	FirstTriangle;
	unsigned int	NumberOfTriangle;
};

struct  LwoMorphVertex{
        float   X,Y,Z;
};

struct MorphSet{
    struct MorphSet         *next;
    char                    name[88];
    struct  LwoMorphVertex  *vertex;
    int                     morf0spot1;
};

typedef struct  _LwoLayer{

    struct  _LwoObject  *LWO_file;

	int     m_LayerNumber;

    sLwoVertex  *LWO_Vertex;

    sLwoPolygon    *LWO_Polygon;
	unsigned int      MaxNbVertex;
    unsigned int      MaxNbPolygon;
	unsigned int	m_ColorFlag;

		int     m_MaxNbTriangle;

		struct  LwoTriangle    *m_LWO_Triangle;

		unsigned int	m_NumberOfSurfaceUsed;

		struct LwoSortedTriangleSurface	*m_pSurfaceSortedTriangleListIndex;

	int     NumberOfDiscontinousVertexToAdd;

    struct LwoMatrix objectPos;

    int     flags;
    int     parent;

    float       m_boundMin[3];

    float       m_boundMax[3];

    char        *name;

    int             nbmorphtable;
    struct MorphSet *firstmorph;
    struct  LwoMorphVertex  *baseMorph;

	struct RememberVMAPChunk	*m_pFirstVMAP;
	struct RememberVMAPChunk	*m_pFirstVMAD;

} sLwoLayer;

typedef enum
{
	TBit_MIPMap=1,
	TBit_ClampWidth=2,
	TBit_ClampHeight=4,
	TBit_NoSmooth=8,

} e_typebits;

struct  LwTexture{
	unsigned int	IndexInFile;
    float            RGBAcolor[4];
    char            *name;
    char            *txuv_used;
    char            *mapimage;
    char            *reflectimage;
    char            *bumpimage;
    int             typebits;
    float             additiveRate;
    float            transparencyRate;

};

struct  LwoClip{
    int                 LWClipindex;

    char                *imagename;

};

struct  LwoIndexedEnvelope{
    struct KTable *lwEnvlope;
    int         index;
};

typedef struct _LwoObject {

    char        *idname;

    unsigned int         m_NumberOfLayers;

    sLwoLayer    *m_LWO_Layer;

    struct LwTexture   *surfaces;
    unsigned int         nbsurf;
    struct LwoIndexedEnvelope   *envlp;
	unsigned int         nbenvlp;
    unsigned int        nbclip;

    struct LwoClip *clipimagetable;

    int     LWO2_true;

    const unsigned char *m_pMainChunk;

	unsigned int         m_mainsize;

} sLwoObject ;

	typedef enum
	{

		ReadLwoFlag_DiscontinuousUVToNewVertex=1,

		ReadLwoFlag_SortTriangleBySurfaces=2
	} ReadLwoFlag;

	lwo2_error	ReadLwo(sLwoObject *_pLwoObject, const unsigned char *_pLwoFileChunk, unsigned int _lwoFileByteLength, const unsigned int _flags );

	void	CloseLwo(sLwoObject *_pLwoObject);

	const sLwoLayer *LwoGetLayer(const sLwoObject *_pLwoObject, const unsigned int _layerIndex);

#ifdef __cplusplus
};
#endif
#endif
