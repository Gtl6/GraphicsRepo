#include "matrixlib.h"

int main(){
	float s = 3.0f;
	vec4 v1 = {1, 2, 3, 4};
	vec4 v2 = {5, 6, 7, 8};
	mat4 m1 = {{1, -5, 9, 13}, {2, 6, -10, 14}, {3, 7, 11, 15}, {4, 8, 12, -16}};
	mat4 m2 = {{4, 8, 12, 16}, {3, 7, 11, 15}, {2, 6, 10, 14}, {1, 5, 9, 13}};

	printf("Scalar - vectori multiplication:\n");
	vector_print(scalar_vector_multiply(s, v1));

	printf("Vector - vector addition and subtraction:\n");
	vector_print(vector_add(v1, v2));
	vector_print(vector_sub(v1, v2));

	printf("Magnitude of v1:\n");
	printf("%.2f\n", magnitude(v1));
	
	printf("Normalize v1:\n");
	vector_print(normalize(v1));

	printf("Dot product v1 . v2:\n");
	printf("%.2f\n", dot_product(v1, v2));

	printf("v1 x v2");
	vector_print(cross_product(v1, v2));


	printf("s * m1:\n");
	matrix_print(scalar_matrix_multiply(s, m1));
	
	printf("m1 +/- m2:\n");
	matrix_print(matrix_add(m1, m2));
	matrix_print(matrix_sub(m1, m2));
	
	printf("m1 * m2:\n");
	matrix_print(matrix_matrix_multiply(m1, m2));
	
	printf("m1 inverse:\n");
	matrix_print(inverse(m1));
	
	printf("m1 inverse times m1:\n");
	matrix_print(matrix_matrix_multiply(m1, inverse(m1)));
	
	printf("m1 transpose:\n");
	matrix_print(transpose(m1));

	printf("m1 * v1:\n");
	vector_print(matrix_vector_multiply(m1, v1));
	
	return 0;
}
