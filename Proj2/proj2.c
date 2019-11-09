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

// Animation vars
int maze[MAZEWIDTH * MAZEHEIGHT];
int phase = 0;
float angle = 0;
int spinningOrMoving = 0; // 0 is spinning 1 is moving
vec4 oldPlace = {0, 0, 0, 0};
vec4 newPlace = {0, 0, 0, 0};
int oldAngle = 0;
int newAngle = 0;
int justTurned = 0;

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
	make_maze(maze, MAZEWIDTH, MAZEHEIGHT);
  print_maze(maze, MAZEWIDTH, MAZEHEIGHT);
  print_gross_maze(maze, MAZEWIDTH, MAZEHEIGHT);
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
  	proj_ctm = perspective(PI / 2, 1, -1, -100);

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

// Change coordinates from maze indices to world coordinates
vec4 maze_world_convert(vec4 maze_vec){
  float normOffset = groundOffset + pillarThick + wallLen/2;
  maze_vec.x = normOffset + maze_vec.x * (pillarThick + wallLen);
  maze_vec.y = wallHeight / 2;
  maze_vec.z = normOffset + maze_vec.z * (pillarThick + wallLen);
  return maze_vec;
}

vec4 world_maze_convert(vec4 world_vec){
  float normOffset = groundOffset + pillarThick + wallLen/2;
  world_vec.x = (world_vec.x - normOffset) / (pillarThick + wallLen);
  world_vec.y = wallHeight / 2;
  world_vec.z = (world_vec.z - normOffset) / (pillarThick + wallLen);
  return world_vec;
}

// Figure out the location of a spot directly in front of a given vector
//    in front of being defined by a given angle
vec4 derive_forward(vec4 pos, float angle){
  vec4 dir_vec = {sinf(angle), 0, cosf(angle), 0};
  return vector_add(pos, dir_vec);
}

// Based on angle, determine whether to adjust forward or to rotate
void update_view(){
  // In the case that we're spinning,
  if(spinningOrMoving == 0){
    float change = (newAngle - oldAngle) * PI / 2;
    //if(change == 3 * PI / 2) change = -PI / 2; // Turning left
    //if(change == -3 * PI / 2) change = PI / 2; // Turning right

    vec4 eye = oldPlace;
  	vec4 at = derive_forward(oldPlace, oldAngle * (PI / 2) + angle * change);
  	vec4 up = {0, 1, 0};
  	view_ctm = look_at_v(eye, at, up);
  }
  // Otherwise, we're moving... duh
  else{
    vec4 moveVec = vector_sub(newPlace, oldPlace);
    vec4 shortened = scalar_vector_multiply(angle, moveVec);
    vec4 resultant = vector_add(oldPlace, shortened);

    vec4 eye = resultant;
  	vec4 at = derive_forward(resultant, oldAngle * (PI / 2));
  	vec4 up = {0, 1, 0};
  	view_ctm = look_at_v(eye, at, up);
  }
}

// Determines which way to go depending on the spun orientation
void calc_spun(int forward, int left){
    // Okay, so you're sitting at <oldPlace.x, oldPlace.z> looking at an angle of oldAngle
    oldPlace = world_maze_convert(oldPlace);
    int block = maze[MAZEWIDTH * (int)oldPlace.z + (int)oldPlace.x];
    vector_print(oldPlace);
    oldPlace = maze_world_convert(oldPlace);

    printf("Block is %d\n", block);
    printf("And forward is %d\n", forward);

    if(block % left == 0 && justTurned == 0){ // Prefer turning left
      printf("Turning left!\n");
      spinningOrMoving = 0; // We're spinning now
      newAngle = (oldAngle + 1);
      justTurned = 1;
      return;
    } // Else if you can move forward
    else if(block % forward == 0){
      // We're Moving now
      spinningOrMoving = 1;
      justTurned = 0;
      switch(forward){
        case 2: // Forward is backwards
          newPlace = world_maze_convert(newPlace);
          newPlace.z = newPlace.z - 1;
          newPlace = maze_world_convert(newPlace);
          break;
        case 3: // Forward is forwards
          newPlace = world_maze_convert(newPlace);
          newPlace.z = newPlace.z + 1;
          newPlace = maze_world_convert(newPlace);
          break;
        case 7: // Forward is looking right
          newPlace = world_maze_convert(newPlace);
          newPlace.x = newPlace.x - 1;
          newPlace = maze_world_convert(newPlace);
          break;
        case 5: // Otherwise we're looking left
          newPlace = world_maze_convert(newPlace);
          newPlace.x = newPlace.x + 1;
          newPlace = maze_world_convert(newPlace);
          break;
      }
      printf("Walking forwards to:\n");
      vector_print(newPlace);
      return;
    }
    else{ // Otherwise turn right
      printf("Turning right!\n");
      spinningOrMoving = 0; // We're spinning now
      newAngle = oldAngle - 1;
      //if(newAngle == 2 * PI) newAngle = 0;
      justTurned = 1;
      return;
    } // Else if you can move forward

}

// Figure out what to do next
void calc_next_move(){
  printf("Facing direction: %d\n", oldAngle);
  if(oldAngle == -1) oldAngle = 3;
  if(oldAngle == 4) oldAngle = 0;

  if(oldAngle == 0) // Looking directly forward
    return calc_spun(3, 5);
  else if(oldAngle == 1) // Looking to the left
    return calc_spun(5, 2);
  else if(oldAngle == 2) // Looking back
    return calc_spun(2, 7);
  else if(oldAngle == 3) // Looking to the right
    return calc_spun(7, 3);
}

void idle(void){
  // Part 1: The Spinning works
  if(phase == 0){
	   angle += 0.01;

  	float r = groundtoprightx / 2;
  	vec4 eye = {r + r * sin(angle), 10, r + -1 * r * cos(angle)};
  	vec4 at = {groundtoprightx / 2, 0, groundtoprightz / 2};
  	vec4 up = {0, 1, 0};
  	view_ctm = look_at_v(eye, at, up);

    if(angle > 4 * PI){
      angle = 0;
      phase = 1;
      spinningOrMoving = 1;
      oldPlace = (vec4){groundtoprightx / 2, 10, 0, 0};
      //newPlace = maze_world_convert((vec4){0, 0, 0, 0});
      newPlace = maze_world_convert((vec4){0, 0, 0, 0});
    }
  } // Part 2: Navigate the maze
  else if(phase == 1){
    angle += 0.01;

    // If we're out of the maze
    vec4 scaledOld = world_maze_convert(oldPlace);
    if(scaledOld.x == -1){
      angle = 0;
      newAngle = 1;
    }
    else{
      // If we've completed the animation, figure out the next one
      if(angle >= 1){
        angle = 0;
        oldPlace = newPlace;
        oldAngle = newAngle;
        printf("\nFiguring out the next thing\n");
        calc_next_move();
      } // Otherwise, update the view with the animation
      else{
        update_view();
      }
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
    glutCreateWindow("Griffin Lynch's Project 2");
    glewInit();
    init();

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
	  glutIdleFunc(idle);

    glutMainLoop();

    return 0;
}
