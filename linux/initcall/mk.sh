gcc -fPIC -g -c liba.c
gcc -shared -g -o liba.so liba.o
gcc -o test main.c -L./ -la

export LD_LIBRARY_PATH+="$(pwd)"
./test
