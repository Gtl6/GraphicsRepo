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
#include <stdio.h>

#endif  // __APPLE__

#include <sys/time.h>
#include "math.h"
#include "initShader.h"
#include "matrixlib.h"

#define BUFFER_OFFSET( offset )   ((GLvoid*) (offset))

// Figuring out how many vertices I'm going to use
#define VERTS_PER_RECTANGLE 6
#define RECTANGLES_PER_CUBIE 18
#define TRIS_PER_CUBIE 8
#define VERTS_PER_TRI 3
#define VERTS_PER_CUBIE (RECTANGLES_PER_CUBIE * VERTS_PER_RECTANGLE) + (TRIS_PER_CUBIE * VERTS_PER_TRI)

// The verts for our nice cubie
#define num_vertices 27 * VERTS_PER_CUBIE

int clicked = 0;
vec4 old_mouse_pos;
vec4 release_vector;

float near = 0.15;
float far = 0.17;

// We'll pass this to our draw function to tell it which colors to use
typedef struct {
  int GREEN;
  int RED;
  int BLUE;
  int ORANGE;
  int WHITE;
  int YELLOW;
} cubie_colors;

// Properties of our point light
// Position is defined below
vec4 light_ambient = {0.2, 0.2, 0.2, 1.0};
vec4 light_diffuse = {1, 1, 1, 1};
vec4 light_specular = {1, 1, 1, 1};
vec4 light_position = {0, 0.5, -1, 1};

// Calculating color locally, so this is all we need to give to the vShader
mat4 ctm;
GLuint ctm_location;

typedef enum{
  FRONT,
  BACK,
  LEFT,
  RIGHT,
  BOTTOM,
  TOP
} faces;

// Used as a way to make the get_point function cleaner
void easy_update_vecVal(faces face, faces expected_one, faces expected_other, int one_or_other, float *placer){
  int err = 0;
  if(one_or_other == 0){
    if(face == expected_one){
      *placer = -1 * far;
    } else if (face == expected_other) {
      err = 1;
    } else {
      *placer = -1 * near;
    }
  }
  else if(one_or_other == 1){
    if(face == expected_other){
      *placer = far;
    } else if (face == expected_one) {
      err = 1;
    } else {
      *placer = near;
    }
  }

  if(err == 1) printf("You got to one of the unreachable bits when generating a cubie.\n");
}

// Need to derive points based on which part of the cube they are
//    The name refers to which point it is, zero is the first, 1 is the second
vec4 get_point(int left_right, int bottom_top, int back_front, faces face){
  vec4 result;

  // Set each of the values according to the input information
  easy_update_vecVal(face, BACK, FRONT, back_front, &(result.z));
  easy_update_vecVal(face, LEFT, RIGHT, left_right, &(result.x));
  easy_update_vecVal(face, BOTTOM, TOP, bottom_top, &(result.y));
  result.w = 1;

  return result;
}

