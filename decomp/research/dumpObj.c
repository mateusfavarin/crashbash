#include <types.h>
#include <math.h>
#include <model.h>
#include <kernel.h>

#define ADD_VERTEX() \
    arr[i].x = vertexList[i].x; \
    arr[i].y = vertexList[i].y; \
    arr[i].z = vertexList[i].z; \
    LOG("v %d %d %d\n", arr[i].x, arr[i].y, arr[i].z); \
    i++

static Vec3 * const arr = (Vec3 * const) 0x80500000;

void UnpackVerticeList(u32 * pPrim, SVec3Pad * vertexList, VertexEncoding * encodeList)
{
    s32 triStart = 0;
    s32 i = 0;
    LOG("LISTS: %x %x\n", vertexList, encodeList);
    while (encodeList->vertexCount != 0xFF)
    {
        ADD_VERTEX();
        ADD_VERTEX();
        for (s32 j = 0; j < encodeList->vertexCount; j++)
        {
            ADD_VERTEX();
            LOG("f %d %d %d\n", triStart + 1, triStart + 2, triStart + 3);
            triStart++;
        }
        triStart = i;
        encodeList++;
    }
}