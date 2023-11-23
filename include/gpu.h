#ifndef GPU_H
#define GPU_H

#include <types.h>
#include <math.h>

/* Ordering Table */

#define OT_SIZE 4096

typedef struct PrimitiveMemory
{
    void * pStartAddr;
    void * pEndAddr;
    void * pCurrAddr;
} PrimitiveMemory;

typedef union Tag
{
    struct
    {
        u32 addr : 24;
        u32 size : 8;
    };
    u32 self;
} Tag;

typedef struct OrderingTable
{
    Tag ot[OT_SIZE];
    PrimitiveMemory primMem;
} OrderingTable;

/* Primitives */

#define TRI_SIZE  3
#define QUAD_SIZE 4

#define GetPrimMem(pPrim, PrimType) \
    pPrim = (PrimType *) _pOT->primMem.pCurrAddr; \
    _pOT->primMem.pCurrAddr += sizeof(PrimType); \
    pPrim->tag.size = (sizeof(PrimType) - sizeof(Tag)) >> 2

#define AddPrim(pPrim, zIndex) \
    pPrim->tag.addr = _pOTCopy->ot[zIndex].addr; \
    _pOTCopy->ot[zIndex].addr = (u32) pPrim

#define fPolyCode color.code
#define gPolyCode v[0].color.code
#define polyClut v[0].clut
#define polyTpage v[1].tpage

typedef union Texpage
{
    struct
    {
        u32 x                : 4; /* x * 64 */
        u32 y                : 1; /* y * 256 */
        u32 semiTransparency : 2; /* (0=B/2+F/2, 1=B+F, 2=B-F, 3=B+F/4) */
        u32 texpageColors    : 2; /* (0=4bit, 1=8bit, 2=15bit, 3=Reserved) */
        u32 dither           : 1; /* (0=Off/strip LSBs, 1=24bit to 15bit Dither Enabled) */
        u32 drawDisplayArea  : 1; /* bool */
        u32 y_VRAM_EXP       : 1; /* ununsed in retail */
        u32 texFlipX         : 1; /* bool */
        u32 texFlipY         : 1; /* bool */
        u32 unused           : 10;
        u32 code             : 8; /* 0xE1 */
    };
    u32 self;
} Texpage;

typedef union PolyCode
{
    struct
    {
        u8 rawTex           : 1; /* raw texture / modulation */
        u8 semiTransparency : 1; /* semi-transparent / opaque */
        u8 textured         : 1; /* textured / untextured */
        u8 quad             : 1; /* 4 / 3 vertices */
        u8 gouraud          : 1; /* gouraud / flat shading */
        u8 renderCode       : 3; /* 0b001 = Polygon */
    };
    u8 code;
} PolyCode;

typedef union ColorVec
{
    struct
    {
        u8 r;
        u8 g;
        u8 b;
        PolyCode code;
    };
    u32 self;
} ColorVec;

typedef union Point
{
    struct
    {
        s16 x;
        s16 y;
    };
    s32 self;
} Point;

typedef union UV
{
    struct
    {
        u8 u;
        u8 v;
    };
    u16 self;
} UV;

typedef union PolyTexpage
{
    struct
    {
        u16 x                : 4; /* x * 64 */
        u16 y                : 1; /* y * 256 */
        u16 semiTransparency : 2; /* (0=B/2+F/2, 1=B+F, 2=B-F, 3=B+F/4) */
        u16 texpageColors    : 2; /* (0=4bit, 1=8bit, 2=15bit, 3=Reserved) */
        u16 unused           : 2;
        u16 y_VRAM_EXP       : 1; /* ununsed in retail */
        u16 unused2          : 2;
        u16 nop              : 2;
    };
    u16 self;
} PolyTexpage;

typedef union CLUT
{
    struct
    {
        u16 x   : 6; /* X/16  (ie. in 16-halfword steps) */
        u16 y   : 9; /* 0-511 (ie. in 1-line steps) */
        u16 nop : 1; /* Should be 0 */
    };
    u16 self;
} CLUT;

/* Vertex types */

typedef struct GVertex
{
    ColorVec color;
    Point pos;
} GVertex;

typedef struct FTVertex
{
    Point pos;
    UV texCoords;
    union
    {
        CLUT clut;
        PolyTexpage tpage;
    };
} FTVertex;

typedef struct GTVertex
{
    ColorVec color;
    Point pos;
    UV texCoords;
    union
    {
        CLUT clut;
        PolyTexpage tpage;
    };
} GTVertex;

/* Primitive types */

typedef struct PolyG3
{
    Tag tag;
    Texpage texpage;
    u32 nop; /* Should always be zero */
    GVertex v[TRI_SIZE];
} PolyG3;

typedef struct PolyG4
{
    Tag tag;
    Texpage texpage;
    u32 nop; /* Should always be zero */
    GVertex v[QUAD_SIZE];
} PolyG4;

typedef struct PolyGT3
{
    Tag tag;
    GTVertex v[TRI_SIZE];
} PolyGT3;

typedef struct PolyGT4
{
    Tag tag;
    GTVertex v[QUAD_SIZE];
} PolyGT4;

typedef struct PolyFT3
{
    Tag tag;
    ColorVec color;
    FTVertex v[TRI_SIZE];
} PolyFT3;

typedef struct PolyFT4
{
    Tag tag;
    ColorVec color;
    FTVertex v[QUAD_SIZE];
} PolyFT4;

/* Meshes */
typedef struct Quad
{
    u8 field0_0x0[8];
    u16 width;
    u16 height;
    u8 field3_0xc[4];
    u8 widthRelated;
    u8 field5_0x11[19];
    PolyTexpage tpage;
    CLUT clut;
    UV texCoords[QUAD_SIZE];
    u8 field9_0x30[8];
} Quad;

typedef struct ColoredRect
{
    SVec3Pad pos[QUAD_SIZE];
    ColorVec color[QUAD_SIZE];
} ColoredRect;

#endif