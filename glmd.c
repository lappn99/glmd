#include <dlfcn.h> //dlsym, RTLD_NEXT
#include <errno.h>
#include <GL/glx.h>
#include <stdio.h>
#include <stdlib.h> //malloc, realloc, free
#include <string.h> //strlen, memset
#include "glmd.h"
#include "glmdgl.h"

#ifndef GLMD_RES_HEADER
#define GLMD_RES_HEADER "glmdresources.h"
#include GLMD_RES_HEADER
#endif

const uint TRIANGLE_ELEMENT_SIZE = 3;
const uint QUAD_ELEMENT_SIZE = 4;



//Hash table to cache addresses to intercepted functions
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
    uint primitiveSize;
    Vertex3* vertices;
    
} VertexBuffer;

//GLMD State
typedef struct
{
    int immediateExecution;
    GL gl; //Loaded OpenGL Functions
    VertexBuffer defaultVBO; //Default VBO when not overwritten (glCallLists?)
    GLuint defaultProgram; //Used when no other lighting model specified
    FunctionAddrTable funcCache; //Intercepted function hash table
    size_t numCommandLists;
    GLMDCommandList* commandLists;
    GLMDCommandList* currentCommandList;
    
} GLMD;


//Private/Implementation specific functions


int hashKeyExists(size_t);
void* hashGetValue(size_t);
void hashSetValue(size_t, void*);
size_t hashCalculate(const char*);
size_t executeCmd(GLMDParam*);
void addCommandList(GLMDCommandList);



VertexBuffer createVertexBuffer(size_t);

static GLMD glmd;

void*
glmdMalloc(size_t size)
{
    void* ptr = malloc(size);
    if(ptr == NULL)
    {
        perror("malloc\n");
        exit(1);
    }
    return ptr;
}

void*
glmdRealloc(void* ptr, size_t newSize)
{
    void* newPtr = realloc(ptr,newSize);
    if(newPtr == NULL && errno != 0)
    {
        perror("realloc\n");
        exit(1);
    }
    return newPtr;
}

void 
glmdFree(void* ptr)
{
    free(ptr);
}

void 
glmdInit(void)
{
    //Set func hash table to point to NULL by default
    memset(glmd.funcCache.funcArray,0,sizeof(void*) * GLMD_FUNC_HASH_CAPACITY);
    glmd.gl.initialized = 0;
    glmd.immediateExecution = 1;
    
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
    //Cant actually call OpenGL functions until the context is bound
    //But this can be called even for just moving the window
    //So only call this OpenGL init stuff once!
    if(!glmd.gl.initialized)
    {
        fprintf(stderr,"GLMD: Initializing GLMD OpenGL\n");
        glmdInitGL(&glmd.gl);
        glmd.defaultVBO = createVertexBuffer(3);
        glmd.gl.initialized = 1;

        //Creating program/shaders
        GLuint vertexShader,fragmentShader;
        int success;
        char infoLog[512];
        (void)infoLog;
        vertexShader = glmd.gl.glCreateShader(GL_VERTEX_SHADER);
        glmd.gl.glShaderSource(vertexShader,1,&GLMD_DEFAULT_VERTEX_SHADER,NULL);
        glmd.gl.glCompileShader(vertexShader);
        glmd.gl.glGetShaderiv(vertexShader,GL_COMPILE_STATUS,&success);

        if(!success)
        {
            fprintf(stderr,"Could not compile vertex shader\n");
            exit(1);
        }


        fragmentShader = glmd.gl.glCreateShader(GL_FRAGMENT_SHADER);
        glmd.gl.glShaderSource(fragmentShader,1,&GLMD_DEFAULT_FRAGMENT_SHADER,NULL);
        glmd.gl.glCompileShader(fragmentShader);
        glmd.gl.glGetShaderiv(fragmentShader,GL_COMPILE_STATUS,&success);

        if(!success)
        {
            fprintf(stderr,"Could not compile fragment shader\n");
            exit(1);
        }

        glmd.defaultProgram = glmd.gl.glCreateProgram();
        glmd.gl.glAttachShader(glmd.defaultProgram,vertexShader);
        glmd.gl.glAttachShader(glmd.defaultProgram,fragmentShader);

        glmd.gl.glLinkProgram(glmd.defaultProgram);
        glmd.gl.glGetProgramiv(glmd.defaultProgram,GL_LINK_STATUS,&success);

        if(!success)
        {
            fprintf(stderr,"Could not link program\n");
        }
        glmd.gl.glUseProgram(glmd.defaultProgram);

        glmd.gl.glDeleteShader(vertexShader);
        glmd.gl.glDeleteShader(fragmentShader);
    }

}

