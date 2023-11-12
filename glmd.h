#ifndef _GLMD_H
#define _GLMD_H

#define GLMD_FUNC_HASH_CAPACITY 16



void* glmdGetFuncAddr(const char*);

void glmdCreateContext(void);

void glmdMakeContextCurrent(void);

void glmdInit(void);



#endif //_GLMD_H