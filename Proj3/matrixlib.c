// Written by Griffin Lynch
#include "matrixlib.h"

// just a nice thing to have
mat4 identity_matrix(){
	mat4 id = {{1, 0, 0, 0},
					{0, 1, 0, 0},
					{0, 0, 1, 0},
					{0, 0, 0, 1}};

	return id;
}

// Simple Function to print out a 4x1 vector on a screen
// Note the vector is printed as a row vector, though we consider them to be
// 	column vectors internally
void vector_print(vec4 v){
	printf("[%.2f, %.2f, %.2f, %.2f]\n", v.x, v.y, v.z, v.w);
}

// Multiplies each element in a vector by a scalar value and returns the result
vec4 scalar_vector_multiply(float s, vec4 v){
	v.x *= s;
	v.y *= s;
	v.z *= s;
	v.w *= s;
	return v;
}

// Gives the hadamard product of two vectors (component-wise multiplication)
vec4 hadamard_product(vec4 a, vec4 b){
	vec4 returner = (vec4){a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w};
	return returner;
}

// Adds together two vectors, and returns the result
vec4 vector_add(vec4 v, vec4 v2){
	v.x += v2.x;
	v.y += v2.y;
	v.z += v2.z;
	v.w += v2.w;
	return v;
}

// Subtracts one vector from another and returns the result
vec4 vector_sub(vec4 v, vec4 v2){
	v.x -= v2.x;
	v.y -= v2.y;
	v.z -= v2.z;
	v.w -= v2.w;
	return v;
}

// Calculate the magnitude of a vector
float magnitude(vec4 v){
	return (float)sqrt((double)(v.x * v.x + v.y * v.y + v.z * v.z));
}

// Normalize a vector, that is, set its magnitude to 1
vec4 normalize(vec4 v){
	float mag = magnitude(v);
	v.x /= mag;
	v.y /= mag;
	v.z /= mag;
	return v;
}

