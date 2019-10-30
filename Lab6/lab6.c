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

// Technically width and height might be any measure
// However, we're just making an us facing face, and then rotating
void make_face(vec4 com, float width, float height, vec4 *faceLoc){
  vec4 topleft = {com.x - width / 2.0f, com.x + height / 2.0f, 0, 1.0f};
  vec4 topright = {com.x + width / 2.0f, com.x + height / 2.0f, 0, 1.0f};
  vec4 bottomleft = {com.x - width / 2.0f, com.x - height / 2.0f, 0, 1.0f};
  vec4 bottomright = {com.x + width / 2.0f, com.x - height / 2.0f, 0, 1.0f};

  faceLoc[0] = bottomright;
  faceLoc[1] = topright;
  faceLoc[2] = bottomleft;
  faceLoc[3] = topleft;
  faceLoc[4] = bottomleft;
  faceLoc[5] = topright;
}

// Yes, I know that passing rotMat is technically redundant since it's just build_mat
// But that's not what I'm gonna say when I'm debugging this monstrosity later
void rotate_face(vec4 *faceLoc, mat4 rotMat){
  int i;
  for(i = 0; i < num_verts_per_face; i++){
    faceLoc[i] = matrix_vector_multiply(rotMat, faceLoc[i]);
  }
}

// Yeah, the next three functions could probably be combined somehow
//   This is a nice compromise between readability and brevity, I feel
void make_xy_face(vec4 com, float width, float height, vec4 *faceLoc, int flip){
  make_face(com, width, height, faceLoc);

  if(flip == -1){
    build_mat = rotate_y(PI);
    rotate_face(faceLoc, build_mat);
  }
}

void make_yz_face(vec4 com, float height, float depth, vec4 *faceLoc, int flip){
  make_face(com, depth, height, faceLoc);

  if(flip == -1){
    build_mat = rotate_y(3.0f * PI / 2.0f);
    rotate_face(faceLoc, build_mat);
  }
  else{
    build_mat = rotate_y(PI / 2.0f);
    rotate_face(faceLoc, build_mat);
  }
}

void make_xz_face(vec4 com, float width, float depth, vec4 *faceLoc, int flip){
  make_face(com, width, depth, faceLoc);

  if(flip == -1){
    build_mat = rotate_x(3.0f * PI / 2.0f);
    rotate_face(faceLoc, build_mat);
  }
  else{
    build_mat = rotate_x(PI / 2.0f);
    rotate_face(faceLoc, build_mat);
  }
}

// Height along the y, width along the x, depth along the z
// Center of cube is at the given center of mass vector
// Write the vertices to prismMem, and the colors to colorMem
void make_rectangular_prism(float width, float height, float depth, vec4 com, vec4 *prismMem, vec4 *colorMem){
  int i = 0;
  int sliding_i = 0;

  // Make the front and the back
  vec4 depthvec = {0, 0, depth / 2.0f, 0};
  make_xy_face(vector_sub(com, depthvec), width, height, prismMem + 0 * num_verts_per_face, 1);
  for(i = 0; i < 6; i++){
    colorMem[sliding_i] = RED;
    sliding_i++;
  }
  make_xy_face(vector_add(com, depthvec), width, height, prismMem + 1 * num_verts_per_face, -1);
  for(i = 0; i < 6; i++){
    colorMem[sliding_i] = PINK;
    sliding_i++;
  }

  // Make the right and the left
  vec4 widthvec = {width / 2.0f, 0, 0, 0};
  make_yz_face(vector_add(com, widthvec), height, depth, prismMem + 2 * num_verts_per_face, 1);
  for(i = 0; i < 6; i++){
    colorMem[sliding_i] = YELLOW;
    sliding_i++;
  }
  make_yz_face(vector_sub(com, widthvec), height, depth, prismMem + 3 * num_verts_per_face, -1);
  for(i = 0; i < 6; i++){
    colorMem[sliding_i] = CYAN;
    sliding_i++;
  }

  // Make the top and the bottom
  vec4 heightvec = {0, height / 2.0f, 0, 0};
  make_xz_face(vector_add(com, heightvec), width, depth, prismMem + 4 * num_verts_per_face, 1);
  for(i = 0; i < 6; i++){
    colorMem[sliding_i] = BLUE;
    sliding_i++;
  }
  make_xz_face(vector_sub(com, heightvec), width, depth, prismMem + 5 * num_verts_per_face, -1);
  for(i = 0; i < 6; i++){
    colorMem[sliding_i] = GREEN;
    sliding_i++;
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
  float angle = 0.005;

  twin_cube_ctm = matrix_matrix_multiply(rotate_y(angle), twin_cube_ctm);
  spinner_cube_ctm = matrix_matrix_multiply(rotate_z(angle), spinner_cube_ctm);
  flipper_cube_ctm = matrix_matrix_multiply(rotate_x(angle), flipper_cube_ctm);

	glutPostRedisplay();
}

void print_verts(){
  int i;
  vec4 myvec;
  for(i = 0; i < num_vertices; i++){
    myvec = vertices[i];
    printf("{%.2f, %.2f, %.2f}\n", myvec.x, myvec.y, myvec.z);
  }
}

int main(int argc, char **argv)
{
    // Set up the four cubes
    make_rectangular_prism(0.5, 0.5, 0.5, {-0.5, 0.5, 0, 0}, vertices + 0 * num_verts_per_rectangular_prism, colors + 0 * num_colors_per_rectangular_prism);
    make_rectangular_prism(0.5, 0.5, 0.5, {0.5, 0.5, 0, 0}, vertices + 1 * num_verts_per_rectangular_prism, colors + 1 * num_colors_per_rectangular_prism);
    make_rectangular_prism(0.5, 0.5, 0.5, {-0.5, -0.5, 0, 0}, vertices + 2 * num_verts_per_rectangular_prism, colors + 2 * num_colors_per_rectangular_prism);
    make_rectangular_prism(0.5, 0.5, 0.5, {0.5, -0.5, 0, 0}, vertices + 3 * num_verts_per_rectangular_prism, colors + 3 * num_colors_per_rectangular_prism);

    print_verts();

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
