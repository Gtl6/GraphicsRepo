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
#define SPHERE_RESOLUTION 16

const float radius = 0.5;

#define VERTS_PER_RECTANGLE 6
#define VERTS_PER_ROW (SPHERE_RESOLUTION * VERTS_PER_RECTANGLE)
#define VERTS_PER_SPHERE (SPHERE_RESOLUTION * VERTS_PER_ROW)
#define NUM_SPHERES 6
// The verts for one sphere and the floor
#define num_vertices (VERTS_PER_SPHERE + VERTS_PER_RECTANGLE)

typedef struct
{
    GLfloat x;
    GLfloat y;
} vec2;

// The material model that represents the material of the object
typedef struct
{
  vec4 reflect_ambient;
  vec4 reflect_diffuse;
  vec4 reflect_specular;
  GLfloat shininess;
} material;

// The different balls' materials
material ball_materials[5] = {
{{1.0, 0.0, 0.0, 1.0}, {1.0, 0.0, 0.0, 1.0}, {1.0, 1.0, 1.0, 1.0}, 10},
{{0.0, 1.0, 0.0, 1.0}, {0.0, 1.0, 0.0, 1.0}, {1.0, 1.0, 1.0, 1.0}, 10},
{{0.0, 0.0, 1.0, 1.0}, {0.0, 0.0, 1.0, 1.0}, {1.0, 1.0, 1.0, 1.0}, 10},
{{1.0, 1.0, 0.0, 1.0}, {1.0, 1.0, 0.0, 1.0}, {1.0, 1.0, 1.0, 1.0}, 10},
{{1.0, 0.5, 0.0, 1.0}, {1.0, 0.5, 0.0, 1.0}, {1.0, 1.0, 1.0, 1.0}, 10}};

// Need materials for the other stuff too
material table_material = {{0.1, 0.5, 0.0, 1.0}, {0.0, 1.0, 0.0, 1.0}, {1.0, 1.0, 1.0, 1.0}, 10};
material light_material = {{1.0, 1.0, 1.0, 1.0}, {1.0, 1.0, 1.0, 1.0}, {1.0, 1.0, 1.0, 1.0}, 10};

// Properties of our point light
// Position is defined below
vec4 light_ambient = {0.5, 0.5, 0.5, 1.0};
vec4 light_diffuse = {1, 1, 1, 1};
vec4 light_specular = {1, 1, 1, 1};

// The shader variables
mat4 ctm;
mat4 model_view;
mat4 projection;
vec4 ambient_product;
vec4 diffuse_product;
vec4 specular_product;
vec4 light_position;
GLfloat attenuation_constant;
GLfloat attenuation_linear;
GLfloat attenuation_quadratic;
GLfloat shininess;
// The locations of the shader variables
GLuint ctm_location;
GLuint model_view_location;
GLuint projection_location;
GLuint isShadow_location;
GLuint ap_location;
GLuint dp_location;
GLuint sp_location;
GLuint light_position_location;
GLuint ac_location;
GLuint al_location;
GLuint aq_location;
GLuint shininess_location;

// Local vars we won't need to pass
float angle;
vec4 eye;
vec4 at;
vec4 up;
float theta = 0;
float phi = 0;
float view_rad = 5;

// Makes a sphere of radius radius (as specified above) at the origin
void generate_sphere(vec4 *arr){
	float vert_angle = PI / (SPHERE_RESOLUTION);
	int i, j;
	float angle1, angle2, y1, y2, xrad1, xrad2, xangle1, xangle2, xcoord, zcoord;
	int index = 0;
	vec4 zed = {0, 0, 0, 0};

	for(i = 0; i < SPHERE_RESOLUTION; i++){
		angle1 = i * vert_angle;
		angle2 = (i + 1) * vert_angle;
		y1 = radius * cosf(angle1);
		y2 = radius * cosf(angle2);
		xrad1 = radius * sinf(angle1);
		xrad2 = radius * sinf(angle2);
		for(j = 0; j < SPHERE_RESOLUTION; j++){
			xangle1 = j * vert_angle;
			xangle2 = (j + 1) * vert_angle;
			vec4 bl = {xrad2 * cosf(2 * xangle1), y2, xrad2 * sinf(2 * xangle1), 1};
			vec4 br = {xrad2 * cosf(2 * xangle2), y2, xrad2 * sinf(2 * xangle2), 1};
			vec4 tl = {xrad1 * cosf(2 * xangle1), y1, xrad1 * sinf(2 * xangle1), 1};
			vec4 tr = {xrad1 * cosf(2 * xangle2), y1, xrad1 * sinf(2 * xangle2), 1};

			// Most of the time do both of these, but at the top and bottom we only want one
			if(y2 != -1 * radius){
				arr[index++] = bl;
				arr[index++] = tr;
				arr[index++] = br;
			}
      else{
        index += 3;
      }

			if(y1 != 1 * radius){
				arr[index++] = bl;
				arr[index++] = tl;
				arr[index++] = tr;
			}
      else{
        index += 3;
      }
		}
	}
}

