LIBS = -lXi -lXmu -lglut -lGLEW -lGLU -lm -lGL

all: lab6.c initShader.o matrixlib.o
	g++ -o lab6.o lab6.c matrixlib.o initShader.o $(LIBS)

initShader.o: initShader.c initShader.h
	g++ -c initShader.c

matrixlib.o: matrixlib.c matrixlib.h
	g++ -c matrixlib.c

clean:
	rm *.o