// Performs the dot product operation on two vectors, and returns the result
float dot_product(vec4 v, vec4 v2){
	return (v.x * v2.x) + (v.y * v2.y) + (v.z * v2.z);
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
void matrix_print(mat4 m){
	printf("| %.2f %.2f %.2f %.2f |\n", m.x.x, m.y.x, m.z.x, m.w.x);
	printf("| %.2f %.2f %.2f %.2f |\n", m.x.y, m.y.y, m.z.y, m.w.y);
	printf("| %.2f %.2f %.2f %.2f |\n", m.x.z, m.y.z, m.z.z, m.w.z);
	printf("|_%.2f %.2f %.2f %.2f_|\n", m.x.w, m.y.w, m.z.w, m.w.w);
}


// multiplies a matrix by a scalar
mat4 scalar_matrix_multiply(float s, mat4 m){
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
mat4 matrix_add(mat4 m, mat4 m2){
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
mat4 matrix_sub(mat4 m, mat4 m2){
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
mat4 matrix_matrix_multiply(mat4 a, mat4 b){
	mat4 c;

	c.x.x = (a.x.x * b.x.x) + (a.y.x * b.x.y) + (a.z.x * b.x.z) + (a.w.x * b.x.w);
	c.x.y = (a.x.y * b.x.x) + (a.y.y * b.x.y) + (a.z.y * b.x.z) + (a.w.y * b.x.w);
	c.x.z = (a.x.z * b.x.x) + (a.y.z * b.x.y) + (a.z.z * b.x.z) + (a.w.z * b.x.w);
	c.x.w = (a.x.w * b.x.x) + (a.y.w * b.x.y) + (a.z.w * b.x.z) + (a.w.w * b.x.w);

	c.y.x = (a.x.x * b.y.x) + (a.y.x * b.y.y) + (a.z.x * b.y.z) + (a.w.x * b.y.w);
	c.y.y = (a.x.y * b.y.x) + (a.y.y * b.y.y) + (a.z.y * b.y.z) + (a.w.y * b.y.w);
	c.y.z = (a.x.z * b.y.x) + (a.y.z * b.y.y) + (a.z.z * b.y.z) + (a.w.z * b.y.w);
	c.y.w = (a.x.w * b.y.x) + (a.y.w * b.y.y) + (a.z.w * b.y.z) + (a.w.w * b.y.w);

	c.z.x = (a.x.x * b.z.x) + (a.y.x * b.z.y) + (a.z.x * b.z.z) + (a.w.x * b.z.w);
	c.z.y = (a.x.y * b.z.x) + (a.y.y * b.z.y) + (a.z.y * b.z.z) + (a.w.y * b.z.w);
	c.z.z = (a.x.z * b.z.x) + (a.y.z * b.z.y) + (a.z.z * b.z.z) + (a.w.z * b.z.w);
	c.z.w = (a.x.w * b.z.x) + (a.y.w * b.z.y) + (a.z.w * b.z.z) + (a.w.w * b.z.w);

	c.w.x = (a.x.x * b.w.x) + (a.y.x * b.w.y) + (a.z.x * b.w.z) + (a.w.x * b.w.w);
	c.w.y = (a.x.y * b.w.x) + (a.y.y * b.w.y) + (a.z.y * b.w.z) + (a.w.y * b.w.w);
	c.w.z = (a.x.z * b.w.x) + (a.y.z * b.w.y) + (a.z.z * b.w.z) + (a.w.z * b.w.w);
	c.w.w = (a.x.w * b.w.x) + (a.y.w * b.w.y) + (a.z.w * b.w.z) + (a.w.w * b.w.w);

	return c;
}

// Inverse of a matrix Process
// Calculate the matrix of minors
mat4 matrixOfMinor(mat4 a){
	mat4 m;

	// These lines were written by a python program, lmao
	// I did write the python program though, so at least give me credit for that
	m.x.x = (a.y.y * a.z.z * a.w.w) + (a.z.y * a.w.z * a.y.w) + (a.w.y * a.y.z * a.z.w) - (a.y.w * a.z.z * a.w.y) - (a.z.w * a.w.z * a.y.y) - (a.w.w * a.y.z * a.z.y);
	m.x.y = (a.y.x * a.z.z * a.w.w) + (a.z.x * a.w.z * a.y.w) + (a.w.x * a.y.z * a.z.w) - (a.y.w * a.z.z * a.w.x) - (a.z.w * a.w.z * a.y.x) - (a.w.w * a.y.z * a.z.x);
	m.x.z = (a.y.x * a.z.y * a.w.w) + (a.z.x * a.w.y * a.y.w) + (a.w.x * a.y.y * a.z.w) - (a.y.w * a.z.y * a.w.x) - (a.z.w * a.w.y * a.y.x) - (a.w.w * a.y.y * a.z.x);
	m.x.w = (a.y.x * a.z.y * a.w.z) + (a.z.x * a.w.y * a.y.z) + (a.w.x * a.y.y * a.z.z) - (a.y.z * a.z.y * a.w.x) - (a.z.z * a.w.y * a.y.x) - (a.w.z * a.y.y * a.z.x);
	m.y.x = (a.x.y * a.z.z * a.w.w) + (a.z.y * a.w.z * a.x.w) + (a.w.y * a.x.z * a.z.w) - (a.x.w * a.z.z * a.w.y) - (a.z.w * a.w.z * a.x.y) - (a.w.w * a.x.z * a.z.y);
	m.y.y = (a.x.x * a.z.z * a.w.w) + (a.z.x * a.w.z * a.x.w) + (a.w.x * a.x.z * a.z.w) - (a.x.w * a.z.z * a.w.x) - (a.z.w * a.w.z * a.x.x) - (a.w.w * a.x.z * a.z.x);
	m.y.z = (a.x.x * a.z.y * a.w.w) + (a.z.x * a.w.y * a.x.w) + (a.w.x * a.x.y * a.z.w) - (a.x.w * a.z.y * a.w.x) - (a.z.w * a.w.y * a.x.x) - (a.w.w * a.x.y * a.z.x);
	m.y.w = (a.x.x * a.z.y * a.w.z) + (a.z.x * a.w.y * a.x.z) + (a.w.x * a.x.y * a.z.z) - (a.x.z * a.z.y * a.w.x) - (a.z.z * a.w.y * a.x.x) - (a.w.z * a.x.y * a.z.x);
	m.z.x = (a.x.y * a.y.z * a.w.w) + (a.y.y * a.w.z * a.x.w) + (a.w.y * a.x.z * a.y.w) - (a.x.w * a.y.z * a.w.y) - (a.y.w * a.w.z * a.x.y) - (a.w.w * a.x.z * a.y.y);
	m.z.y = (a.x.x * a.y.z * a.w.w) + (a.y.x * a.w.z * a.x.w) + (a.w.x * a.x.z * a.y.w) - (a.x.w * a.y.z * a.w.x) - (a.y.w * a.w.z * a.x.x) - (a.w.w * a.x.z * a.y.x);
	m.z.z = (a.x.x * a.y.y * a.w.w) + (a.y.x * a.w.y * a.x.w) + (a.w.x * a.x.y * a.y.w) - (a.x.w * a.y.y * a.w.x) - (a.y.w * a.w.y * a.x.x) - (a.w.w * a.x.y * a.y.x);
	m.z.w = (a.x.x * a.y.y * a.w.z) + (a.y.x * a.w.y * a.x.z) + (a.w.x * a.x.y * a.y.z) - (a.x.z * a.y.y * a.w.x) - (a.y.z * a.w.y * a.x.x) - (a.w.z * a.x.y * a.y.x);
	m.w.x = (a.x.y * a.y.z * a.z.w) + (a.y.y * a.z.z * a.x.w) + (a.z.y * a.x.z * a.y.w) - (a.x.w * a.y.z * a.z.y) - (a.y.w * a.z.z * a.x.y) - (a.z.w * a.x.z * a.y.y);
	m.w.y = (a.x.x * a.y.z * a.z.w) + (a.y.x * a.z.z * a.x.w) + (a.z.x * a.x.z * a.y.w) - (a.x.w * a.y.z * a.z.x) - (a.y.w * a.z.z * a.x.x) - (a.z.w * a.x.z * a.y.x);
	m.w.z = (a.x.x * a.y.y * a.z.w) + (a.y.x * a.z.y * a.x.w) + (a.z.x * a.x.y * a.y.w) - (a.x.w * a.y.y * a.z.x) - (a.y.w * a.z.y * a.x.x) - (a.z.w * a.x.y * a.y.x);
	m.w.w = (a.x.x * a.y.y * a.z.z) + (a.y.x * a.z.y * a.x.z) + (a.z.x * a.x.y * a.y.z) - (a.x.z * a.y.y * a.z.x) - (a.y.z * a.z.y * a.x.x) - (a.z.z * a.x.y * a.y.x);

	return m;
}

// Returns the cofactor of a matrix
mat4 cofactor(mat4 m){
	m.x.y *= -1;
	m.x.w *= -1;
	m.y.x *= -1;
	m.y.z *= -1;
	m.z.y *= -1;
	m.z.w *= -1;
	m.w.x *= -1;
	m.w.z *= -1;

	return m;
}

// Find the determinant of the 4x4 matrix and its 4x4 matrix of minor NOTE: DO THIS AFTER YOU APPLY THE COFACTOR
float determinant(mat4 m, mat4 n){
	float det = m.x.x * n.x.x + m.y.x * n.y.x + m.z.x * n.z.x + m.w.x * n.w.x;
	return det;
}

// Generates the transpose of a matrix
mat4 transpose(mat4 m){
	mat4 b;

	b.x.x = m.x.x;
	b.y.x = m.x.y;
	b.z.x = m.x.z;
	b.w.x = m.x.w;

	b.x.y = m.y.x;
	b.y.y = m.y.y;
	b.z.y = m.y.z;
	b.w.y = m.y.w;

	b.x.z = m.z.x;
	b.y.z = m.z.y;
	b.z.z = m.z.z;
	b.w.z = m.z.w;

	b.x.w = m.w.x;
	b.y.w = m.w.y;
	b.z.w = m.w.z;
	b.w.w = m.w.w;

	return b;
}

// Matrix-vector multiplication
vec4 matrix_vector_multiply(mat4 a, vec4 v){
	vec4 result;
	result.x = (a.x.x * v.x) + (a.y.x * v.y) + (a.z.x * v.z) + (a.w.x * v.w);
	result.y = (a.x.y * v.x) + (a.y.y * v.y) + (a.z.y * v.z) + (a.w.y * v.w);
	result.z = (a.x.z * v.x) + (a.y.z * v.y) + (a.z.z * v.z) + (a.w.z * v.w);
	result.w = (a.x.w * v.x) + (a.y.w * v.y) + (a.z.w * v.z) + (a.w.w * v.w);

	return result;
}

// Returns the inverse of a matrix, using many of the above helper functions
// This is the only function that calls other functions in this library. Let's hope it doesn't slow everything down.
mat4 inverse(mat4 m){
	mat4 mom = matrixOfMinor(m); // Generates the Matrix of minor, which is perhaps trivially obvious given the function name. Oh well.
	mom = cofactor(mom); // Cofactors the Matrix of minor
	float det = determinant(m, mom); // Figures out the determinant using the cofactored Matrix of Minor

	// We can apparently assume that det is non-zero
	if(det == 0) printf("Det is zero! THERE's AN ISSUE HERE!\n");
	det = 1/det;
	mom = transpose(mom);
	mom = scalar_matrix_multiply(det, mom);
	return mom;
}

// Returns a translation matrix (as specified by the powerpoints)
mat4 translate(float x, float y, float z){
	mat4 returner = {{1, 0, 0, x},
			 {0, 1, 0, y},
			 {0, 0, 1, z},
			 {0, 0, 0, 1}};
	return transpose(returner);
}

// Returns a scaling matrix (as specified by the powerpoints)
mat4 scale(float x, float y, float z){
	mat4 returner = {{x, 0, 0, 0},
			 {0, y, 0, 0},
			 {0, 0, z, 0},
			 {0, 0, 0, 1}};
	return transpose(returner);
}

// Returns a rotation about the z-axis matrix
mat4 rotate_z(float t){
	mat4 returner = {{cos(t), -1*sin(t), 0, 0},
			 {sin(t), cos(t), 0, 0},
			 {0, 0, 1, 0},
			 {0, 0, 0, 1}};
	return transpose(returner);
}

// Returns a rotation about the x-axis matrix
mat4 rotate_x(float t){
	mat4 returner = {{1, 0, 0, 0},
			 {0, cos(t), -1*sin(t), 0},
			 {0, sin(t), cos(t), 0},
			 {0, 0, 0, 1}};
	return transpose(returner);
}

// Returns a rotation about the y-axis matrix
mat4 rotate_y(float t){
	mat4 returner = {{cos(t), 0, sin(t), 0},
			 {0, 1, 0, 0},
			 {-1*sin(t), 0, cos(t), 0},
			 {0, 0, 0, 1}};
	return transpose(returner);
}

// Returns a rotation about any arbitrary vector
mat4 rotate_about_vector(vec4 v, float t){
	mat4 ret = {{1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1}};
	vec4 xz = {v.x, 0, v.z, 0};
	vec4 yz = {0, v.y, v.z, 0};
	vec4 z_proj = {0, 0, 1.0, 0};
	vec4 y_proj = {0, 1.0, 0, 0};

	// First figure out the angle to rotate to get it even with the yz plane
	float a = magnitude(yz);
	float b = magnitude(z_proj);
	float output = dot_product(yz, z_proj) / (a * b);
	float theta1 = acos(output);

	if(v.x > 0) theta1 *= -1;

	if(isnan(theta1)) theta1 = PI / 2;

	// Now the vector is in the yz plane, rotate up to the positive y vector
	a = magnitude(v);
	b = magnitude(y_proj);
	output = dot_product(v, y_proj) / (a * b);
	float theta2 = acos(output);

	if(isnan(theta2)) theta2 = 0;

	// So now we know both of those angles are good, we first rotate to the yz plane
	ret = matrix_matrix_multiply(rotate_y(theta1), ret);
	// Now rotate so the thing is on the y-axis
	ret = matrix_matrix_multiply(rotate_x(theta2), ret);
	// Now do your actual angle rotation
	ret = matrix_matrix_multiply(rotate_y(t), ret);
	// And now put the object back
	ret = matrix_matrix_multiply(rotate_x(-1 * theta2), ret);
	ret = matrix_matrix_multiply(rotate_y(-1 * theta1), ret);

	return ret;
}

mat4 rotate_local_vector(vec4 local_vec, float t, vec4 com){
	mat4 ret;
	ret = translate(-com.x, -com.y, -com.z);
	ret = matrix_matrix_multiply(rotate_about_vector(local_vec, t), ret);
	ret = matrix_matrix_multiply(translate(com.x, com.y, com.z), ret);
	return ret;
}

mat4 rotate_local_x(float t, vec4 com){
	return rotate_local_vector((vec4){1.0f, 0, 0, 1.0f}, t, com);
}

mat4 rotate_local_y(float t, vec4 com){
	return rotate_local_vector((vec4){0, 1.0f, 0, 1.0f}, t, com);
}

mat4 rotate_local_z(float t, vec4 com){
	return rotate_local_vector((vec4){0, 0, 1.0f, 1.0f}, t, com);
}


// I don't actually know what this does, to be totally honest with you
mat4 frustum(float left, float right, float bottom, float top, float near, float far){
	mat4 returner = {{-2 * near / (right - left), 0, (left + right) / (right - left), 0},
										{0, -2 * near / (top - bottom), (bottom + top) / (top - bottom), 0},
										{0, 0, (near + far) / (far - near), -2 * near * far / (far - near)},
										{0, 0, -1, 0}};
	returner = transpose(returner);
	return returner;
}


// Moves the camera to eye_vec, and looks at at_vec with a dutch according to up_vec
mat4 look_at_f(float eyex, float eyey, float eyez, float atx, float aty, float atz, float upx, float upy, float upz){
	vec4 pos = {eyex, eyey, eyez, 0};
	vec4 at = {atx, aty, atz, 0};
	vec4 vup = {upx, upy, upz, 0};

	// This will give the camera frame z vector
	vec4 n = vector_sub(pos, at);

	// This gives the camera frame x vector
	vec4 u = cross_product(vup, n);

	// This gives our camera frame y vector
	vec4 v = cross_product(n, u);

	// Now normalize them all
	u = normalize(u);
	v = normalize(v);
	n = normalize(n);

	// This is the matrix
	mat4 M = {{u.x, v.x, n.x, eyex},
							{u.y, v.y, n.y, eyey},
							{u.z, v.z, n.z, eyez},
							{0, 0, 0, 1}};

	// The translation matrix that moves the points from the world frame to the camera frame
	mat4 ret = transpose(M);
	ret = inverse(ret);

	return ret;
}

mat4 look_at_v(vec4 eye, vec4 at, vec4 up){
	return look_at_f(eye.x, eye.y, eye.z, at.x, at.y, at.z, up.x, up.y, up.w);
}

// A perspective view matrix
mat4 perspective(float fovy, float aspect, float near, float far){
	float cot = cosf(-1 * fovy / 2) / sinf(-1 * fovy / 2);
	mat4 ret = {{-1 * cot / aspect, 0, 0, 0},
							{0, -1 * cot, 0, 0},
							{0, 0, (near + far) / (far - near), -2 * near * far / (far - near)},
							{0, 0, -1, 0}};
	ret = transpose(ret);
	return ret;
}
