/*
 * triangle.c
 *
 *  Created on: Aug 28, 2017
 *      Author: Thumrongsak Kosiyatrakul
 */


#ifdef __APPLE__  // include Mac OS X verions of headers

#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>

#else // non-Mac OS X operating systems

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>

#endif  // __APPLE__

#include "initShader.h"
#include "matrixlib.h"

#define BUFFER_OFFSET( offset )   ((GLvoid*) (offset))

vec4 vertices[6] =
{{ 0.0,  0.5,  0.0, 1.0},	// top
 {-0.5, -0.5,  0.0, 1.0},	// bottom left
 { 0.5, -0.5,  0.0, 1.0},	// bottom right
 { 0.5,  0.8, -0.5, 1.0},	// top
 { 0.9,  0.0, -0.5, 1.0},	// bottom right
 { 0.1,  0.0, -0.5, 1.0}};	// bottom left

vec4 colors[6] =
{{1.0, 0.0, 0.0, 1.0},	// red   (for top)
 {0.0, 1.0, 0.0, 1.0},	// green (for bottom left)
 {0.0, 0.0, 1.0, 1.0},	// blue  (for bottom right)
 {0.0, 1.0, 0.0, 1.0},	// green (for bottom right)
 {0.0, 1.0, 0.0, 1.0},	// green (for bottom right)
 {0.0, 1.0, 0.0, 1.0}};	// green (for bottom right)

const float circle_radius = 0.05;
const int circle_resolution = 8;
const float coil_space = 0.05;
const int num_coils = 5;
const int coil_resolution = 8;

// Calculates the number of vertices
int NUMVERTICES = 6 * circle_resolution * coil_resolution * num_coils + 6 * circle_resolution;

GLuint ctm_location;
mat4 ctm;

// The spring is generated depending on input variables as specified above
void generate_spring(vec4 *arr){
	// My Code
}

float random_col(){
	return (float)(rand() % 100) / 100;
}

void generate_colors(vec4 *arr){
	int i;
	for(i = 0; i < NUMVERTICES; i++){
		vec4 col = {random_col(), random_col(), random_col(), 1.0f};
		arr[i++] = col;
		arr[i++] = col;
		arr[i] = col;
	}
}


void init(void)
{
    GLuint program = initShader("vshader.glsl", "fshader.glsl");
    glUseProgram(program);

	ctm_location = glGetUniformLocation(program, "ctm");

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

	vec4 vertices[NUMVERTICES];
	generate_vertices(vertices);
	vec4 colors[NUMVERTICES];
	generate_colors(colors);

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
    
	glUniformMatrix4fv(ctm_location, 1, GL_FALSE, (GLfloat *) &ctm);

	glDrawArrays(GL_TRIANGLES, 0, num_vertices);
    glutSwapBuffers();
}

void keyboard(unsigned char key, int mousex, int mousey)
{
    if(key == 'q')
    	exit(0);

    glutPostRedisplay();
}

void idle(void){
	// Continue rotation, if there is any
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(512, 512);
    glutInitWindowPosition(100,100);
    glutCreateWindow("Triangle");
    glewInit();
    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);
    glutMainLoop();

    return 0;
}
