#include <types.h>
#include <math.h>
#include <gte.h>
#include <gpu.h>
#include <extern.h>
#include <kernel.h>

/*
    @brief: Multiplies each color component to an inverse darkness level factor
    @address: 0x8002a2ec
    @params:
        @color
    @return: color adjusted by the global variable darknessLevel
*/
static ColorVec AdjustColorLightLevel(ColorVec color)
{
    if (_darknessLevel)
    {
        u32 lightLevel = PSYQ_FP_ONE - _darknessLevel;
        color.r = PSYQ_FP_MULT(color.r, lightLevel);
        color.g = PSYQ_FP_MULT(color.g, lightLevel);
        color.b = PSYQ_FP_MULT(color.b, lightLevel);
    }
    return color;
}

/*
    @brief: Add a Gouraud quad into the ordering table
    @address: 0x8001a0d8
    @params:
        @quad
        @polyFlags
    @return: pointer to the PSX primitive
*/
PolyG4 * AddPolyG4(ColoredRect * quad, u32 polyFlags)
{
    // TODO: figure out polyFlags
    if (!(polyFlags & 0x8000)) { return nullptr; }
    PolyG4 * pPrim;
    GetPrimMem(pPrim, PolyG4);
    for (s32 i = 0; i < QUAD_SIZE; i++) { pPrim->v[i].color = AdjustColorLightLevel(quad->color[i]); }
    pPrim->texpage = (Texpage) {.code = 0xE1, .dither = 1};
    pPrim->gPolyCode = (PolyCode) {.quad = 1, .gouraud = 1, .renderCode = 1};
    if (polyFlags & 0x21) // TODO: figure out polyFlags
    {
        pPrim->texpage.self |= polyFlags & 0x60; // TODO: figure out polyFlags
        pPrim->gPolyCode.semiTransparency = 1;
    }
    pPrim->nop = 0;

    u32 zIndex = 0;
    if (polyFlags & 0x10000000) // TODO: figure out polyFlags
    {
        s32 width = _pFrameBuffer->w;
        for (s32 i = 0; i < QUAD_SIZE; i++)
        {
            pPrim->v[i].pos.x = (width * (_g_xOffset + quad->pos[i].x)) / 640;
            pPrim->v[i].pos.y = (_g_yOffset + quad->pos[i].y) / 2;
        }
    }
    else
    {
        gte_ldv3c(&quad->pos[0]);
        gte_rtpt();
        gte_avsz3();
        gte_stotz(&zIndex);
        gte_stsxy0(&pPrim->v[0].pos);
        gte_stsxy1(&pPrim->v[1].pos);
        gte_stsxy2(&pPrim->v[2].pos);
        gte_ldv0(&quad->pos[3]);
        gte_rtps();
        gte_stsxy(&pPrim->v[3].pos);
    }
    zIndex = (zIndex + _zIndexOffset) / 2;
    if (zIndex < _maxZIndex) { AddPrim(pPrim, zIndex); }
    return pPrim;
}