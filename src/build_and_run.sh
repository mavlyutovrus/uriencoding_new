REGIM=-O3
g++ $REGIM main.cpp -o main.o -std=gnu++11 -c
g++ $REGIM hat-trie/*.c -std=gnu++11 -c
g++ $REGIM art-tree/art.c -msse2 -msse -msse3 -o art.o -std=gnu++11 -c
g++ $REGIM lexico_tree/KeyManager.cpp -o KeyManager.o -std=gnu++11 -c
g++ $REGIM -o tester *.o  -lboost_system -lpthread 
rm *.o

for filename in `ls ../datasets/`  
do
     echo "processing $filename"
     ./tester ../datasets/$filename
done