// Builds the world
void generate_vertices(vec4 *arr){
  // First let's build out the floor
  vec4 bl = {0, 0, 0, 1};
	vec4 br = {20, 0, 0, 1};
	vec4 tl = {0, 0, 20, 1};
	vec4 tr = {20, 0, 20, 1};

	arr[0] = br;
	arr[1] = bl;
	arr[2] = tr;
	arr[3] = bl;
	arr[4] = tl;
	arr[5] = tr;

  // Stick in a sphere
  generate_sphere(arr + 6);
}

// Figure out the normal for each face
void generate_normals(vec4 *vertices, vec4 *norms){
  norms[0] = (vec4){0, 1, 0, 0};
  norms[1] = (vec4){0, 1, 0, 0};
  norms[2] = (vec4){0, 1, 0, 0};
  norms[3] = (vec4){0, 1, 0, 0};
  norms[4] = (vec4){0, 1, 0, 0};
  norms[5] = (vec4){0, 1, 0, 0};

  int i;
  for(i = 6; i < num_vertices; i++){
    // Since it's a sphere, the normal vector is just the vector itself
    norms[i] = normalize(vertices[i]);
  }
}

//Generate vertices and stuff
void init(void)
{
		// Setting up local variables
    angle = 0;
    light_position = (vec4){10, 3, 10, 1};
  	projection = perspective(PI / 2, 1, -1, -100);

		GLuint program = initShader("vshader.glsl", "fshader.glsl");
    glUseProgram(program);

    // I really don't have any idea what to set these to
    attenuation_constant = 1;
    attenuation_linear = 0.1;
    attenuation_quadratic = 0.1;

    float distance = 2;
    float attenuation = 1/(attenuation_constant + (attenuation_linear * distance) +
    (attenuation_quadratic * distance * distance));

    printf("Attenuation is roughly: %f\n", attenuation);

    // Tell OpenGL where the shared vars are
    ctm_location = glGetUniformLocation(program, "ctm");
    model_view_location = glGetUniformLocation(program, "model_view");
    projection_location = glGetUniformLocation(program, "projection");
    isShadow_location = glGetUniformLocation(program, "isShadow");
    ap_location = glGetUniformLocation(program, "ambient_product");
    dp_location = glGetUniformLocation(program, "diffuse_product");
    sp_location = glGetUniformLocation(program, "specular_product");
    light_position_location = glGetUniformLocation(program, "light_position");
    ac_location = glGetUniformLocation(program, "attenuation_constant");
    al_location = glGetUniformLocation(program, "attenuation_linear");
    aq_location = glGetUniformLocation(program, "attenuation_quadratic");
    shininess_location = glGetUniformLocation(program, "shininess");

    // I can actually just straight up send these right now
    glUniform1fv(ac_location, 1, (GLfloat *) &attenuation_constant);
    glUniform1fv(al_location, 1, (GLfloat *) &attenuation_linear);
    glUniform1fv(aq_location, 1, (GLfloat *) &attenuation_quadratic);

    vec4 vertices[num_vertices];
		vec4 normals[num_vertices];
		generate_vertices(vertices);
		generate_normals(vertices, normals);

    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) + sizeof(normals), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices), sizeof(normals), normals);

    GLuint vPosition = glGetAttribLocation(program, "vPosition");
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

    GLuint vNormal = glGetAttribLocation(program, "vNormal");
    glEnableVertexAttribArray(vNormal);
    glVertexAttribPointer(vNormal, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid *) sizeof(vertices));

    glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
    glClearColor(0.5, 0.1, 0.0, 1.0);
    glDepthRange(1,0);
}

// Just a simple vector operation
vec4 product(vec4 u, vec4 v)
{
  vec4 result;
  result.x = u.x * v.x;
  result.y = u.y * v.y;
  result.z = u.z * v.z;
  result.w = u.w * v.w;
  return result;
}

