/*
 * Lab6
 *
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

// Controlling the spinning from the top (rather than the bottom)
int run = 1;

const vec4 RED = {1, 0, 0, 0};
const vec4 GREEN = {0, 1, 0, 0};
const vec4 BLUE = {0, 0, 1, 0};
const vec4 PINK = {1, 0, 1, 0};
const vec4 CYAN = {0, 1, 1, 0};
const vec4 YELLOW = {1, 1, 0, 0};

// 6 verts per face times 6 faces per cube times 4 cubes
const int num_verts_per_face = 6;
const int num_verts_per_rectangular_prism = num_verts_per_face * 6;
const int num_vertices = num_verts_per_rectangular_prism * 4;
vec4 vertices[num_vertices];
// 2 colors per face times 6 faces per cube times 4 cubes
const int num_colors_per_rectangular_prism = 6 * 6;
const int num_colors = num_colors_per_rectangular_prism * 4;
vec4 colors[num_vertices];
GLuint ctm_location;
mat4 build_mat;
mat4 ctm;

// Our shapes
vec4 twin_cube_com = {0, 0.5f, 0, 1.0f};
mat4 twin_cube_ctm = rotate_y(0);
vec4 spinner_com = {-0.5f, -0.5f, 0, 1.0f};
mat4 spinner_cube_ctm = rotate_z(0);
vec4 flipper_com = {0.5, -0.5f, 0, 1.0f};
mat4 flipper_cube_ctm = rotate_x(0);



// Height along the y, width along the x, depth along the z
// Center of cube is at the given center of mass vector
// Write the vertices to prismMem, and the colors to colorMem
void make_rectangular_prism(float width, float height, float depth, vec4 *prismMem, vec4 *colorMem){
  vec4 rdb = {1, -1, 1, 1};
  vec4 rub = {1, 1, 1, 1};
  vec4 ldb = {-1, -1, 1, 1};
  vec4 lub = {-1, 1, 1, 1};
  vec4 rdf = {1, -1, -1, 1};
  vec4 ruf = {1, 1, -1, 1};
  vec4 ldf = {-1, -1, -1, 1};
  vec4 luf = {-1, 1, -1, 1};
  int i;

  width /= 2;
  height /= 2;
  depth /= 2;

  // Front
  prismMem[0] = rdb;
  prismMem[1] = rub;
  prismMem[2] = ldb;
  prismMem[3] = lub;
  prismMem[4] = ldb;
  prismMem[5] = rub;
  for(i = 0; i < 6; i++) colorMem[i] = RED;

  // Back
  // Just copied and pasted Front, so that's why the nums are weird
  prismMem[6] = rdf;
  prismMem[8] = ruf;
  prismMem[7] = ldf;
  prismMem[9] = luf;
  prismMem[11] = ldf;
  prismMem[10] = ruf;
  for(i = 6; i < 12; i++) colorMem[i] = BLUE;

  // Left
  prismMem[12] = ldb;
  prismMem[13] = lub;
  prismMem[14] = ldf;
  prismMem[15] = luf;
  prismMem[16] = ldf;
  prismMem[17] = lub;
  for(i = 12; i < 18; i++) colorMem[i] = PINK;

  // Right
  // Ditto to back
  prismMem[18] = rdb;
  prismMem[20] = rub;
  prismMem[19] = rdf;
  prismMem[21] = ruf;
  prismMem[23] = rdf;
  prismMem[22] = rub;
  for(i = 18; i < 24; i++) colorMem[i] = CYAN;

  // Top
  prismMem[24] = rub;
  prismMem[25] = ruf;
  prismMem[26] = lub;
  prismMem[27] = luf;
  prismMem[28] = lub;
  prismMem[29] = ruf;
  for(i = 24; i < 30; i++) colorMem[i] = YELLOW;

  // Bottom
  prismMem[30] = rdb;
  prismMem[32] = rdf;
  prismMem[31] = ldb;
  prismMem[33] = ldf;
  prismMem[35] = ldb;
  prismMem[34] = rdf;
  for(i = 30; i < 36; i++) colorMem[i] = GREEN;

  // Change the dimensions of the given rectangular prism to those specified
  mat4 changermat = scale(width, height, depth);

  for(i = 0; i < 36; i++){
    prismMem[i] = matrix_vector_multiply(changermat, prismMem[i]);
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

    // Render the Bro Cubes
	  glUniformMatrix4fv(ctm_location, 1, GL_FALSE, (GLfloat *) &twin_cube_ctm);
    glDrawArrays(GL_TRIANGLES, 0, num_verts_per_rectangular_prism * 2);

    // Render Captain Spinner
    glUniformMatrix4fv(ctm_location, 1, GL_FALSE, (GLfloat *) &spinner_cube_ctm);
    glDrawArrays(GL_TRIANGLES, 72, num_verts_per_rectangular_prism);

    // Render Chieftan Flip
    glUniformMatrix4fv(ctm_location, 1, GL_FALSE, (GLfloat *) &flipper_cube_ctm);
    glDrawArrays(GL_TRIANGLES, 108, num_verts_per_rectangular_prism);

    glutSwapBuffers();
}

void keyboard(unsigned char key, int mousex, int mousey)
{
    if(key == 'q')
    	exit(0);
}

void idle(void){
  if(run == 1){
    float angle = 0.005;

    twin_cube_ctm = matrix_matrix_multiply(rotate_y(angle), twin_cube_ctm);
    spinner_cube_ctm = matrix_matrix_multiply(rotate_local_z(angle, spinner_com), spinner_cube_ctm);
    flipper_cube_ctm = matrix_matrix_multiply(rotate_local_x(angle, flipper_com), flipper_cube_ctm);

	   glutPostRedisplay();
   }
}

void print_verts(){
  int i;
  vec4 myvec;
  for(i = 0; i < num_vertices; i++){
    myvec = vertices[i];
    printf("{%.2f, %.2f, %.2f}\n", myvec.x, myvec.y, myvec.z);
  }
}

void place_cube(float x, float y, vec4 *cube){
  mat4 mover = translate(x, y, 0);
  int i;
  for(i = 0; i < 36; i++){
    cube[i] = matrix_vector_multiply(mover, cube[i]);
  }
}

int main(int argc, char **argv)
{
    // Set up the four cubes
    make_rectangular_prism(0.5, 0.5, 0.5, vertices + 0 * num_verts_per_rectangular_prism, colors + 0 * num_colors_per_rectangular_prism);
    make_rectangular_prism(0.5, 0.5, 0.5, vertices + 1 * num_verts_per_rectangular_prism, colors + 1 * num_colors_per_rectangular_prism);
    make_rectangular_prism(0.5, 0.5, 0.5, vertices + 2 * num_verts_per_rectangular_prism, colors + 2 * num_colors_per_rectangular_prism);
    make_rectangular_prism(0.5, 0.5, 0.5, vertices + 3 * num_verts_per_rectangular_prism, colors + 3 * num_colors_per_rectangular_prism);

    // Place the cubes
    place_cube(0.5, 0.5, vertices + 0 * num_verts_per_rectangular_prism);
    place_cube(-0.5, 0.5, vertices + 1 * num_verts_per_rectangular_prism);
    place_cube(-0.5, -0.5, vertices + 2 * num_verts_per_rectangular_prism);
    place_cube(0.5, -0.5, vertices + 3 * num_verts_per_rectangular_prism);


    //print_verts();
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(512, 512);
    glutInitWindowPosition(100,100);
    glutCreateWindow("Lab 6");
    glewInit();
    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
	  glutIdleFunc(idle);
    glutMainLoop();

    return 0;
}