//Get address of intercepted function
//assumes that NAME is an intercepted function
//TODO: Behave more generally 
void*
glmdGetFuncAddr(const char* name)
{
    void* func;
    size_t hash = hashCalculate(name);
    if(!hashKeyExists(hash))
    {
        //Get intercepted function address and store in hash table
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

//Add vertex to current polygon/vertex list
void
glmdAddVertex(float x, float y, float z)
{   
    if(glmd.defaultVBO.size >= glmd.defaultVBO.capacity)
    {
        //If no size, realloc to the next multiple of the amount of vertices needed for primitive (Triangle)
        //TODO: Implement other primitives
        size_t remainder = (glmd.defaultVBO.capacity + 1) % glmd.defaultVBO.primitiveSize;
        size_t newSize = (glmd.defaultVBO.capacity + 1 + glmd.defaultVBO.primitiveSize - remainder) * sizeof(Vertex3);
        glmd.defaultVBO.vertices = glmdRealloc(glmd.defaultVBO.vertices, newSize );
        glmd.defaultVBO.capacity = newSize;
    }

    glmd.defaultVBO.vertices[glmd.defaultVBO.size++] = (Vertex3){.x = x, .y = y, .z = z};

}   

//Start vertex sequence
void 
glmdBeginVtxList(void)
{
    glmd.gl.glBindBuffer(GL_ARRAY_BUFFER,glmd.defaultVBO.name);
    //Zero out buffer memory
    glmd.gl.glBufferData(GL_ARRAY_BUFFER,0,NULL,GL_DYNAMIC_DRAW);
    glmd.gl.glBindBuffer(GL_ARRAY_BUFFER,0);
}

//End vertex sequence
void 
glmdEndVtxList(void)
{
    glmd.gl.glBindBuffer(GL_ARRAY_BUFFER,glmd.defaultVBO.name);

    //Send data to vertex buffer
    glmd.gl.glBufferData(GL_ARRAY_BUFFER,
        sizeof(Vertex3) * glmd.defaultVBO.size,glmd.defaultVBO.vertices,GL_DYNAMIC_DRAW);

    glmd.gl.glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3 * sizeof(float),(void*)0);
    glmd.gl.glEnableVertexAttribArray(0);

    
    glmd.gl.glBindBuffer(GL_ARRAY_BUFFER,0);
}

//Draw to screen
void
glmdDraw(void)
{
    glmd.gl.glBindBuffer(GL_ARRAY_BUFFER,glmd.defaultVBO.name);
    glmd.gl.glUseProgram(glmd.defaultProgram);
    glmd.gl.glDrawArrays(GL_TRIANGLES,0,glmd.defaultVBO.size);
    glmd.gl.glBindBuffer(GL_ARRAY_BUFFER,0);
    glmd.defaultVBO.size = 0;
    
}

void
glmdCmd(GLMDParam* params, size_t size)
{

    if(glmd.immediateExecution || params[0].opv == GLMDOP_END_CMDLIST)
    {
        executeCmd(params);
    }
    else
    {
        if(glmd.currentCommandList == NULL)
        {
            fprintf(stderr,"GLMD: No command list bound");
            exit(1);
        }
        GLMDCommandList* list = glmd.currentCommandList;
        if(list->size == 0)
        {
           list->commands = glmdMalloc(sizeof(GLMDParam) * size );

        }
        else
        {
            list->commands = glmdRealloc(list->commands,sizeof(GLMDParam) * (size + list->size) );
            
        }        
        memcpy(&(list->commands[list->size]),params,sizeof(GLMDParam) * size);
        list->size += size;
        


    }
}

void
glmdCreateCommandList(unsigned int name)
{
    GLMDCommandList list;
    list.name = name;
    list.size = 0;
    addCommandList(list);

    
}

void
glmdStartCommandList(GLuint name)
{
    int i;
    
    for(i = 0; i < glmd.numCommandLists; i++)
    {
        if(glmd.commandLists[i].name == name)
        {
            glmd.currentCommandList = &(glmd.commandLists[i]);
            glmd.immediateExecution = 0;
        }
    }

}
void 
glmdEndCommandList(void)
{
    glmd.currentCommandList = NULL;
    glmd.immediateExecution = 1;
}

void 
glmdExecuteCommandList(unsigned int name)
{
    GLMDCommandList* list;
    int i;
    for(i = 0; i < glmd.numCommandLists; i++)
    {
        if(glmd.commandLists[i].name == name)
        {
            list = &(glmd.commandLists[i]);
            
        }
    }
    if(list == NULL)
    {
        fprintf(stderr,"GLMD: List not found: %u", name);
        exit(1);
    }
    GLMDParam* currentParam;
    currentParam = &list->commands[0];
    for(size_t i = 0; i < list->size;i+=executeCmd(currentParam))
    {
        currentParam = &(list->commands[i]);
    }
   
    
    
}

size_t 
executeCmd(GLMDParam* params)
{

    if(params == NULL)
    {
        return 0;
    }

    GLMDOP op = params[0].opv;
    size_t cmdSize = 1;
    switch(op)
    {
        case GLMDOP_START_VTXLIST:
            glmdBeginVtxList();
            break;
        case GLMDOP_END_VTXLIST:
            glmdEndVtxList();
            break;
        case GLMDOP_VTX3F:
            glmdAddVertex(params[1].fv,params[2].fv,params[3].fv);
            cmdSize += 3;
            break;
        case GLMDOP_FLUSH:
            glmdDraw();
            break;
        case GLMDOP_CREATE_CMDLIST:
            glmdCreateCommandList(params[1].uiv);
            cmdSize += 1;
            break;
        case GLMDOP_START_CMDLIST:
            glmdStartCommandList(params[1].uiv);
            cmdSize += 1;
            break;
        case GLMDOP_END_CMDLIST:
            glmdEndCommandList();
            cmdSize += 1;
            break;
        case GLMDOP_EXEC_CMDLIST:
            glmdExecuteCommandList(params[1].uiv);
            cmdSize += 1;
            break;
        case GLMDOP_NOP: break;
        
    }
    return cmdSize;


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
    vb.vertices = glmdMalloc(initialCapacity * sizeof(Vertex3));
    vb.capacity = initialCapacity;
    vb.primitiveSize = TRIANGLE_ELEMENT_SIZE;
    vb.size = 0;
    return vb;

}

void
addCommandList(GLMDCommandList list)
{
    if(glmd.numCommandLists == 0)
    {
        glmd.commandLists = glmdMalloc(sizeof(GLMDCommandList));
    }
    glmd.commandLists[glmd.numCommandLists++] = list;
}