// A very long, repetitive function that sticks a cubie into memory (including the colors)
void generate_cubie(vec4 *vertices, vec4 *colors){
  // Back (our side) first
  vertices[0] = get_point(0, 0, 0, BACK);
  vertices[1] = get_point(1, 0, 0, BACK);
  vertices[2] = get_point(1, 1, 0, BACK);
  vertices[3] = get_point(0, 0, 0, BACK);
  vertices[4] = get_point(1, 1, 0, BACK);
  vertices[5] = get_point(0, 1, 0, BACK);
  // Now front (which is facing away from us)
  vertices[6] = get_point(1, 0, 1, FRONT);
  vertices[7] = get_point(0, 0, 1, FRONT);
  vertices[8] = get_point(0, 1, 1, FRONT);
  vertices[9] = get_point(0, 1, 1, FRONT);
  vertices[10] = get_point(1, 1, 1, FRONT);
  vertices[11] = get_point(1, 0, 1, FRONT);
  // Now left
  vertices[12] = get_point(0, 0, 1, LEFT);
  vertices[13] = get_point(0, 0, 0, LEFT);
  vertices[14] = get_point(0, 1, 0, LEFT);
  vertices[15] = get_point(0, 0, 1, LEFT);
  vertices[16] = get_point(0, 1, 0, LEFT);
  vertices[17] = get_point(0, 1, 1, LEFT);
  // And right
  vertices[18] = get_point(1, 0, 0, RIGHT);
  vertices[19] = get_point(1, 0, 1, RIGHT);
  vertices[20] = get_point(1, 1, 1, RIGHT);
  vertices[21] = get_point(1, 0, 0, RIGHT);
  vertices[22] = get_point(1, 1, 1, RIGHT);
  vertices[23] = get_point(1, 1, 0, RIGHT);
  // Bottom
  vertices[24] = get_point(0, 0, 0, BOTTOM);
  vertices[25] = get_point(1, 0, 1, BOTTOM);
  vertices[26] = get_point(1, 0, 0, BOTTOM);
  vertices[27] = get_point(0, 0, 0, BOTTOM);
  vertices[28] = get_point(0, 0, 1, BOTTOM);
  vertices[29] = get_point(1, 0, 1, BOTTOM);
  // Top
  vertices[30] = get_point(0, 1, 0, TOP);
  vertices[31] = get_point(1, 1, 0, TOP);
  vertices[32] = get_point(1, 1, 1, TOP);
  vertices[33] = get_point(0, 1, 0, TOP);
  vertices[34] = get_point(1, 1, 1, TOP);
  vertices[35] = get_point(0, 1, 1, TOP);
  // Now comes the tricky part - the angled surfaces
  // Start off with the one above the back
  vertices[36] = get_point(0, 1, 0, BACK);
  vertices[37] = get_point(1, 1, 0, BACK);
  vertices[38] = get_point(1, 1, 0, TOP);
  vertices[39] = get_point(0, 1, 0, BACK);
  vertices[40] = get_point(1, 1, 0, TOP);
  vertices[41] = get_point(0, 1, 0, TOP);
  // Follow it forward to the one behind the top
  vertices[42] = get_point(0, 1, 1, TOP);
  vertices[43] = get_point(1, 1, 1, TOP);
  vertices[44] = get_point(1, 1, 1, FRONT);
  vertices[45] = get_point(0, 1, 1, TOP);
  vertices[46] = get_point(1, 1, 1, FRONT);
  vertices[47] = get_point(0, 1, 1, FRONT);
  // Keep wrapping around to front bottom
  vertices[48] = get_point(0, 0, 1, FRONT);
  vertices[49] = get_point(1, 0, 1, FRONT);
  vertices[50] = get_point(1, 0, 1, BOTTOM);
  vertices[51] = get_point(0, 0, 1, FRONT);
  vertices[52] = get_point(1, 0, 1, BOTTOM);
  vertices[53] = get_point(0, 0, 1, BOTTOM);
  // Now the bottom and back ones
  vertices[54] = get_point(0, 0, 0, BOTTOM);
  vertices[55] = get_point(1, 0, 0, BOTTOM);
  vertices[56] = get_point(1, 0, 0, BACK);
  vertices[57] = get_point(0, 0, 0, BOTTOM);
  vertices[58] = get_point(1, 0, 0, BACK);
  vertices[59] = get_point(0, 0, 0, BACK);
  //Now the top right one
  vertices[60] = get_point(1, 1, 0, RIGHT);
  vertices[61] = get_point(1, 1, 1, RIGHT);
  vertices[62] = get_point(1, 1, 1, TOP);
  vertices[63] = get_point(1, 1, 0, RIGHT);
  vertices[64] = get_point(1, 1, 1, TOP);
  vertices[65] = get_point(1, 1, 0, TOP);
  // Top left
  vertices[66] = get_point(0, 1, 1, LEFT);
  vertices[67] = get_point(0, 1, 0, LEFT);
  vertices[68] = get_point(0, 1, 0, TOP);
  vertices[69] = get_point(0, 1, 1, LEFT);
  vertices[70] = get_point(0, 1, 0, TOP);
  vertices[71] = get_point(0, 1, 1, TOP);
  // bottom right
  vertices[72] = get_point(1, 0, 0, BOTTOM);
  vertices[73] = get_point(1, 0, 1, BOTTOM);
  vertices[74] = get_point(1, 0, 1, RIGHT);
  vertices[75] = get_point(1, 0, 0, BOTTOM);
  vertices[76] = get_point(1, 0, 1, RIGHT);
  vertices[77] = get_point(1, 0, 0, RIGHT);
  // bottom left
  vertices[78] = get_point(0, 0, 1, BOTTOM);
  vertices[79] = get_point(0, 0, 0, BOTTOM);
  vertices[80] = get_point(0, 0, 0, LEFT);
  vertices[81] = get_point(0, 0, 1, BOTTOM);
  vertices[82] = get_point(0, 0, 0, LEFT);
  vertices[83] = get_point(0, 0, 1, LEFT);
  // front right
  vertices[84] = get_point(1, 0, 1, RIGHT);
  vertices[85] = get_point(1, 0, 1, FRONT);
  vertices[86] = get_point(1, 1, 1, FRONT);
  vertices[87] = get_point(1, 0, 1, RIGHT);
  vertices[88] = get_point(1, 1, 1, FRONT);
  vertices[89] = get_point(1, 1, 1, RIGHT);
  // front left
  vertices[90] = get_point(0, 0, 1, FRONT);
  vertices[91] = get_point(0, 0, 1, LEFT);
  vertices[92] = get_point(0, 1, 1, LEFT);
  vertices[93] = get_point(0, 0, 1, FRONT);
  vertices[94] = get_point(0, 1, 1, LEFT);
  vertices[95] = get_point(0, 1, 1, FRONT);
  // back right
  vertices[96] = get_point(1, 0, 0, BACK);
  vertices[97] = get_point(1, 0, 0, RIGHT);
  vertices[98] = get_point(1, 1, 0, RIGHT);
  vertices[99] = get_point(1, 0, 0, BACK);
  vertices[100] = get_point(1, 1, 0, RIGHT);
  vertices[101] = get_point(1, 1, 0, BACK);
  // back left
  vertices[102] = get_point(0, 0, 0, LEFT);
  vertices[103] = get_point(0, 0, 0, BACK);
  vertices[104] = get_point(0, 1, 0, BACK);
  vertices[105] = get_point(0, 0, 0, LEFT);
  vertices[106] = get_point(0, 1, 0, BACK);
  vertices[107] = get_point(0, 1, 0, LEFT);
  // Close to done! Let's not forget the triangles!
  // These are actually going to be pretty easy, cause it's the same coord
  // Start w/ top right, on the back
  vertices[108] = get_point(1, 1, 0, BACK);
  vertices[109] = get_point(1, 1, 0, RIGHT);
  vertices[110] = get_point(1, 1, 0, TOP);
  // Top left back
  vertices[111] = get_point(0, 1, 0, LEFT);
  vertices[112] = get_point(0, 1, 0, BACK);
  vertices[113] = get_point(0, 1, 0, TOP);
  // Top right front
  vertices[114] = get_point(1, 1, 1, FRONT);
  vertices[115] = get_point(1, 1, 1, TOP);
  vertices[116] = get_point(1, 1, 1, RIGHT);
  // Top left front
  vertices[117] = get_point(0, 1, 1, FRONT);
  vertices[118] = get_point(0, 1, 1, LEFT);
  vertices[119] = get_point(0, 1, 1, TOP);
  // Halfway done. Keep the faith, bud
  // bottom left back
  vertices[120] = get_point(0, 0, 0, BACK);
  vertices[121] = get_point(0, 0, 0, LEFT);
  vertices[122] = get_point(0, 0, 0, BOTTOM);
  // bottom right back
  vertices[123] = get_point(1, 0, 0, BACK);
  vertices[124] = get_point(1, 0, 0, BOTTOM);
  vertices[125] = get_point(1, 0, 0, RIGHT);
  //bottom left front
  vertices[126] = get_point(0, 0, 1, FRONT);
  vertices[127] = get_point(0, 0, 1, BOTTOM);
  vertices[128] = get_point(0, 0, 1, LEFT);
  // bottom right front
  vertices[129] = get_point(1, 0, 1, FRONT);
  vertices[130] = get_point(1, 0, 1, RIGHT);
  vertices[131] = get_point(1, 0, 1, BOTTOM);

  // Hopefully we got that right (lord only knows) so now let's do the colors
  int i;
  int clicker = 0;
  for(i = 0; i < 6; i++){colors[i + clicker] = (vec4){0, 1, 0, 1};}
  clicker = 6;
  for(i = 0; i < 6; i++){colors[i + clicker] = (vec4){0, 0, 1, 1};}
  clicker = 12;
  for(i = 0; i < 6; i++){colors[i + clicker] = (vec4){1, 0.5, 0, 1};}
  clicker = 18;
  for(i = 0; i < 6; i++){colors[i + clicker] = (vec4){1, 0, 0, 1};}
  clicker = 24;
  for(i = 0; i < 6; i++){colors[i + clicker] = (vec4){1, 1, 0, 1};}
  clicker = 30;
  for(i = 0; i < 6; i++){colors[i + clicker] = (vec4){1, 1, 1, 1};}
  // Wrap up the rest of the colors with black
  for(i = clicker + 6; i < VERTS_PER_CUBIE; i++){
    //colors[i] = (vec4){0, 0, 0, 1};
  }
}

