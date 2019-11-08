LIBS = -lXi -lXmu -lglut -lGLEW -lGLU -lm -lGL

cone: cone.c initShader.o matrixlib.o
	gcc -o cone cone.c initShader.o matrixlib.o $(LIBS)

matrixlib: matrixlib.c
	gcc -c matrixlib matrixlib.c

cone2: cone2.c initShader.o
	gcc -o cone2 cone2.c initShader.o $(LIBS)

initShader.o: initShader.c initShader.h
	gcc -c initShader.c
