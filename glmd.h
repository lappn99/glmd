#ifndef _GLMD_H
#define _GLMD_H

#define GLMD_FUNC_HASH_CAPACITY 32

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
    GLMDOP_START_VTXLIST,
    GLMDOP_END_VTXLIST,
    GLMDOP_VTX3F,
    GLMDOP_FLUSH,
    GLMDOP_CREATE_CMDLIST,
    GLMDOP_START_CMDLIST,
    GLMDOP_END_CMDLIST,
    GLMDOP_EXEC_CMDLIST,
    GLMDOP_NOP
} GLMDOP;

typedef union
{
    GLMDOP opv;
    float fv;
    unsigned int uiv;
    int iv;
} GLMDParam;

typedef struct
{   
    unsigned int name;
    size_t size;
    GLMDParam* commands;
} GLMDCommandList;

#define GLMDPARAM_VEC3(x, y, z) (GLMDParam){.fv = x}, (GLMDParam){.fv = y}, (GLMDParam){.fv = z} 


void* glmdMalloc(size_t);
void* glmdRealloc(void* ,size_t);
void glmdFree(void*);

void* glmdGetFuncAddr(const char*);
void glmdCreateContext(void);
void glmdMakeContextCurrent(void);
void glmdInit(void);

void glmdAddVertex(float, float, float);
void glmdBeginVtxList(void);
void glmdEndVtxList(void);
void glmdDraw(void);
void glmdCreateCommandList(unsigned int);
void glmdStartCommandList(unsigned int);
void glmdEndCommandList(void);
void glmdExecuteCommandList(unsigned int);

void glmdCmd(GLMDParam*, size_t size);





#endif //_GLMD_H