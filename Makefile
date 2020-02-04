# build an executable named parser from main.cpp
all: main.cpp 
	g++ main.cpp -o main -lX11

clean: 
	$(RM) main