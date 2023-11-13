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

typedef enum 
{
    GLMDOP_START_CMDLIST,
    GLMDOP_END_CMDLIST,
    GLMDOP_VTX3F
} GLMDOP;

typedef union
{
    GLMDOP opv;
    float fv;
    unsigned int uiv;
    int iv;
} GLMDParam;

#define GLMDPARAM_VEC3(x, y, z) (GLMDParam){.fv = x}, (GLMDParam){.fv = y}, (GLMDParam){.fv = z} 


void* glmdGetFuncAddr(const char*);
void glmdCreateContext(void);
void glmdMakeContextCurrent(void);
void glmdInit(void);

void glmdAddVertex(float, float, float);
void glmdBeginVtxList(void);
void glmdEndVtxList(void);
void glmdDraw(void);
void glmdCmd(GLMDParam*, size_t size);





#endif //_GLMD_H