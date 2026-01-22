Compiler = gcc
LinkerFlags = -I Libraries/SDL2/Include -I Libraries/INIH -L Libraries/SDL2/lib/x64 -L Libraries/INIH -lSDL2main -lSDL2 -lSDL2_mixer -lSDL2_image -lINIH
TestFlags = -o0
DebugFlags = -g3 -o0
ReleaseFlags = -g0 -o3 -s -flto -std=c11
Target = GAME

all: Test

Test:
	cls
	$(Compiler) $(TestFlags) Source/*.c -o Binaries/$(Target)_Test.exe $(LinkerFlags)
	Binaries/$(Target)_Test
Debug:
	cls
	$(Compiler) $(DebugFlags) Source/*.c -o Binaries/$(Target)_Debug.exe $(LinkerFlags)
	gdb Binaries/$(Target)_Debug
Release:
	cls
	windres resources.rc -O coff -o resources.res
	$(Compiler) $(ReleaseFlags) resources.res Source/*.c -o Binaries/$(Target).exe $(LinkerFlags) 