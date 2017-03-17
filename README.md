# ZD
This will be a os layer based on posix.

g++ -I./include src/*.c -fPIC -shared -o libzd.so

gcc -I./include src/*.c -fPIC -shared -o libzd.so -lpthread -lrt  

gcc test.c -L. libzd.so -o test
