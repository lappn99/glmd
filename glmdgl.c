#include <dlfcn.h> //dlopen, dlsym
#include <stdio.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include "glmdgl.h"

#define GLMD_GL_REG_FUNC(FUNC) #FUNC,

const char* GL_FUNC_LIST[] = {
    GLMD_GL_REG_FUNC(glBindBuffer)
    GLMD_GL_REG_FUNC(glGenBuffers)
    GLMD_GL_REG_FUNC(glBufferData)
    GLMD_GL_REG_FUNC(glDrawArrays)
    GLMD_GL_REG_FUNC(glEnableVertexAttribArray)
    GLMD_GL_REG_FUNC(glVertexAttribPointer)
    GLMD_GL_REG_FUNC(glCreateShader)
    GLMD_GL_REG_FUNC(glLinkProgram)
    GLMD_GL_REG_FUNC(glUseProgram)
    GLMD_GL_REG_FUNC(glAttachShader)
};

#define NUM_GL_FUNCS (sizeof(GL_FUNC_LIST) / sizeof(char*))



void
glmdInitGL(GL* gl)
{

    
    if(gl->initialized)
    {
        return;
    }
    //fprintf(stderr,"Registering %lu OpenGL Functions", NUM_GL_FUNCS);
    void* libgl = dlopen("libGL.so",RTLD_LAZY);

    #define REGISTER_GL_FUNC( FUNC ) (gl->FUNC = dlsym(libgl, #FUNC ))

    REGISTER_GL_FUNC(glBindBuffer);
    REGISTER_GL_FUNC(glGenBuffers);
    REGISTER_GL_FUNC(glBufferData);
    REGISTER_GL_FUNC(glDrawArrays);
    REGISTER_GL_FUNC(glEnableVertexAttribArray);
    REGISTER_GL_FUNC(glVertexAttribPointer);
    REGISTER_GL_FUNC(glVertexAttribPointer);
    REGISTER_GL_FUNC(glCreateShader);
    REGISTER_GL_FUNC(glLinkProgram);
    REGISTER_GL_FUNC(glUseProgram);
    REGISTER_GL_FUNC(glAttachShader);
    REGISTER_GL_FUNC(glShaderSource);
    REGISTER_GL_FUNC(glCreateProgram);
    REGISTER_GL_FUNC(glDeleteShader);
    REGISTER_GL_FUNC(glCompileShader);
    REGISTER_GL_FUNC(glGetShaderiv);
    REGISTER_GL_FUNC(glGetProgramiv);
    REGISTER_GL_FUNC(glGetStringi);

    GLint numExtensions = 0;
    glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);

    
    

}