// Generate all of the cubies and their colors
void generate_vertices_and_colors(vec4 *vertices, vec4 *colors){
  int i, j;
  // For each cubie, make it and put it in its place
  for(i = 0; i < 27; i++){
    float chunksize = 2 * far;
    float x_pos = ((i % 9) / 3) * chunksize - chunksize;
    float z_pos = (i / 9) * chunksize - chunksize;
    float y_pos = (i % 3) * chunksize - chunksize;
    generate_cubie(&(vertices[i * VERTS_PER_CUBIE]), &(colors[i * VERTS_PER_CUBIE]));
    // generate transform for this cube and also attach it to the internal model somehow
    mat4 optimus = translate(x_pos, y_pos, z_pos);
    for(j = 0; j < VERTS_PER_CUBIE; j++){
      vertices[i * VERTS_PER_CUBIE + j] = matrix_vector_multiply(optimus, vertices[i * VERTS_PER_CUBIE + j]);
    }
  }
}

// Need to generate the normals for when we're looking at the cube
void generate_normals(vec4 *vertices, vec4 *normals){
  // Literally all of these are triangles, sooooo...
  int i;
  vec4 v1, v2, v3;
  // Add the normal vectors in, face by face
  for(i = 0; i < num_vertices; i++){
    v1 = vertices[i];
    v2 = vertices[i + 1];
    v3 = vertices[i + 2];
    vec4 norm = normalize(cross_product(vector_sub(v2, v1), vector_sub(v3, v1)));
    normals[i++] = norm;
    normals[i++] = norm;
    normals[i] = norm; // Plus Plus done by the for loop
  }
}

