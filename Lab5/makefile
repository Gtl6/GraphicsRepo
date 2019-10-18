LIBS = -lXi -lXmu -lglut -lGLEW -lGLU -lm -lGL

all: textureLab.c initShader.o
	g++ -o lab5 textureLab.c initShader.o $(LIBS)

initShader.o: initShader.c initShader.h
	g++ -c initShader.c


