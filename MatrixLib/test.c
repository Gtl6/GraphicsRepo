#include "matrixlib.h"

int main(){
	float s = 3.0f;
	vec4 v1 = {1, 2, 3, 4};
	vec4 v2 = {5, 6, 7, 8};
	mat4 m1 = {{1, -5, 9, 13}, {2, 6, -10, 14}, {3, 7, 11, 15}, {4, 8, 12, -16}};
	mat4 m2 = {{4, 8, 12, 16}, {3, 7, 11, 15}, {2, 6, 10, 14}, {1, 5, 9, 13}};

	vector_print(scalar_vector_multiply(s, v1));
	vector_print(scalar_vector_multiply(s, v2));
	vector_print(vector_add(v1, v2));
	vector_print(vector_sub(v1, v2));
	printf("%.2f\n", magnitude(v1));
	printf("%.2f\n", magnitude(v2));
	vector_print(normalize(v1));
	vector_print(normalize(v2));
	printf("%.2f\n", dot_product(v1, v2));
	vector_print(cross_product(v1, v2));
	vector_print(cross_product(v2, v1));

	printf("Done with vectors, boiiiiii\n");
	
	matrix_print(scalar_matrix_multiply(s, m1));
	matrix_print(scalar_matrix_multiply(s, m2));
	matrix_print(matrix_add(m1, m2));
	matrix_print(matrix_sub(m1, m2));
	matrix_print(matrix_multiply(m1, m2));
	matrix_print(matrix_multiply(m2, m1));
	matrix_print(inverse(m1));
	matrix_print(inverse(m2));
	matrix_print(transpose(m1));
	matrix_print(transpose(m2));
	matrix_print(matrix_vector_multiply(m1, v1));
	matrix_print(matrix_vector_multiply(m2, v1));
	
	return 0;
}
