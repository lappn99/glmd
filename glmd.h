#ifndef _GLMD_H
#define _GLMD_H

#define GLMD_FUNC_HASH_CAPACITY 16

typedef struct
{
    union
    {
        struct
        {
            float x;
            float y;
            float z;
        };
        float position[3];   
    };

} Vertex3;

void* glmdGetFuncAddr(const char*);
void glmdCreateContext(void);
void glmdMakeContextCurrent(void);
void glmdInit(void);

void glmdAddVertex(float, float, float);
void glmdBeginVtxList(void);
void glmdEndVtxList(void);
void glmdDraw(void);





#endif //_GLMD_H