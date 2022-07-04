outfile=bin/tetris
cflags="-g `sdl2-config --cflags` -DDEBUG -DLINUX -Ilib/glad-gl3.3/include/ -Ilib/glm -Wall -Wno-unused-variable -isystemlib `pkg-config --cflags freetype2`"
ldflags=
ldlibs="-ldl `sdl2-config --libs` `pkg-config --libs freetype2`"

# if glad.o does not exist, build it
if [ ! -f glad.o ]; then
  clang -o glad.o -c lib/glad-gl3.3/src/glad.c -Ilib/glad-gl3.3/include/
fi

clang++ $cflags main.cpp -o $outfile $ldflags $ldlibs glad.o
