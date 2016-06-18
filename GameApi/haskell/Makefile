
all:
	g++ -g -c -std=c++11 GameApi.cpp -I.. -lpthread -pthread -o GameApi.o 
	ghc -g -c GameApi/GameApi.lhs -o GameApi/GameApi_hs.o
	ghc -g -dynamic -shared -fPIC GameApi/GameApi_hs.o GameApi.o -o libGameApi_hs.so -L.. -L. -lGameApi -lstdc++
	ghc -g --make Example.lhs -L.. -L. -lGameApi_hs -lGameApi
