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

// Some constants to set
// Change them if you like
const float circle_radius = 0.1;
const int circle_resolution = 16;
const float coil_space = 0.03;
const int num_coils = 6;
const int coil_resolution = 32;
const float coil_center_radius = 0.25;

// Just some nice derivations to have on hand, please don't change these
int CIRCLEVERTCOUNT = 3 * circle_resolution;
int COILVERTCOUNT = 6 * circle_resolution * coil_resolution;
int NUMVERTICES = COILVERTCOUNT * num_coils + 2 * CIRCLEVERTCOUNT;
float CIRCANGLE = 2 * PI / circle_resolution;
float TUBEANGLE = 2 * PI / coil_resolution;
float TUBEHEIGHTS = (2 * circle_radius + coil_space) / coil_resolution;

// Some global variables
GLuint ctm_location;
mat4 ctm;
float spring_rotate = 0.05;
vec4 old_mouse_pos;
vec4 release_vector;
bool clicked;

// Will generate just one circle at a time, because I like making functions and offloading effort instead of actually writing code
void generate_one_circle(vec4 *arr, float x, float y, float z, char is_left){
	int i;
	for(i = 0; i < 3 * circle_resolution; i++){
		// Dealing with the angle_numth triangle
		int angle_num = i / 3;
		float theta = CIRCANGLE * angle_num;

		// The first is the center, the second is our point, the third is the next point counter-clockwise
		vec4 center = {0, 0, 0, 1.0};
		vec4 p1 = {circle_radius * cos(theta), circle_radius * sin(theta), 0, 1.0};
		vec4 p2 = {circle_radius * cos(theta + CIRCANGLE), circle_radius * sin(theta + CIRCANGLE), 0, 1.0};

		// Rotate and translate the points
		float angle = PI / 2;
		if(is_left) angle *= -1;
		mat4 ry = rotate_y(angle);
		mat4 trans_mat = translate(x, y, z);
		mat4 movement_mat = matrix_matrix_multiply(trans_mat, ry);
		vec4 new_center = matrix_vector_multiply(movement_mat, center);
		vec4 new_p1 = matrix_vector_multiply(movement_mat, p1);
		vec4 new_p2 = matrix_vector_multiply(movement_mat, p2);
		
		// Load the vertices into the array
		arr[i] = new_center;
		i++;
		arr[i] = new_p1;
		i++;
		arr[i] = new_p2;
	}
}

// Will generate the circles on the ends of the spring
void generate_circles(vec4 *arr){
	float x = 0;
	float y = 0.5 * (coil_space) + 0.5 * (num_coils - 1) * (coil_space + 2 * circle_radius) + circle_radius; // We have our spring centered at the origin, so this is gross
	float z = coil_center_radius;

	generate_one_circle(arr, x, y, z, 0);
	generate_one_circle(arr + CIRCLEVERTCOUNT, x, -1 * y, z, 1); // But since it's at the origin, this is easy
}


float top_radius(float front_angle){
	return coil_center_radius + circle_radius * cos(front_angle);
}

// Creates a "tube", or a single chunk of a taurus
void generate_tube(vec4 *arr, float height, int which_tube){
	int i;
	float top_angle1 = which_tube * TUBEANGLE;
	float top_angle2 = (which_tube + 1) * TUBEANGLE;
	float height2 = height + TUBEHEIGHTS;

	for(i = 0; i < 6 * circle_resolution; i++){
		int angle_count = i / 6;
		float front_angle1 = angle_count * CIRCANGLE;
		float front_angle2 = (angle_count + 1) * CIRCANGLE;
		// In order, our point, the next point on the circle, our point on the other side of the tube, and the next point on the other side of the tube
		// Do you ever look at a line of code (or four) and wonder where your life went so horribly wrong?
		vec4 p1 = {top_radius(front_angle1) * sin(top_angle1), height + circle_radius * sin(front_angle1), top_radius(front_angle1) * cos(top_angle1), 1.0};
		vec4 p2 = {top_radius(front_angle2) * sin(top_angle1), height + circle_radius * sin(front_angle2), top_radius(front_angle2) * cos(top_angle1), 1.0};
		vec4 p3 = {top_radius(front_angle1) * sin(top_angle2), height2 + circle_radius * sin(front_angle1), top_radius(front_angle1) * cos(top_angle2), 1.0};
		vec4 p4 = {top_radius(front_angle2) * sin(top_angle2), height2 + circle_radius * sin(front_angle2), top_radius(front_angle2) * cos(top_angle2), 1.0};

		// On the plus side, I somehow managed to make it so that the surface's normal angle doesn't matter
		// Don't ask me how. Absolutely bonkers how that worked out.
		arr[i] = p1;
		i++;
		arr[i] = p3;
		i++;
		arr[i] = p4;
		i++;
		arr[i] = p1;
		i++;
		arr[i] = p4;
		i++;
		arr[i] = p2;
	}
}

void generate_coil(vec4 *arr, float height){
	int i;
	int VERTSPERTUBE = 6 * circle_resolution;

	for(i = 0; i < coil_resolution; i++){
		generate_tube(arr + i * VERTSPERTUBE, height + i * TUBEHEIGHTS, i);
	}
}

// The spring is generated depending on input variables as specified above
void generate_spring(vec4 *arr){
	generate_circles(arr);
	float start_height = 0;
	float coil_height = 2 * circle_radius + coil_space;

	// If we have an odd number of coils, the middle will be the middle of a coil
	// With an even number, we have to shift it up by half a coil
	if(num_coils % 2 == 0){
		start_height = coil_height / 2;
	}

	// Note num_coils results in int division. This is on purpose (so 5 drops two coils, as would 4, so that's why we need the shifting)
	start_height -= (num_coils / 2) * coil_height + coil_height / 2;

	// To account for the fact we've already populated the array with two circles
	int circle_offset = 2 * CIRCLEVERTCOUNT;

	int i;
	for(i = 0; i < num_coils; i++){
		// Build a coil in the array (at the right spot) at the height given
		generate_coil(arr + circle_offset + i * (COILVERTCOUNT), start_height + i * coil_height);
	}
}



float random_col(){
	return (float)(rand()%100) / 100;
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

	// I'm not sure what this does, but it seems to make the ctm thing work
	ctm_location = glGetUniformLocation(program, "ctm");
	// Setup ctm as the identity matrix
	// Cooooome, as you are, as you were, as I waaaant you to be
	ctm = {{1,0,0,0}, {0,1,0,0},{0,0,1,0},{0,0,0,1}};

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

	vec4 vertices[NUMVERTICES];
	generate_spring(vertices);
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
