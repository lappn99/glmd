#include <dlfcn.h> //dlsym, RTLD_NEXT
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
} GLMD;


//Private/Implementation specific functions


int hashKeyExists(size_t);
void* hashGetValue(size_t);
void hashSetValue(size_t, void*);
size_t hashCalculate(const char*);
void executeCmd(GLMDParam* params, size_t size);

VertexBuffer createVertexBuffer(size_t);

static GLMD glmd;


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
        size_t remainder = (glmd.defaultVBO.capacity + 1) % TRIANGLE_ELEMENT_SIZE;
        size_t newSize = (glmd.defaultVBO.capacity + 1 + TRIANGLE_ELEMENT_SIZE - remainder) * sizeof(Vertex3);
        glmd.defaultVBO.vertices = realloc(glmd.defaultVBO.vertices, newSize * sizeof(Vertex3));
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

    glmd.defaultVBO.size = 0;
    glmd.gl.glBindBuffer(GL_ARRAY_BUFFER,0);
}

//Draw to screen
void
glmdDraw(void)
{
    glmd.gl.glBindBuffer(GL_ARRAY_BUFFER,glmd.defaultVBO.name);
    glmd.gl.glUseProgram(glmd.defaultProgram);
    glmd.gl.glDrawArrays(GL_TRIANGLES,0,3);
    glmd.gl.glBindBuffer(GL_ARRAY_BUFFER,0);
    
}

void
glmdCmd(GLMDParam* params, size_t size)
{

    if(glmd.immediateExecution)
    {
        executeCmd(params,size);
    }
    else
    {
        exit(1);
    }
}

void 
executeCmd(GLMDParam* params, size_t size)
{
    //NOOP
    if(size == 0)
    {
        return;
    }

    GLMDOP op = params[0].opv;
    switch(op)
    {
        case GLMDOP_START_CMDLIST:
            glmdBeginVtxList();
            break;
        case GLMDOP_END_CMDLIST:
            glmdEndVtxList();
            break;
        case GLMDOP_VTX3F:
            glmdAddVertex(params[1].fv,params[2].fv,params[3].fv);
            break;
        
    }


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
    vb.vertices = malloc(initialCapacity * sizeof(Vertex3));
    vb.capacity = initialCapacity;
    vb.size = 0;
    return vb;

}
