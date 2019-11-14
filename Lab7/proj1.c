/*
 * proj1.c
 *
 *  Finished on: Oct 10, 2019
 *      Author: Griffin Lynch
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
#define PI 3.14159265

// Imagine the sphere as a grid of rectangles wrapped on a sphere
//    The grid is a sphere_resolution x sphere_resolution size grid
const float sphere_resolution = 16;
const float radius = 0.5;

// Just some nice derivations to have on hand, please don't change these
int VERTS_PER_RECTANGLE = 6;
int VERTS_PER_ROW = sphere_resolution * VERTS_PER_RECTANGLE;
int NUMVERTICES = sphere_resolution * VERTS_PER_ROW;

// Some global variables
GLuint ctm_location;
mat4 ctm;
float spring_rotate = 0.05;
vec4 old_mouse_pos;
vec4 release_vector;
bool clicked;

void generate_sphere(vec4 *arr){
	float vert_angle = PI / (sphere_resolution);
	int i, j;
	float angle1, angle2, y1, y2, xrad1, xrad2, xangle1, xangle2, xcoord, zcoord;
	int index = 0;
	vec4 zed = {0, 0, 0, 0};

	for(i = 0; i < sphere_resolution; i++){
		angle1 = i * vert_angle;
		angle2 = (i + 1) * vert_angle;
		y1 = radius * cosf(angle1);
		y2 = radius * cosf(angle2);
		xrad1 = radius * sinf(angle1);
		xrad2 = radius * sinf(angle2);
		for(j = 0; j < sphere_resolution; j++){
			xangle1 = j * vert_angle;
			xangle2 = (j + 1) * vert_angle;
			vec4 bl = {xrad2 * cosf(2 * xangle1), y2, xrad2 * sinf(2 * xangle1), 1};
			vec4 br = {xrad2 * cosf(2 * xangle2), y2, xrad2 * sinf(2 * xangle2), 1};
			vec4 tl = {xrad1 * cosf(2 * xangle1), y1, xrad1 * sinf(2 * xangle1), 1};
			vec4 tr = {xrad1 * cosf(2 * xangle2), y1, xrad1 * sinf(2 * xangle2), 1};

			bl = normalize(bl);
			br = normalize(br);
			tl = normalize(tl);
			tr = normalize(tr);

			// Most of the time do both of these, but at the top and bottom we only want one
			if(y2 != -1){
				arr[index++] = bl;
				arr[index++] = tr;
				arr[index++] = br;
			}

			if(y1 != 1){
				arr[index++] = bl;
				arr[index++] = tl;
				arr[index++] = tr;
			}
		}
	}
}

void generate_colors(vec4 *arr, vec4 *colors){
	int i;
	for(i = 0; i < NUMVERTICES; i++){
		// Get the three verts of your face
		vec4 v1 = arr[i];
		vec4 v2 = arr[i + 1];
		vec4 v3 = arr[i + 2];
		// Get two sides
		vec4 v4 = vector_sub(v2, v1);
		vec4 v5 = vector_sub(v3, v1);
		// And get the normal vector
		vec4 v6 = cross_product(v4, v5);
		// Normalize
		vec4 col = normalize(v6);

		colors[i++] = col;
		colors[i++] = col;
		colors[i] = col;
	}
}


void init(void)
{
    GLuint program = initShader("vshader.glsl", "fshader.glsl");
    glUseProgram(program);

		// I'm not sure what this does, but it seems to make the ctm thing work
		ctm_location = glGetUniformLocation(program, "ctm");
		// Setup ctm as the identity matrix
		// Cooooome, as you are, as you were, as I waaaant you to be
		ctm = {{1,0,0,0}, {0,1,0,0},{0,0,1,0},{0,0,0,1}};

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

		vec4 vertices[NUMVERTICES];
		vec4 colors[NUMVERTICES];
		generate_sphere(vertices);
		generate_colors(vertices, colors);

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

		glDrawArrays(GL_TRIANGLES, 0, NUMVERTICES);
    glutSwapBuffers();
}

void keyboard(unsigned char key, int mousex, int mousey)
{
    if(key == 'q')
    	exit(0);

    glutPostRedisplay();
}

void mouse(int button, int state, int x, int y){
	x -= 256;
	y -= 256;
	float xf = x / 256.0;
	float yf = y / 256.0;

	if(button == 3){
		ctm = matrix_matrix_multiply(scale(1.02, 1.02, 1.02), ctm);
	}
	else if(button == 4){
		float num = 1/ 1.02;
		ctm = matrix_matrix_multiply(scale(num, num, num), ctm);
	}
	else if(button == GLUT_LEFT_BUTTON){
		if(state == GLUT_DOWN){
			// Check it's within our circle
			if(xf*xf + yf*yf < 1){
				clicked = true;
				old_mouse_pos = {xf, yf, 1 - sqrt(xf*xf + yf*yf), 1.0};
			}
		}
		else if(state == GLUT_UP){
			clicked = false;
			vec4 cur_mouse_pos = {xf, yf, 1 - sqrt(xf*xf + yf*yf), 1.0};
			release_vector = vector_sub(old_mouse_pos, cur_mouse_pos);
		}
	}
	glutPostRedisplay();
}

// rotates the object according to what we got from the mouse
void rotate_ctm(vec4 v1, vec4 v2){
	// Fun fact, absolute value is equivalent to magnitude (it's just one-dimensional)
	float abs1 = magnitude(v1);
	float abs2 = magnitude(v2);

	float denom = abs1 * abs2;
	if(denom != 0){
		float res = dot_product(v1, v2) / denom;
		float angle = acos(res);

		if(!isnan(angle)){
			vec4 theperp = cross_product(scalar_vector_multiply(100, v1), scalar_vector_multiply(100, v2));

			ctm = matrix_matrix_multiply(rotate_about_vector(theperp, angle), ctm);
		}
	}
}

// The motion handler for the mouse movement handler
void motion(int x, int y){
	if(clicked){
		x -= 256;
		y -= 256;
		float xf = x / 256.0;
		float yf = y / 256.0;

		if(xf*xf + yf*yf < 1){
			vec4 cur_mouse_pos = {xf, yf, 1 - sqrt(xf*xf + yf*yf), 0};
			// Export that rotation to another function
			rotate_ctm(old_mouse_pos, cur_mouse_pos);
			old_mouse_pos = cur_mouse_pos;
		}
	}
	glutPostRedisplay();
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
		glutMouseFunc(mouse);
		glutMotionFunc(motion);
    glutMainLoop();

    return 0;
}
