filename=$(basename -- "$1")
extension="${filename##*.}"
command_name=""
if [ $extension == "c" ]
then
    command_name=clang
elif [ $extension == "cpp" ]
then
    command_name=clang++
fi

cflags="-g `sdl2-config --cflags` -DDEBUG -Ilib/glad-gl3.3/include/ -Wall -Wno-unused-variable -isystemlib/ `pkg-config --cflags freetype2`"
ldflags=
ldlibs="-ldl `sdl2-config --libs` `pkg-config --libs freetype2`"

whole_command="$command_name $cflags $1 -o bin/tst_`basename ${1%%.*}` $ldflags $ldlibs glad.o" 


# if glad.o does not exist, build it
if [ ! -f glad.o ]; then
  clang -o glad.o -c lib/glad-gl3.3/src/glad.c -Ilib/glad-gl3.3/include/
fi

echo $whole_command
$whole_command

if [ $? -eq 0 ]
then
    echo "------- Output: ------"
    #time ${1%%.*}
    bin/tst_`basename ${1%%.*}`
fi