// Draw handler
void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPolygonMode(GL_FRONT, GL_FILL);
    glPolygonMode(GL_BACK, GL_LINE);

    // Update the camera
    eye = (vec4){10 + view_rad * cosf(theta), 1 + view_rad * sin(phi), 10 + view_rad * sinf(theta), 1};
  	at = (vec4){10, 0, 10, 1};
  	up = (vec4){0, 1, 0, 1};
  	model_view = (mat4)look_at_v(eye, at, up);

    ctm = identity_matrix();

    glUniformMatrix4fv(model_view_location, 1, GL_FALSE, (GLfloat *) &model_view);
    glUniformMatrix4fv(projection_location, 1, GL_FALSE, (GLfloat *) &projection);
    glUniform4fv(light_position_location, 1, (GLfloat *) &light_position);

    // First draw the table
    glUniform1i(isShadow_location, 0);
    glUniformMatrix4fv(ctm_location, 1, GL_FALSE, (GLfloat *) &ctm);
    ambient_product = product(light_ambient, table_material.reflect_ambient);
    diffuse_product = product(light_diffuse, table_material.reflect_diffuse);
    specular_product = product(light_specular, table_material.reflect_specular);
    glUniform4fv(ap_location, 1, (GLfloat *) &ambient_product);
    glUniform4fv(dp_location, 1, (GLfloat *) &diffuse_product);
    glUniform4fv(sp_location, 1, (GLfloat *) &specular_product);
    glUniform1fv(shininess_location, 1, (GLfloat *) &table_material.shininess);
    glDrawArrays(GL_TRIANGLES, 0, VERTS_PER_RECTANGLE);

    // Set up ctm for light
    ctm = translate(light_position.x, light_position.y, light_position.z);
    ctm = matrix_matrix_multiply(ctm, scale(0.2, 0.2, 0.2));

    // Draw the light
    glUniform1i(isShadow_location, 0);
    glUniformMatrix4fv(ctm_location, 1, GL_FALSE, (GLfloat *) &ctm);
    ambient_product = product(light_ambient, light_material.reflect_ambient);
    diffuse_product = product(light_diffuse, light_material.reflect_diffuse);
    specular_product = product(light_specular, light_material.reflect_specular);
    glUniform4fv(ap_location, 1, (GLfloat *) &ambient_product);
    glUniform4fv(dp_location, 1, (GLfloat *) &diffuse_product);
    glUniform4fv(sp_location, 1, (GLfloat *) &specular_product);
    glUniform1fv(shininess_location, 1, (GLfloat *) &light_material.shininess);
    glDrawArrays(GL_TRIANGLES, VERTS_PER_RECTANGLE, VERTS_PER_SPHERE);

    // Then draw all of the balls
    int i;
    for(i = 0; i < 5; i++){
      // Set the location of the ball
      ctm = translate(10 + i * cosf(angle / (i + 1)), 0.5, 10 + i * sinf(angle / (i + 1)));

      glUniform1i(isShadow_location, 0);
      glUniformMatrix4fv(ctm_location, 1, GL_FALSE, (GLfloat *) &ctm);
      ambient_product = product(light_ambient, ball_materials[i].reflect_ambient);
      diffuse_product = product(light_diffuse, ball_materials[i].reflect_diffuse);
      specular_product = product(light_specular, ball_materials[i].reflect_specular);
      glUniform4fv(ap_location, 1, (GLfloat *) &ambient_product);
      glUniform4fv(dp_location, 1, (GLfloat *) &diffuse_product);
      glUniform4fv(sp_location, 1, (GLfloat *) &specular_product);
      glUniform1fv(shininess_location, 1, (GLfloat *) &ball_materials[i].shininess);
      glDrawArrays(GL_TRIANGLES, VERTS_PER_RECTANGLE, VERTS_PER_SPHERE);

      // And shadows
      glUniform1i(isShadow_location, 1);
      glDrawArrays(GL_TRIANGLES, VERTS_PER_RECTANGLE, VERTS_PER_SPHERE);
    }

    glutSwapBuffers();
}

void keyboard(unsigned char key, int mousex, int mousey)
{
    if(key == 'x')
    	exit(0);

    if(key == 'w'){
      if(phi < (PI / 2 - PI / 8)){
        phi += 1 / (5 * 2 * PI);
      }
    }

    if(key == 's'){
      if(phi > 0){
        phi -= 1 / (5 * 2 * PI);
      }
    }

    if(key == 'd'){
      theta = (theta + 1 / (5 * 2 * PI));
      if(theta > 2 * PI) theta -= 2 * PI;
    }

    if(key == 'a'){
      theta = (theta - 1 / (5 * 2 * PI));
      if(theta < 0) theta += 2 * PI;
    }

    if(key == 'q'){
      if(view_rad > 2) view_rad -= 0.2;
    }

    if(key == 'e'){
      if(view_rad < 10) view_rad += 0.2;
    }

    if(key == 'i'){
      if(light_position.z < 20) light_position.z += 0.2;
    }

    if(key == 'k'){
      if(light_position.z > 0) light_position.z -= 0.2;
    }

    if(key == 'j'){
      if(light_position.x < 20) light_position.x += 0.2;
    }

    if(key == 'l'){
      if(light_position.x > 0) light_position.x -= 0.2;
    }

    if(key == 'u'){
      if(light_position.y < 20) light_position.y += 0.2;
    }

    if(key == 'o'){
      if(light_position.y > 0) light_position.y -= 0.2;
    }

    glutPostRedisplay();
}

// Tick handler
void idle(void){
  angle += 1 / (5 * 2 * PI);
  glutPostRedisplay();
}


int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
		glEnable(GL_DEPTH_TEST);
    glutInitWindowSize(512, 512);
    glutInitWindowPosition(100,100);
    glutCreateWindow("Griffin Lynch's Project 3");
    glewInit();
    init();

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
	  glutIdleFunc(idle);

    glutMainLoop();

    return 0;
}