//Generate vertices and stuff
void init(void)
{
		GLuint program = initShader("vshader.glsl", "fshader.glsl");
    glUseProgram(program);

    // Tell OpenGL where the shared vars are
    ctm_location = glGetUniformLocation(program, "ctm");
    ctm = identity_matrix();

    // Load in all our data straightup
    vec4 vertices[num_vertices];
    vec4 colors[num_vertices];
    vec4 normals[num_vertices];
		generate_vertices_and_colors(vertices, colors);
    generate_normals(vertices, normals);

    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) + sizeof(colors) + sizeof(normals), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices), sizeof(colors), colors);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices) + sizeof(colors), sizeof(normals), normals);


    GLuint vPosition = glGetAttribLocation(program, "vPosition");
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

    GLuint vColor = glGetAttribLocation(program, "vColor");
    glEnableVertexAttribArray(vColor);
    glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid *) sizeof(vertices));

    GLuint vNormal = glGetAttribLocation(program, "vNormal");
    glEnableVertexAttribArray(vNormal);
    glVertexAttribPointer(vNormal, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid *) sizeof(vertices) + sizeof(colors));

    glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
    glClearColor(0.5, 0.7, 1.0, 1.0);
    glDepthRange(1,0);

    glutPostRedisplay();
}

// Draw handler
void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPolygonMode(GL_FRONT, GL_FILL);
    glPolygonMode(GL_BACK, GL_LINE);

    // Let's draw some cubies!
    glUniformMatrix4fv(ctm_location, 1, GL_FALSE, (GLfloat *) &ctm);
    glDrawArrays(GL_TRIANGLES, 0, num_vertices);

    glutSwapBuffers();
}

void keyboard(unsigned char key, int mousex, int mousey)
{
    if(key == 'q'){
    	exit(0);
    }
}

// Tick handler
void idle(void){
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

// Mouse handler. Hopefully this will just plug in nicely.
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
				clicked = 1;
				old_mouse_pos = (vec4){xf, yf, 1 - sqrt(xf*xf + yf*yf), 1.0};
			}
		}
		else if(state == GLUT_UP){
			clicked = 0;
			vec4 cur_mouse_pos = {xf, yf, 1 - sqrt(xf*xf + yf*yf), 1.0};
			release_vector = vector_sub(old_mouse_pos, cur_mouse_pos);
		}
	}
	glutPostRedisplay();
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
		glEnable(GL_DEPTH_TEST);
    glutInitWindowSize(512, 512);
    glutInitWindowPosition(100,100);
    glutCreateWindow("Griffin Lynch's Project 4");
    glewInit();
    init();

    glutDisplayFunc(display);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
    glutKeyboardFunc(keyboard);
	  glutIdleFunc(idle);

    glutMainLoop();

    return 0;
}
