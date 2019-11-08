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
#include "MazeGenerator.h"

#define BUFFER_OFFSET( offset )   ((GLvoid*) (offset))

typedef struct
{
    GLfloat x;
    GLfloat y;
} vec2;

// Figuring out how many vectors I'm going to use
#define MAZEWIDTH 8
#define MAZEHEIGHT 8
#define wallLen 4
#define wallThick 0.5
#define wallHeight 4.5
#define pillarThick 1
#define pillarHeight 5
float groundOffset = 4 * wallLen;

#define facesPerRectPrism

#define numWalls ((MAZEWIDTH + 1) * (MAZEHEIGHT + 1))
#define numWallPlusCols (numWalls + numWalls)
#define vertsPerRectPrism 36
// The total number of verts plus the 6 needed for the ground
#define num_vertices (vertsPerRectPrism * numWallPlusCols + 6)

// Defining our ground to go from (0, 0) to:
#define groundtoprightx (2 * groundOffset + MAZEWIDTH * (wallLen + pillarThick) + pillarThick)
#define groundtoprightz (2 * groundOffset + MAZEHEIGHT * (wallLen + pillarThick) + pillarThick)

// The vshader vars
mat4 view_ctm;
mat4 proj_ctm;
GLuint model_view_location;
GLuint projection_location;
float angle = 0;

// The texture mapping info
vec2 tex_coords[num_vertices];
vec2 brick_coords[6] = {{0.0, 0.5}, {0.5, 0.5}, {0.5, 0.0}, {0.0, 0.5}, {0.5, 0.0}, {0.0, 0.0}};
vec2 stone_coords[6] = {{0.5, 0.5}, {1.0, 0.5}, {1.0, 0.0}, {0.5, 0.5}, {1.0, 0.0}, {0.5, 0.0}};
vec2 grass_coords[6] = {{0.0, 1.0}, {0.5, 1.0}, {0.5, 0.5}, {0.0, 1.0}, {0.5, 0.5}, {0.0, 0.5}};

// Create the ground face
void add_ground(vec4 *arr){
	vec4 bl = {0, 0, 0, 1};
	vec4 br = {groundtoprightx, 0, 0, 1};
	vec4 tl = {0, 0, groundtoprightz, 1};
	vec4 tr = {groundtoprightx, 0, groundtoprightz, 1};

	int i;
	for(int i = 0; i < 6; i++)
		tex_coords[i] = grass_coords[i];

	arr[0] = bl;
	arr[1] = tr;
	arr[2] = br;
	arr[3] = bl;
	arr[4] = tl;
	arr[5] = tr;
}

// Makes a 1x1x1 cube centered at the origin
void make_unit_cube(vec4 *prismMem){
  vec4 rdb = {1, -1, 1, 1};
  vec4 rub = {1, 1, 1, 1};
  vec4 ldb = {-1, -1, 1, 1};
  vec4 lub = {-1, 1, 1, 1};
  vec4 rdf = {1, -1, -1, 1};
  vec4 ruf = {1, 1, -1, 1};
  vec4 ldf = {-1, -1, -1, 1};
  vec4 luf = {-1, 1, -1, 1};

  // Front
  prismMem[0] = ldb;
  prismMem[1] = rdb;
  prismMem[2] = rub;
  prismMem[3] = ldb;
  prismMem[4] = rub;
  prismMem[5] = lub;

  // Back
  // Just copied and pasted Front, so that's why the nums are weird
	prismMem[6] = rdf;
  prismMem[7] = ldf;
  prismMem[8] = luf;
  prismMem[9] = rdf;
  prismMem[10] = luf;
  prismMem[11] = ruf;

  // Left
  prismMem[12] = ldf;
  prismMem[13] = ldb;
  prismMem[14] = lub;
  prismMem[15] = ldf;
  prismMem[16] = lub;
  prismMem[17] = luf;

  // Right
  // Ditto to back
  prismMem[18] = rdb;
  prismMem[19] = rdf;
  prismMem[20] = ruf;
  prismMem[21] = rdb;
  prismMem[22] = ruf;
  prismMem[23] = rub;

  // Top
  prismMem[24] = lub;
  prismMem[25] = rub;
  prismMem[26] = ruf;
  prismMem[27] = lub;
  prismMem[28] = ruf;
  prismMem[29] = luf;

  // Bottom
  prismMem[30] = rdb;
  prismMem[31] = ldb;
  prismMem[32] = ldf;
  prismMem[33] = rdb;
  prismMem[34] = ldf;
  prismMem[35] = rdf;

  // Change the dimensions of the given rectangular prism to those specified
  mat4 changermat = scale(0.5, 0.5, 0.5);
	int i;
	for(i = 0; i < 36; i++){
		prismMem[i] = matrix_vector_multiply(changermat, prismMem[i]);
	}
}

