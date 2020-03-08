# build an executable named parser from main.cpp
all: main.cpp 
	g++ -std=c++17 FileOperator.cpp main.cpp -o ProyectoSiso2 -lX11 -lstdc++fs

clean: 
	$(RM) main