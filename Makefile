Compiler = gcc
IncludeFlags = -I Source -I Libraries/SDL2/Include -I Libraries/INIH
LinkerFlags = -L Libraries/SDL2/lib/x64 -L Libraries/INIH -lSDL2main -lSDL2 -lSDL2_mixer -lSDL2_image -lINIH
TestFlags = -o0
DebugFlags = -g3 -o0
ReleaseFlags = -g0 -o3 -s -flto -std=c11
SOURCES = Source/MOBSCE.c Source/CONFIG.c Source/INPUT.c Source/RESOURCE.c Source/VIDEO.c Source/AUDIO.c
OBJECTS = $(SOURCES:Source/%.c=Binaries/%.o)
Target = GAME

all: Test

Test:
	cls
	$(Compiler) $(TestFlags) Source/*.c Source/Non-Engine/*.c -o Binaries/$(Target)_Test.exe $(IncludeFlags) $(LinkerFlags)
	Binaries/$(Target)_Test
Debug:
	cls
	$(Compiler) $(DebugFlags) Source/*.c Source/Non-Engine/*.c -o Binaries/$(Target)_Debug.exe $(IncludeFlags) $(LinkerFlags)
	gdb Binaries/$(Target)_Debug
Release:
	cls
	windres resources.rc -O coff -o resources.res
	$(Compiler) $(ReleaseFlags) resources.res Source/*.c Source/Non-Engine/*.c -o Binaries/$(Target).exe $(IncludeFlags) $(LinkerFlags) 

$(OBJECTS): Binaries/%.o: Source/%.c
	$(Compiler) $(ReleaseFlags) $(IncludeFlags) -c $< -o $@

StaticLibrary: $(OBJECTS)
	ar rcs Binaries/libMOBSCE.a $(OBJECTS)
	del Binaries\*.o
	rm Binaries/*.o

StaticTest:
	cls
	$(Compiler) $(TestFlags) Source/Non-Engine/*.c -o Binaries/$(Target)_Test.exe $(IncludeFlags) -L Binaries $(LinkerFlags) -lMOBSCE
	Binaries/$(Target)_Test
StaticDebug:
	cls
	$(Compiler) $(DebugFlags) Source/Non-Engine/*.c -o Binaries/$(Target)_Debug.exe $(IncludeFlags) -L Binaries $(LinkerFlags) -lMOBSCE
	gdb Binaries/$(Target)_Debug
StaticRelease:
	cls
	windres resources.rc -O coff -o resources.res
	$(Compiler) $(ReleaseFlags) resources.res Source/Non-Engine/*.c -o Binaries/$(Target).exe $(IncludeFlags) -L Binaries $(LinkerFlags) -lMOBSCE