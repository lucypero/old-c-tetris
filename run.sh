if [ "$1" == "sw" ]; then 
  export LIBGL_ALWAYS_SOFTWARE=1
else
  export LIBGL_ALWAYS_SOFTWARE=0
fi
./bin/tetris
