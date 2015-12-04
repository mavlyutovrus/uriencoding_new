cd src
REGIM=-O3
g++ $REGIM main.cpp -o main.o -std=gnu++11 -c
g++ $REGIM hat-trie/*.c -std=gnu++11 -c
g++ $REGIM art-tree/art.c -msse2 -msse -msse3 -o art.o -std=gnu++11 -c
g++ $REGIM lexico_tree/KeyManager.cpp -o KeyManager.o -std=gnu++11 -c
g++ $REGIM -o tester *.o  -lboost_system -lpthread 
rm *.o
mv tester ../
cd ..

echo "DS_NAME:$@"
./tester $@ | grep -v "^#"
