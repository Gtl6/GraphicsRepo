// Written by Griffin Lynch
#include <stdio.h>
#include <math.h>

#ifndef PI
#define PI 3.1415926
#endif

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

// Just gives you the identity matrix. Nice to have.
mat4 identity_matrix();

// All basic functions
void vector_print(vec4 v);
vec4 scalar_vector_multiply(float s, vec4 v);
vec4 vector_add(vec4 v, vec4 v2);
vec4 vector_sub(vec4 v, vec4 v2);
float magnitude(vec4 v);
float dot_product(vec4 v, vec4 v2);
vec4 cross_product(vec4 v, vec4 v2);
vec4 normalize(vec4);
vec4 hadamard_product(vec4 a, vec4 b);

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
mat4 rotate_about_vector(vec4 v, float t);
mat4 rotate_local_x(float t, vec4 com);
mat4 rotate_local_y(float t, vec4 com);
mat4 rotate_local_z(float t, vec4 com);
mat4 rotate_local_vector(vec4 local_vec, float t, vec4 com);

// Camera Functions
mat4 frustum(float left, float right, float bottom, float top, float near, float far);
mat4 look_at_f(float eyex, float eyey, float eyez, float atx, float aty, float atz, float upx, float upy, float upz);
mat4 perspective(float fovy, float aspect, float near, float far);
mat4 look_at_v(vec4 eye, vec4 at, vec4 up);