// Builds a single pillar at some calculated coordinates
void make_pillar(int j, int i, vec4 *arr, vec2 *texs){
	float x = groundOffset + j * (pillarThick + wallLen) + 0.5 * pillarThick;
	float z = groundOffset + i * (pillarThick + wallLen) + 0.5 * pillarThick;

	int p, q;
	for(p = 0; p < 6; p++){
		for(q = 0; q < 6; q++){
			texs[6 * p + q] = stone_coords[q];
		}
	}

	make_unit_cube(arr);
	mat4 resizemat = scale(pillarThick, pillarHeight, pillarThick);
	mat4 movmat = translate(x, 0.5 * pillarHeight, z);
	mat4 changemat = matrix_matrix_multiply(movmat, resizemat);

	for(i = 0; i < 36; i++){
		arr[i] = matrix_vector_multiply(changemat, arr[i]);
	}
}

// Builds a single horizontal wall at the origin
void make_wall(vec4 *arr, vec2 *texs){
	make_unit_cube(arr);
	mat4 resizemat = scale(wallLen, wallHeight, wallThick);

	int i, j;
	for(i = 0; i < 6; i++){
		for(j = 0; j < 6; j++){
			texs[6 * i + j] = brick_coords[j];
		}
	}

	for(i = 0; i < 36; i++){
		arr[i] = matrix_vector_multiply(resizemat, arr[i]);
	}
}

// Builds a horizontal wall at some calculated coordinates
void make_horizontal_wall(int j, int i, vec4 *arr, vec2 *texs){
	float x = groundOffset + j * (pillarThick + wallLen) + pillarThick + 0.5 * wallLen;
	float z = groundOffset + i * (pillarThick + wallLen) + 0.5 * pillarThick;

	make_wall(arr, texs);
	mat4 movmat = translate(x, 0.5 * wallHeight, z);

	for(i = 0; i < 36; i++)
		arr[i] = matrix_vector_multiply(movmat, arr[i]);
}

// Builds a vertical wall at some calculated coordinates
void make_vertical_wall(int j, int i, vec4 *arr, vec2 *texs){
	float x = groundOffset + j * (pillarThick + wallLen) + 0.5 * pillarThick;
	float z = groundOffset + i * (pillarThick + wallLen) + pillarThick + 0.5 * wallLen;

	make_wall(arr, texs);
	mat4 rotmat = rotate_y(PI / 2);
	mat4 movmat = translate(x, 0.5 * wallHeight, z);
	mat4 changemat = matrix_matrix_multiply(movmat, rotmat);

	for(i = 0; i < 36; i++){
		arr[i] = matrix_vector_multiply(changemat, arr[i]);
	}
}


