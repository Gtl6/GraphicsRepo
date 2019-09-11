// Written by Griffin Lynch
#include "matrixlib.h"

int main(void){
	vec4 v = {1, 2, 3, 4};
	vec4 v2 = {2, 3, 4, 5};

	print(v);
	print(v2);
	v = multiply(5, v);
	print(v);
	v2 = add(v, v2);
	print(v2);
	v = sub(v, v2);
	print(v);

	printf("v: ");
	print(v);
	printf("v2: ");
	print(v2);

	return 0;
}

// Simple Function to print out a 4x1 vector on a screen
// Note the vector is printed as a row vector, though we consider them to be
// 	column vectors internally
void print(vec4 v){
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

// prints out a 4x4 matrix
void print(mat4 m){
	printf("| %.2f %.2f %.2f %.2f |\n", m.x.x, m.y.x, m.z.x, m.w.x);
	printf("| %.2f %.2f %.2f %.2f |\n", m.x.y, m.y.y, m.z.y, m.w.y);
	printf("| %.2f %.2f %.2f %.2f |\n", m.x.z, m.y.z, m.z.z, m.w.z);
	printf("|_%.2f %.2f %.2f %.2f |\n", m.x.w, m.y.w, m.z.w, m.w.w);
}


// multiplies a matrix by a scalar
mat4 multiply(float s, mat4 m){
	m.x.x *= s;
	m.x.y *= s;
	m.x.z *= s;
	m.x.w *= s;
	m.y.x *= s;
	m.y.y *= s;
	m.y.z *= s;
	m.y.w *= s;
	m.z.x *= s;
	m.z.y *= s;
	m.z.z *= s;
	m.z.w *= s;
	m.w.x *= s;
	m.w.y *= s;
	m.w.z *= s;
	m.w.w *= s;

	return m;
}

// adds m2 to m
mat4 add(mat4 m, mat4 m2){
	m.x.x += m2.x.x;
	m.x.y += m2.x.y;
	m.x.z += m2.x.z;
	m.x.w += m2.x.w;
	m.y.x += m2.y.x;
	m.y.y += m2.y.y;
	m.y.z += m2.y.z;
	m.y.w += m2.y.w;
	m.z.x += m2.z.x;
	m.z.y += m2.z.y;
	m.z.z += m2.z.z;
	m.z.w += m2.z.w;
	m.w.x += m2.w.x;
	m.w.y += m2.w.y;
	m.w.z += m2.w.z;
	m.w.w += m2.w.w;

	return m;
} 


// subtracts m2 from m
mat4 sub(mat4 m, mat4 m2){
	m.x.x -= m2.x.x;
	m.x.y -= m2.x.y;
	m.x.z -= m2.x.z;
	m.x.w -= m2.x.w;
	m.y.x -= m2.y.x;
	m.y.y -= m2.y.y;
	m.y.z -= m2.y.z;
	m.y.w -= m2.y.w;
	m.z.x -= m2.z.x;
	m.z.y -= m2.z.y;
	m.z.z -= m2.z.z;
	m.z.w -= m2.z.w;
	m.w.x -= m2.w.x;
	m.w.y -= m2.w.y;
	m.w.z -= m2.w.z;
	m.w.w -= m2.w.w;

	return m;
}

// multiplies two 4x4 matrices
mat4 multiply(mat4 a, mat4 b){
	mat4 c;

	c.x.x = (a.x.x * b.x.x) + (a.x.y * b.y.x) + (a.x.z * b.z.x) + (a.x.w * b.w.x);
	c.y.x = (a.y.x * b.x.x) + (a.y.y * b.y.x) + (a.y.z * b.z.x) + (a.y.w * b.w.x);
	c.z.x = (a.z.x * b.x.x) + (a.z.y * b.y.x) + (a.z.z * b.z.x) + (a.z.w * b.w.x);
	c.w.x = (a.w.x * b.x.x) + (a.w.y * b.y.x) + (a.w.z * b.z.x) + (a.w.w * b.w.x);

	c.x.y = (a.x.x * b.x.y) + (a.x.y * b.y.y) + (a.x.z * b.z.y) + (a.x.w * b.w.y);
	c.y.y = (a.y.x * b.x.y) + (a.y.y * b.y.y) + (a.y.z * b.z.y) + (a.y.w * b.w.y);
	c.z.y = (a.z.x * b.x.y) + (a.z.y * b.y.y) + (a.z.z * b.z.y) + (a.z.w * b.w.y);
	c.w.y = (a.w.x * b.x.y) + (a.w.y * b.y.y) + (a.w.z * b.z.y) + (a.w.w * b.w.y);

	c.x.z = (a.x.x * b.x.z) + (a.x.y * b.y.z) + (a.x.z * b.z.z) + (a.x.w * b.w.z);
	c.y.z = (a.y.x * b.x.z) + (a.y.y * b.y.z) + (a.y.z * b.z.z) + (a.y.w * b.w.z);
	c.z.z = (a.z.x * b.x.z) + (a.z.y * b.y.z) + (a.z.z * b.z.z) + (a.z.w * b.w.z);
	c.w.z = (a.w.x * b.x.z) + (a.w.y * b.y.z) + (a.w.z * b.z.z) + (a.w.w * b.w.z);

	c.x.w = (a.x.x * b.x.w) + (a.x.y * b.y.w) + (a.x.z * b.z.w) + (a.x.w * b.w.w);
	c.y.w = (a.y.x * b.x.w) + (a.y.y * b.y.w) + (a.y.z * b.z.w) + (a.y.w * b.w.w);
	c.z.w = (a.z.x * b.x.w) + (a.z.y * b.y.w) + (a.z.z * b.z.w) + (a.z.w * b.w.w);
	c.w.w = (a.w.x * b.x.w) + (a.w.y * b.y.w) + (a.w.z * b.z.w) + (a.w.w * b.w.w);

	return c;
}

// Inverse of a matrix

// Generates the transpose of a matrix
mat4 transpose(mat4 m){
	mat4 b;

	b.x.x = a.x.x;
	b.x.y = a.y.x;
	b.x.z = a.z.x;
	b.x.w = a.w.x;

	b.y.x = a.x.y;
	b.y.y = a.y.y;
	b.y.z = a.z.y;
	b.y.w = a.w.y;

	b.z.x = a.x.z;
	b.z.y = a.y.z;
	b.z.z = a.z.z;
	b.z.w = a.w.z;

	b.w.x = a.x.w;
	b.w.y = a.y.w;
	b.w.z = a.z.w;
	b.w.w = a.w.w;

	return b;
}

// Matrix-vector multiplication











