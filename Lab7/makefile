LIBS = -lXi -lXmu -lglut -lGLEW -lGLU -lm -lGL

all: proj1.c initShader.o matrixlib.o
	g++ -o proj1.o proj1.c matrixlib.o initShader.o $(LIBS)

initShader.o: initShader.c initShader.h
	g++ -c initShader.c

matrixlib.o: matrixlib.c matrixlib.h
	g++ -c matrixlib.c $(LIBS)

clean:
	rm *.o
