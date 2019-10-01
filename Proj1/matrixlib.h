// Written by Griffin Lynch
#include <stdio.h>
#include <math.h>

typedef struct {
	float x;
	float y;
	float z;
	float w;
} vec4;

typedef struct {
	vec4 x;
	vec4 y;
	vec4 z;
	vec4 w;
} mat4;

// All basic functions
void vector_print(vec4 v);
vec4 scalar_vector_multiply(float s, vec4 v);
vec4 vector_add(vec4 v, vec4 v2);
vec4 vector_sub(vec4 v, vec4 v2);
float magnitude(vec4 v);
float dot_product(vec4 v, vec4 v2);
vec4 cross_product(vec4 v, vec4 v2);
vec4 normalize(vec4);

void matrix_print(mat4 m);
mat4 scalar_matrix_multiply(float s, mat4 m);
mat4 matrix_add(mat4 m, mat4 m2);
mat4 matrix_sub(mat4 m, mat4 m2);
mat4 matrix_matrix_multiply(mat4 m, mat4 m2);
mat4 inverse(mat4 m);
mat4 transpose(mat4 m);
vec4 matrix_vector_multiply(mat4 m, vec4 v);

// Point manipulation functions
mat4 scale(float x, float y, float z);
mat4 translate(float x, float y, float z);
mat4 rotate_x(float t);  // t for theta
mat4 rotate_y(float t);
mat4 rotate_z(float t);
