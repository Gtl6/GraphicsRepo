/*
 * Project1.c
 *
 *  Created on: September 26, 2019
 *      Author: Griffin Lynch
 */


#ifdef __APPLE__  // include Mac OS X verions of headers

#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>

#else // non-Mac OS X operating systems

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>
#include <stdio.h>

#endif  // __APPLE__

#include "math.h"
#include "initShader.h"

#define BUFFER_OFFSET( offset )   ((GLvoid*) (offset))

typedef struct
{
    GLfloat x;
    GLfloat y;
    GLfloat z;
    GLfloat w;
} vec4;

// I don't like using #define any more than you do, but it seems like the best way to do it
#define RESOLUTION 24
int num_vertices = RESOLUTION * 6;

// The cone will be from y = -1 to y = 1
void generate_vertices(int res, vec4 *arr){
	int i = 0;
	int index = 0;
	float chunksize = (2.0f * 3.1415926535f) / res;
	for(i = 0; i < res; i++){
		// add the center
		vec4 made = {0, -0.5f, 0, 1.0f};
		arr[index] = made;
		index++;
		// The two points we need to generate, and remember the right hand rule!
		float x1 = 0.5f * cosf(chunksize * (i + 1));
		float z1 = 0.5f * sinf(chunksize * (i + 1));
		float x2 = 0.5f * cosf(chunksize * i);
		float z2 = 0.5f * sinf(chunksize * i);
		made.x = x1;
		made.z = z1;
		arr[index] = made;
		index++;
		made.x = x2;
		made.z = z2;
		arr[index] = made;
		index++;

		// Now to add the upper triangle (the one we'll actually see)
		made.x = 0;
		made.y = 0.5f;
		made.z = 0;
		arr[index] = made;
		index++;

		made.x = x2;
		made.y = -0.5f;
		made.z = z2;
		arr[index] = made;
		index++;

		made.x = x1;
		made.z = z1;
		arr[index] = made;
		index++;
	}
}

float random_col(){
	return (float)(rand() % 100) / 100;
}

void generate_colors(int res, vec4 *arr){
	// Just need to fill the list with res * 2 random colors
	int i;
	for(i = 0; i < res * 6; i++){
		vec4 col = {random_col(), random_col(), random_col(), 1.0f};
		arr[i] = col;
		i++;
		arr[i] = col;
		i++;
		arr[i] = col;
	}
}

void init(void)
{
    GLuint program = initShader("vshader.glsl", "fshader.glsl");
    glUseProgram(program);

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

	vec4 vertices[RESOLUTION * 6];
   	generate_vertices(RESOLUTION, vertices);
	vec4 colors[RESOLUTION * 6];
   	generate_colors(RESOLUTION, colors);

    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) + sizeof(colors), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices), sizeof(colors), colors);

    GLuint vPosition = glGetAttribLocation(program, "vPosition");
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

    GLuint vColor = glGetAttribLocation(program, "vColor");
    glEnableVertexAttribArray(vColor);
    glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid *) sizeof(vertices));

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glDepthRange(1,0);
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPolygonMode(GL_FRONT, GL_FILL);
    glPolygonMode(GL_BACK, GL_LINE);
    glDrawArrays(GL_TRIANGLES, 0, num_vertices);

    glutSwapBuffers();
}

void keyboard(unsigned char key, int mousex, int mousey)
{
    if(key == 'q')
    	exit(0);

    glutPostRedisplay();
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(512, 512);
    glutInitWindowPosition(100,100);
    glutCreateWindow("Project1 - Cone");
    glewInit();
    init();

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutMainLoop();

    return 0;
}
