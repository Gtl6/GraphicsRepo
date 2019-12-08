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
void easy_update_vecVal(faces face, faces expected_one, faces expected_other int one_or_other float *placer){
  float near = 0.1;
  float far = 0.11;
  int err = 0;
  if(one_or_other == 0){
    if(face == expected_one){
      *placer = -far;
    } else if (face == expected_other) {
      err = 1;
    } else {
      *placer = -near;
    }
  }
  else if(one_or_other == 1){
    if(face == expected_other){
      result.z = far;
    } else if (face == expected_one) {
      err = 1;
    } else {
      result.z = near;
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
  vertices[10] = get_points(1, 1, 1, FRONT);
  vertices[11] = get_points(1, 0, 1, FRONT);
  // Now left
  vertices[12] = get_points(0, 0, 0, LEFT);
  vertices[13] = get_points(0, 0, 1, LEFT);
  vertices[14] = get_points(0, 1, 0, LEFT);
  vertices[15] = get_points(0, 0, 0, LEFT);
  vertices[16] = get_points(0, 1, 0, LEFT);
  vertices[17] = get_points(0, 0, 1, LEFT);
  // And right
  vertices[18] = get_points(1, 0, 0, RIGHT);
  vertices[19] = get_points(1, 0, 1, RIGHT);
  vertices[20] = get_points(1, 1, 1, RIGHT);
  vertices[21] = get_points(1, 0, 0, RIGHT);
  vertices[22] = get_points(1, 1, 1, RIGHT);
  vertices[23] = get_points(1, 1, 1, RIGHT);
  // Bottom
  vertices[24] = get_points(0, 0, 0, BOTTOM);
  vertices[25] = get_points(1, 0, 1, BOTTOM);
  vertices[26] = get_points(1, 0, 0, BOTTOM);
  vertices[27] = get_points(0, 0, 0, BOTTOM);
  vertices[28] = get_points(0, 0, 1, BOTTOM);
  vertices[29] = get_points(1, 0, 1, BOTTOM);
  // Top
  vertices[30] = get_points(0, 1, 0, TOP);
  vertices[31] = get_points(1, 1, 0, TOP);
  vertices[32] = get_points(1, 1, 1, TOP);
  vertices[33] = get_points(0, 1, 0, TOP);
  vertices[34] = get_points(1, 1, 1, TOP);
  vertices[35] = get_points(0, 1, 1, TOP);
  // Now comes the tricky part - the angled surfaces
  // Start off with the one above the back
  vertices[36] = get_points(0, 1, 0, BACK);
  vertices[37] = get_points(1, 1, 0, BACK);
  vertices[38] = get_points(1, 1, 0, TOP);
  vertices[39] = get_points(0, 1, 0, BACK);
  vertices[40] = get_points(1, 1, 0, TOP);
  vertices[41] = get_points(0, 1, 0, TOP);
  // Follow it forward to the one behind the top
  vertices[42] = get_points(0, 1, 1, TOP);
  vertices[43] = get_points(1, 1, 1, TOP);
  vertices[44] = get_points(1, 1, 1, FRONT);
  vertices[45] = get_points(0, 1, 1, TOP);
  vertices[46] = get_points(1, 1, 1, FRONT);
  vertices[47] = get_points(0, 1, 1, FRONT);
  // Keep wrapping around to front bottom
  vertices[48] = get_points(0, 0, 1, FRONT);
  vertices[49] = get_points(1, 0, 1, FRONT);
  vertices[50] = get_points(1, 0, 1, BOTTOM);
  vertices[51] = get_points(0, 0, 1, FRONT);
  vertices[52] = get_points(1, 0, 1, BOTTOM);
  vertices[53] = get_points(0, 0, 1, BOTTOM);
  // Now the bottom and back ones
  vertices[54] = get_points(0, 0, 0, BOTTOM);
  vertices[55] = get_points(1, 0, 0, BOTTOM);
  vertices[56] = get_points(1, 0, 0, BACK);
  vertices[57] = get_points(0, 0, 0, BOTTOM);
  vertices[58] = get_points(1, 0, 0, BACK);
  vertices[59] = get_points(0, 0, 0, BACK);
  //Now the top right one
  vertices[60] = get_points(1, 1, 0, RIGHT);
  vertices[61] = get_points(1, 1, 1, RIGHT);
  vertices[62] = get_points(1, 1, 1, TOP);
  vertices[63] = get_points(1, 1, 0, RIGHT);
  vertices[64] = get_points(1, 1, 1, TOP);
  vertices[65] = get_points(1, 1, 0, TOP);
  // Top left
  vertices[66] = get_points(0, 1, 1, LEFT);
  vertices[67] = get_points(0, 1, 0, LEFT);
  vertices[68] = get_points(0, 1, 0, TOP);
  vertices[69] = get_points(0, 1, 1, LEFT);
  vertices[70] = get_points(0, 1, 0, TOP);
  vertices[71] = get_points(0, 1, 1, TOP);
  // bottom right
  vertices[72] = get_points(1, 0, 0, BOTTOM);
  vertices[73] = get_points(1, 0, 1, BOTTOM);
  vertices[74] = get_points(1, 0, 1, RIGHT);
  vertices[75] = get_points(1, 0, 0, BOTTOM);
  vertices[76] = get_points(1, 0, 1, RIGHT);
  vertices[77] = get_points(1, 0, 0, RIGHT);
  // bottom left
  vertices[78] = get_points(0, 0, 1, BOTTOM);
  vertices[79] = get_points(0, 0, 0, BOTTOM);
  vertices[80] = get_points(0, 0, 0, LEFT);
  vertices[81] = get_points(0, 0, 1, BOTTOM);
  vertices[82] = get_points(0, 0, 0, LEFT);
  vertices[83] = get_points(0, 0, 1, LEFT);
  // front right
  vertices[84] = get_points(1, 0, 1, RIGHT);
  vertices[85] = get_points(1, 0, 1, FRONT);
  vertices[86] = get_points(1, 1, 1, FRONT);
  vertices[87] = get_points(1, 1, 1, RIGHT);
  vertices[88] = get_points(1, 1, 1, FRONT);
  vertices[89] = get_points(1, 1, 1, RIGHT);
  // front left
  vertices[90] = get_points(0, 0, 1, FRONT);
  vertices[91] = get_points(0, 0, 1, LEFT);
  vertices[92] = get_points(0, 1, 1, LEFT);
  vertices[93] = get_points(0, 0, 1, FRONT);
  vertices[94] = get_points(0, 1, 1, LEFT);
  vertices[95] = get_points(0, 1, 1, FRONT);
  // back right
  vertices[96] = get_points(1, 0, 0, BACK);
  vertices[97] = get_points(1, 0, 0, RIGHT);
  vertices[98] = get_points(1, 1, 0, RIGHT);
  vertices[99] = get_points(1, 1, 0, BACK);
  vertices[100] = get_points(1, 1, 0, RIGHT);
  vertices[101] = get_points(1, 1, 0, BACK);
  // back left
  vertices[102] = get_points(0, 0, 0, LEFT);
  vertices[103] = get_points(0, 0, 0, BACK);
  vertices[104] = get_points(0, 1, 0, BACK);
  vertices[105] = get_points(0, 0, 0, LEFT);
  vertices[106] = get_points(0, 1, 0, BACK);
  vertices[107] = get_points(0, 1, 0, LEFT);
  // Close to done! Let's not forget the triangles!
  // These are actually going to be pretty easy, cause it's the same coord
  // Start w/ top right, on the back
  vertices[108] = get_points(1, 1, 0, BACK);
  vertices[109] = get_points(1, 1, 0, RIGHT);
  vertices[110] = get_points(1, 1, 0, TOP);
  // Top left back
  vertices[111] = get_points(0, 1, 0, LEFT);
  vertices[112] = get_points(0, 1, 0, BACK);
  vertices[113] = get_points(0, 1, 0, TOP);
  // Top right front
  vertices[114] = get_points(1, 1, 1, FRONT);
  vertices[115] = get_points(1, 1, 1, TOP);
  vertices[116] = get_points(1, 1, 1, RIGHT);
  // Top left front
  vertices[117] = get_points(0, 1, 1, FRONT);
  vertices[118] = get_points(0, 1, 1, TOP);
  vertices[119] = get_points(0, 1, 1, LEFT);
  // Halfway done. Keep the faith, bud
  // bottom left back
  vertices[120] = get_points(0, 0, 0, BACK);
  vertices[121] = get_points(0, 0, 0, LEFT);
  vertices[122] = get_points(0, 0, 0, BOTTOM);
  // bottom right back
  vertices[123] = get_points(1, 0, 0, BACK);
  vertices[124] = get_points(1, 0, 0, BOTTOM);
  vertices[125] = get_points(1, 0, 0, RIGHT);
  //bottom left front
  vertices[126] = get_points(0, 0, 1, FRONT);
  vertices[127] = get_points(0, 0, 1, BOTTOM);
  vertices[128] = get_points(0, 0, 1, LEFT);
  // bottom right front
  vertices[129] = get_points(1, 0, 1, FRONT);
  vertices[130] = get_points(1, 0, 1, RIGHT);
  vertices[131] = get_points(1, 0, 1, BOTTOM);

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
    colors[i] = {0, 0, 0, 1};
  }
}

// Generate one cubie centered at the origin
void generate_vertices_and_colors(vec4 *vertices, vec4 *colors){
  int i, j;
  for(i = 0; i < 27; i++){
    int x_pos = i % 3;
    int z_pos = (i % 9) / 3;
    int y_pos = i / 9;
    generate_cubie(vertices[i * VERTS_PER_CUBIE], vertices[i * VERTS_PER_CUBIE]);
    // TODO: generate transform for this cube and also attach it to the internal model somehow
  }
}

// Need to generate the normals for when we're looking at the cube
void generate_normals(vec4 *vertices, vec4 *normals){
  // Literally all of these are triangles, sooooo...
  int i;
  vec4 v1, v2, v3;
  for(i = 0; i < num_vertices; i++){
    vec4 norm = normalize(cross_product())
  }
}

//Generate vertices and stuff
void init(void)
{
		GLuint program = initShader("vshader.glsl", "fshader.glsl");
    glUseProgram(program);

    // Tell OpenGL where the shared vars are
    ctm_location = glGetUniformLocation(program, "ctm");

    // Load in all our data straightup
    vec4 vertices[num_vertices];
    vec4 colors[num_vertices];
    vec4 normals[num_vertices];
		generate_vertices_and_colors(vertices, colors);
    generate_normals(vertices, normals);

    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

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
}

// Draw handler
void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPolygonMode(GL_FRONT, GL_FILL);
    glPolygonMode(GL_BACK, GL_LINE);

    // Let's draw some cubies!
    int i;
    for(i = 0; i < 27; i++){
      glUniform1fv(ctm_location, 1, (GLfloat *) &ctm);
      glDrawArrays(GL_TRIANGLES, 0, num_vertices);
    }

    glutSwapBuffers();
}

void keyboard(unsigned char key, int mousex, int mousey)
{
    if(key == 'q'){
    	exit(0);
    }

    // glutPostRedisplay();
}

// Tick handler
void idle(void){
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
    glutKeyboardFunc(keyboard);
	  glutIdleFunc(idle);

    glutMainLoop();

    return 0;
}
