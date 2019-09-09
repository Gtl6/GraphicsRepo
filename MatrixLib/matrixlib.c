// Written by Griffin Lynch
#include "matrixlib.h"

int main(void){
	vec4 v = {1, 2, 3, 4};
	vec4 v2 = {2, 3, 4, 5};

	print_vector(v);
	print_vector(v2);
	v = multiply(5, v);
	print_vector(v);
	v2 = add(v, v2);
	print_vector(v2);
	v = sub(v, v2);
	print_vector(v);

	printf("v: ");
	print_vector(v);
	printf("v2: ");
	print_vector(v2);

	return 0;
}

// Simple Function to print out a 4x1 vector on a screen
// Note the vector is printed as a row vector, though we consider them to be
// 	column vectors internally
void print_vector(vec4 v){
	printf("[%.2f, %.2f, %.2f, %.2f]\n", v.x, v.y, v.z, v.w);
}

// Multiplies each element in a vector by a scalar value and returns the result
vec4 multiply(float s, vec4 v){
	v.x *= s;
	v.y *= s;
	v.z *= s;
	v.w *= s;
	return v;
}

// Adds together two vectors, and returns the result
vec4 add(vec4 v, vec4 v2){
	v.x += v2.x;
	v.y += v2.y;
	v.z += v2.z;
	v.w += v2.w;
	return v;
}

// Subtracts one vector from another and returns the result
vec4 sub(vec4 v, vec4 v2){
	v.x -= v2.x;
	v.y -= v2.y;
	v.z -= v2.z;
	v.w -= v2.w;
	return v;
}

// Performs the dot product operation on two vectors, and returns the result
float dot_product(vec4 v, vec4 v2){
	return (v.x * v2.x) + (v.y * v2.y) + (v.z * v2.z) + (v.w * v2.w);
}

// Performs the cross product operation on two vectors, and returns the result
vec4 cross_product(vec4 v, vec4 v2){
	vec4 returner;
	returner.x = (v.y * v2.z) - (v.z * v2.y);
	returner.y = (v.z * v2.x) - (v.x * v2.z);
	returner.z = (v.x * v2.y) - (v.y * v2.x);
	returner.w = 0.0f;
	return returner;
}















