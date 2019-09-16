#include "matrixlib.h"

int main(){
	float s = 3.0f;
	vec4 v1 = {1, 2, 3, 4};
	vec4 v2 = {5, 6, 7, 8};
	mat4 m1 = {{1, -5, 9, 13}, {2, 6, -10, 14}, {3, 7, 11, 15}, {4, 8, 12, -16}};
	mat4 m2 = {{4, 8, 12, 16}, {3, 7, 11, 15}, {2, 6, 10, 14}, {1, 5, 9, 13}};

	mat4 I = {{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}};

	matrix_print(inverse(m2));
	printf("\n");
	printf("%.2f\n", dot_product(v1, cross_product(v1, v2)));
	printf("\n");
	printf("%.2f\n", dot_product(v1, vector_add(v1, v2)));
	printf("%.2f\n", dot_product(v1, v1) + dot_product(v1, v2));
	printf("\n");
	matrix_print(transpose(I));
	matrix_print(inverse(I));
	printf("\n");
	vector_print(vector_sub(v1, vector_add(v1, v2)));
	printf("\n");

	mat4 m3 = scalar_matrix_multiply(2.0f, m1);
	matrix_print(scalar_matrix_multiply(0.5f, m3));
	matrix_print(m1);


	return 0;
}
