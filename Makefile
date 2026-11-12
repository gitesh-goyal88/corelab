CXX ?= g++
corelab: src/main.cpp $(wildcard src/*.hpp) src/content.hpp src/http_server.hpp src/simulations.hpp src/simulations2.hpp src/simulations3.hpp src/simulations4.hpp src/simulations5.hpp src/physics.hpp src/physics2.hpp src/mathsim1.hpp src/mathsim2.hpp src/mathsim3.hpp src/mathsim4.hpp src/simulations6.hpp src/simulations7.hpp src/simulations8.hpp src/simulations9.hpp
	$(CXX) -std=c++17 -O2 -pthread -Wall -Wno-misleading-indentation -o corelab src/main.cpp
run: corelab
	./corelab 8080
clean:
	rm -f corelab
