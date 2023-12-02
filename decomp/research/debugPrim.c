#include <types.h>
#include <math.h>
#include <gpu.h>
#include <model.h>
#include <kernel.h>

#define CRASHBALL_P1_ENCODE_LIST_ADDR ((VertexEncoding *) 0x800dc024)

#define PRINT_VERTEX(i) \
    k++; \
    LOG("Vertex: %d\n", k); \
    LOG("Color: %x %x %x\n", pPrim->v[i].color.r, pPrim->v[i].color.g, pPrim->v[i].color.b); \
    LOG("UV: %x %x\n", pPrim->v[i].texCoords.u, pPrim->v[i].texCoords.v) \

void DebugPrim(PolyGT3 * pPrim, SVec3Pad * vertexList, VertexEncoding * encodeList)
{
    if (encodeList !=  CRASHBALL_P1_ENCODE_LIST_ADDR) return;

    u32 k = 0;
    while (encodeList->vertexCount != 0xFF)
    {
        LOG("New group: %d\n", encodeList->vertexCount);
        LOG("CLUT: %x\n", pPrim->polyClut);
        LOG("TexPage: %x\n\n", pPrim->polyTpage);
        PRINT_VERTEX(0);
        PRINT_VERTEX(1);
        for (s32 j = 0; j < encodeList->vertexCount; j++)
        {
            PRINT_VERTEX(2);
            pPrim++;
        }
        encodeList++;
    }
}