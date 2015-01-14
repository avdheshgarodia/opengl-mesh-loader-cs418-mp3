#Makefile for mp3
.PHONY: all clean
all: mp3
	
mp3: mp3.cpp
	g++ -Wall -o mp3 mp3.cpp -lSOIL -lGLEW -framework OpenGL -framework GLUT -framework CoreFoundation
clean:
	rm  -f mp3 