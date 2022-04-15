all: client-phase1 client-phase2 client-phase3 client-phase4 client-phase5

client-phase1: client-phase1.cpp
	g++ -std=c++11 client-phase1.cpp -o client-phase1 -lpthread

client-phase2: client-phase2.cpp
	g++ -std=c++11 client-phase2.cpp -o client-phase2 -lpthread

client-phase3: client-phase3.cpp
	g++ -std=c++11 client-phase3.cpp -o client-phase3 -lpthread

client-phase4: client-phase4.cpp
	g++ -std=c++11 client-phase4.cpp -o client-phase4 -lpthread

client-phase5: client-phase5.cpp
	g++ -std=c++11 client-phase5.cpp -o client-phase5 -lpthread
