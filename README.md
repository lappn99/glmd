# GL M.D

## Description
This is an experiment to create a 'Transition Layer' of sorts that translates OpenGL Immediate Mode/Legacy API calls into modern API calls. Hopefully this will allow three things:

1. Optimize programs that use Immediate Mode/Legacy API.
2. Pair the simplicity of the Immediate Mode/Legacy API with the customization and performance of the modern API.
3. Have fun.

**This is not finished yet**

At the very least I would like to get most functions found in programs from Mesa-utils (glxgears, etc...) working.

## How it works

Simply, this makes use of ` LD_PRELOAD ` to hook into relevant OpenGL and GLX calls

Calling the modern OpenGL API from ` GLMD ` is a bit trickier though, as typically you would need a function loader, such as ` GLAD ` or ` GLEW `. But, these are libraries which would need to be linked to the target executable, which wouldn't be very convenient or useful. Instead, these functions need to be loaded manually. 

## Using

To build the shared library simply call ` make build `

To use with a program, ` LD_PRELOAD ` needs to be set, like so:

` LD_PRELOAD=$PWD/glmd.so glxgears `

There are also the ` test ` and ` gears ` Makefile targets for examples/testing