// Builds the world
void generate_vertices(vec4 *arr){
  int maze[MAZEWIDTH * MAZEHEIGHT];
	make_maze(maze, MAZEWIDTH, MAZEHEIGHT);

  print_maze(maze, MAZEWIDTH, MAZEHEIGHT);

	add_ground(arr);

	int i, j;
	// Initialized to 6 cause of the ground plane
	int arrayOffset = 6;


	// This is actually going to be very similar to my print function
	for(i = 0; i < MAZEHEIGHT; i++){
    for(j = 0; j < MAZEWIDTH; j++){
			// Figure out whether we need walls
			int box = maze[i * MAZEWIDTH + j];
      // Make a pillar
			make_pillar(j, i, arr + arrayOffset, tex_coords + arrayOffset);
			arrayOffset += vertsPerRectPrism;

      if(box % 2 != 0) {
					make_horizontal_wall(j, i, arr + arrayOffset, tex_coords + arrayOffset);
					arrayOffset += vertsPerRectPrism;
			}

      // build the mid wall, if necessary
      if(box % 7 != 0) {
				make_vertical_wall(j, i, arr + arrayOffset, tex_coords + arrayOffset);
				arrayOffset += vertsPerRectPrism;
			}
    }
		// Build the right pillar and side
		make_pillar(j, i, arr + arrayOffset, tex_coords + arrayOffset);
		arrayOffset += vertsPerRectPrism;
    make_vertical_wall(j, i, arr + arrayOffset, tex_coords + arrayOffset);
    arrayOffset += vertsPerRectPrism;
	}

	// And now do the back row of walls
	for(j = 0; j < MAZEWIDTH + 1; j++){
		// Build the right pillar and side
		make_pillar(j, i, arr + arrayOffset, tex_coords + arrayOffset);
		arrayOffset += vertsPerRectPrism;
    if(j < MAZEWIDTH)
		  make_horizontal_wall(j, i, arr + arrayOffset, tex_coords + arrayOffset);
    arrayOffset += vertsPerRectPrism;
  }

	// And... in theory it's done
	// Several hours later, in theory, it's slightly more done
	// Several hours later, in theory, it should be showing up (course, it isn't)
	// About an hour later... it actually is showing up. Jesus, I feel smart as hell
}

void init(void)
{
		// This is all stuff stolen from lab5
		int width = 800;
		int height = 800;
		GLubyte my_texels[width][height][3];

		FILE *fp = fopen("images/p2texture04.raw", "r");
		fread(my_texels, width * height * 3, 1, fp);
		fclose(fp);

		GLuint mytex[1];
		glGenTextures(1, mytex);
		glBindTexture(GL_TEXTURE_2D, mytex[0]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, my_texels);
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

		int param;
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &param);

		// Other stuff I don't really understand
		GLuint program = initShader("vshader.glsl", "fshader.glsl");
    glUseProgram(program);

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

		vec4 vertices[num_vertices];
   	generate_vertices(vertices);

    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) + sizeof(tex_coords), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices), sizeof(tex_coords), tex_coords);

		model_view_location = glGetUniformLocation(program,"model_view_matrix");
		projection_location = glGetUniformLocation(program,"projection_matrix");

    GLuint vPosition = glGetAttribLocation(program, "vPosition");
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

    GLuint vTexCoord = glGetAttribLocation(program, "vTexCoord");
    glEnableVertexAttribArray(vTexCoord);
    glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid *) 0 + (sizeof(vertices)));

		GLuint texture_location = glGetUniformLocation(program, "texture");
    glUniform1i(texture_location, 0);

    glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
    glClearColor(0.0, 0.0, 1.0, 1.0);
    glDepthRange(1,0);
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



    glPolygonMode(GL_FRONT, GL_FILL);
    glPolygonMode(GL_BACK, GL_LINE);

	  glUniformMatrix4fv(model_view_location, 1, GL_FALSE, (GLfloat *) &view_ctm);
		glUniformMatrix4fv(projection_location, 1, GL_FALSE, (GLfloat *) &proj_ctm);
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
	angle += 0.005;

	float r = groundtoprightx / 2;
	vec4 eye = {r + r * sin(angle), 10, r + -1 * r * cos(angle)};
	vec4 at = {groundtoprightx / 2, 0, groundtoprightz / 2};
	vec4 up = {0, 1, 0};
	view_ctm = look_at_v(eye, at, up);
	proj_ctm = perspective(PI / 2, 1, -1, -100);

  glutPostRedisplay();
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
		glEnable(GL_DEPTH_TEST);
    glutInitWindowSize(512, 512);
    glutInitWindowPosition(100,100);
    glutCreateWindow("Griffin Lynch's Project 2");
    glewInit();
    init();

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
	  glutIdleFunc(idle);

    

    glutMainLoop();

    return 0;
}
