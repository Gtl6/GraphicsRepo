LIBS = -lXi -lXmu -lglut -lGLEW -lGLU -lm -lGL

cone: cone.c initShader.o matrixlib.o
	gcc -o cone cone.c matrixlib.o initShader.o $(LIBS)

cone2: cone2.c initShader.o
	gcc -o cone2 cone2.c initShader.o $(LIBS)

matrixlib.o: matrixlib.c
	gcc -c matrixlib.c $(LIBS)

initShader.o: initShader.c initShader.h
	gcc -c initShader.c
