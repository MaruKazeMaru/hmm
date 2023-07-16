ls | grep -x build > /dev/null
[ $? -eq 0 ] || mkdir build
cd build

g++  -I../../include -c ../../src/markov_model.cpp #../../src/parser.cpp

ar rcs libmarkov_model.a markov_model.o
#ar rcs libparser.a parser.o

g++ -o debug.out ../calc_seq_prob.cpp -I../../include -L. -lmarkov_model #-lparser