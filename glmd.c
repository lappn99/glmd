#include <dlfcn.h> //dlsym, RTLD_NEXT
#include <GL/glx.h>
#include <stdio.h>
#include <stdlib.h> //malloc, realloc, free
#include <string.h> //strlen, memset
#include "glmd.h"
#include "glmdgl.h"

typedef struct FunctionAddrTable
{
    size_t tableSize;
    void* funcArray[GLMD_FUNC_HASH_CAPACITY];
} FunctionAddrTable;

typedef struct GLMD
{
    GL gl;

    GLuint defaultVBO;
    FunctionAddrTable funcCache;
} GLMD;


const size_t FUNC_HASH_CAPACITY = 16;

int hashKeyExists(size_t);
void* hashGetValue(size_t);
void hashSetValue(size_t, void*);
size_t hashCalculate(const char*);

static GLMD glmd;

void 
glmdInit(void)
{
    memset(glmd.funcCache.funcArray,0,sizeof(void*) * FUNC_HASH_CAPACITY);
    glmdInitGL(&glmd.gl);
    glmd.gl.glGenBuffers(1,&glmd.defaultVBO);
    fprintf(stderr,"GLMD: Created Default VBO: %d\n",glmd.defaultVBO);
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
    fprintf(stderr,"GLMD Make Context current\n");
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

