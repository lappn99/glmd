#ifndef _GLMDGL_H
#define _GLMDGL_H
#include <GL/glext.h>





typedef struct GL
{   
    
    PFNGLBINDBUFFERPROC                 glBindBuffer;
    PFNGLGENBUFFERSPROC                 glGenBuffers;
    PFNGLBUFFERDATAPROC                 glBufferData;
    PFNGLDRAWARRAYSEXTPROC              glDrawArrays;
    PFNGLDRAWARRAYSINDIRECTPROC         glDrawArraysIndirect;
    PFNGLENABLEVERTEXATTRIBARRAYPROC    glEnableVertexAttribArray;
    PFNGLVERTEXATTRIBPOINTERPROC        glVertexAttribPointer;
    PFNGLCREATESHADERPROC               glCreateShader;
    PFNGLDELETESHADERPROC               glDeleteShader;
    PFNGLLINKPROGRAMPROC                glLinkProgram;
    PFNGLUSEPROGRAMPROC                 glUseProgram;
    PFNGLATTACHSHADERPROC               glAttachShader;
    PFNGLSHADERSOURCEPROC               glShaderSource;
    PFNGLCOMPILESHADERPROC              glCompileShader;
    PFNGLCREATEPROGRAMPROC              glCreateProgram;
    PFNGLGETSHADERIVPROC                glGetShaderiv;
    PFNGLGETPROGRAMIVPROC               glGetProgramiv;
    PFNGLGETSTRINGIPROC                 glGetStringi;





    int initialized;

} GL;

typedef struct
{
    GLuint count;
    GLuint instanceCount;
    GLuint first;
    GLuint baseInstance;
} DrawArraysIndirectCommand;



void glmdInitGL(GL* gl);


#endif //_GLMDGL_H