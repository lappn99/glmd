#include <dlfcn.h> //dlsym, RTLD_NEXT
#include <GL/glx.h>
#include <stdio.h>
#include <stdlib.h> //malloc, realloc, free
#include <string.h> //strlen, memset
#include "glmd.h"
#include "glmdgl.h"

typedef struct
{
    size_t tableSize;
    void* funcArray[GLMD_FUNC_HASH_CAPACITY];
} FunctionAddrTable;

typedef struct 
{
    GLuint name;
    size_t size;
    size_t capacity;
} VertexBuffer;

typedef struct
{
    GL gl;
    VertexBuffer defaultVBO;
    FunctionAddrTable funcCache;
} GLMD;



int hashKeyExists(size_t);
void* hashGetValue(size_t);
void hashSetValue(size_t, void*);
size_t hashCalculate(const char*);

VertexBuffer createVertexBuffer(size_t);

static GLMD glmd;

void 
glmdInit(void)
{
    memset(glmd.funcCache.funcArray,0,sizeof(void*) * GLMD_FUNC_HASH_CAPACITY);
    glmd.gl.initialized = 0;
    
}

void
glmdCreateContext(void)
{
    glmdInit();
    fprintf(stderr,"GLMD Create context\n");
}

void 
glmdMakeContextCurrent(void)
{
    if(!glmd.gl.initialized)
    {
        fprintf(stderr,"GLMD: Initializing GLMD OpenGL\n");
        glmdInitGL(&glmd.gl);
        glmd.defaultVBO = createVertexBuffer(3);
        glmd.gl.initialized = 1;
    }

}

void*
glmdGetFuncAddr(const char* name)
{
    void* func;
    size_t hash = hashCalculate(name);
    if(!hashKeyExists(hash))
    {
        func = dlsym(RTLD_NEXT,name);
        fprintf(stderr,"GLMD: Caching %s at address %p\n",name,func);
        hashSetValue(hash,func);
    }
    else
    {
        func = hashGetValue(hash);
    }
    return func;
}

void
glmdAddVertex(float x, float y, float z)
{

}

int 
hashKeyExists(size_t hash)
{
    size_t index = hash % GLMD_FUNC_HASH_CAPACITY;
    if(glmd.funcCache.funcArray[index] == NULL)
    {
        return 0;
    }
    return 1;
}

void* 
hashGetValue(size_t hash)
{
    size_t index = hash % GLMD_FUNC_HASH_CAPACITY;
    return glmd.funcCache.funcArray[index];

}

void 
hashSetValue(size_t hash, void* value)
{

    size_t index = hash % GLMD_FUNC_HASH_CAPACITY;
    glmd.funcCache.funcArray[index] = value;
}

//https://stackoverflow.com/questions/7666509/hash-function-for-string
size_t 
hashCalculate(const char* key)
{
    size_t hash = 0;
    int i;
    for(i = 0; i < strlen(key);i++)
    {
        hash = ((hash << 5) + hash) + key[i];
    }
    return hash;
}


VertexBuffer
createVertexBuffer(size_t initialCapacity)
{
    VertexBuffer vb;
    glmd.gl.glGenBuffers(1,&vb.name);
    glmd.gl.glBindBuffer(GL_ARRAY_BUFFER,vb.name);
    glmd.gl.glBufferData(GL_ARRAY_BUFFER,initialCapacity,NULL,GL_DYNAMIC_DRAW);
    vb.capacity = initialCapacity;
    vb.size = 0;
    return vb;

}
