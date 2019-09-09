// Written by Griffin Lynch
#include <stdio.h>

typedef struct {
	float x;
	float y;
	float z;
	float w;
} vec4;

void print_vector(vec4 v);
vec4 multiply(float s, vec4 v);
vec4 add(vec4 v, vec4 v2);
vec4 sub(vec4 v, vec4 v2);
float dot_product(vec4 v, vec4 v2);
vec4 cross_product(vec4 v, vec4 v2);
