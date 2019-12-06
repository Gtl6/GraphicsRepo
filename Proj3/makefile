LIBS = -lXi -lXmu -lglut -lGLEW -lGLU -lm -lGL

proj3.o: proj3.c initShader.o matrixlib.o
	gcc -o proj3.o proj3.c initShader.o matrixlib.o $(LIBS)

matrixlib: matrixlib.c
	gcc -c matrixlib matrixlib.c

initShader.o: initShader.c initShader.h
	gcc -c initShader.c

clean:
	rm *.o
