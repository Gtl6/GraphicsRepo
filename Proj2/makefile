LIBS = -lXi -lXmu -lglut -lGLEW -lGLU -lm -lGL

proj2.o: proj2.c initShader.o matrixlib.o MazeGenerator.o
	gcc -o proj2 proj2.c MazeGenerator.o initShader.o matrixlib.o $(LIBS)

MazeGenerator: MazeGenerator.c
	gcc -c MazeGenerator MazeGenerator.c

matrixlib: matrixlib.c
	gcc -c matrixlib matrixlib.c

initShader.o: initShader.c initShader.h
	gcc -c initShader.c

testmaze.o: testgenerator.c MazeGenerator.o
	gcc -o testmaze testgenerator.c MazeGenerator.o

clean:
	rm *.o
