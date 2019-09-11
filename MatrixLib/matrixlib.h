// Written by Griffin Lynch
#include <stdio.h>

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

void print(vec4 v);
vec4 multiply(float s, vec4 v);
vec4 add(vec4 v, vec4 v2);
vec4 sub(vec4 v, vec4 v2);
float dot_product(vec4 v, vec4 v2);
vec4 cross_product(vec4 v, vec4 v2);

void print(mat4 m);
mat4 multiply(float s, mat4 m);
mat4 add(mat4 m, mat4 m2);
mat4 sub(mat4 m, mat4 m2);
mat4 multiply(mat4 m, mat4 m2);
mat4 inverse(mat4 m);
mat4 transpose(mat4 m);
mat4 multiply(mat4 m, vec4 v);
