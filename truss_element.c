#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include "logging.h"

//===== Nodal Data ====
int num_node;
float *nodes_x;
float *nodes_y;
float *nodes_z;

//==== Element Data ====
int num_element;
int nodes_per_element = 2;
int *element_connectiviity;

float *cross_section_area;
float *element_length;
float *direction_cosine_l;
float *direction_cosine_m;
float *direction_cosine_n;
float *youngs_modulus;
float *element_stiffness_matrix;

typedef struct rod_elements_1d{
	int num_node;
	int num_els;		//nels in f
	int np_types;		//np_types in f
	float prop;		// prop in f
	float* element_array; // ell in f
	int num_restrained_nodes; // nr in the book
	int* restrained_node_numbers; // The node number of the restrained node
	float* restrained_node_values;	//  The value of the restraind at the coresponding restrained node
	int num_loaded_nodes;
	int* loaded_node_numbers;
	float* loaded_node_values;
	int fixed_freedoms; // fixed_freedoms. Used for displacment B.C

	/*NOTE: 
	The default for 1d_rod elements is that nodes are not restrained
	
	For thesse types of 1d problems it is simple to generate the steering vector g* */ 
} rod_elements_1d;

void local_truss_assembly(int num_nodes, int num_elements, float *nodes_x, float *nodes_y,  float *nodes_z){
	/*Takes input a truss_element  and assembles its stiffness matrix*/
}

void calc_element_length(int num_element, int nodes_per_element, int *element_connectiviity, int num_node, float *nodes_x, float *nodes_y, float *nodes_z, float *element_length){
	// calulate the element length for each element
	float x_i, x_j, y_i, y_j, z_i, z_j;
	int i;
	for(i = 0; i<num_element; i++){
		x_i = nodes_x[element_connectiviity[i*nodes_per_element]]; 
		x_j = nodes_x[element_connectiviity[(i*nodes_per_element)+1]];
		y_i = nodes_y[element_connectiviity[i*nodes_per_element]];
		y_j = nodes_y[element_connectiviity[(i*nodes_per_element)+1]];
		z_i = nodes_z[element_connectiviity[i*nodes_per_element]];
		z_j = nodes_z[element_connectiviity[(i*nodes_per_element)+1]];
		element_length[i] = sqrtf(((x_j - x_i)*(x_j - x_i)) + ((y_j - y_i)*(y_j - y_i)) + ((z_j - z_i)*(z_j - z_i)));
	}
}

void calc_direction_cosine(int num_element, int nodes_per_element, int *element_connectiviity, int num_node, float *nodes_x, float *nodes_y, float *nodes_z, float *element_length, float *direction_cosine_l, float *direction_cosine_m, float *direction_cosine_n){
	// For each element calculate the l_ij, m_ij, and n_ij direction cosines
	float l_i, m_i, n_i;
	int i;
	for(i = 0; i<num_element; i++){
		l_i = (nodes_x[element_connectiviity[(i*nodes_per_element)+1]] - nodes_x[element_connectiviity[i*nodes_per_element]])/element_length[i];
		m_i = (nodes_y[element_connectiviity[(i*nodes_per_element)+1]] - nodes_y[element_connectiviity[i*nodes_per_element]])/element_length[i];
		n_i = (nodes_z[element_connectiviity[(i*nodes_per_element)+1]] - nodes_z[element_connectiviity[i*nodes_per_element]])/element_length[i];
		direction_cosine_l[i] = l_i;
		direction_cosine_m[i] = m_i;
		direction_cosine_n[i] = n_i;
	}
}

void generate_element_stiffness_matricies(float *direction_cosine_l, float *direction_cosine_m, float*direction_cosine_n, int element_num, float *element_stiffness_matrix, int mat_row, int mat_col){
	//	INPUTS: direction_cosine_l, direction_cosine_m, direction_cosine_n, column_num, stiffness_column

	// The dyadic of the direction cosine vector withits self gives
	// [k_00, k_01, k_01, k_10, k_11, k_12, K_20, k_21, k_22] --> The result of the dyuadic of [l_ij, m_ij, k_ij][l_ij, m_ij, k_ij]
	// k_00 = l_ij*l_ij, k_01 = l_ij*m_ij, k_02 = l_ij*n_ij, k_10 = m_ij*l_ij, k_11 = m_ij*n_ij
	// k_20 = n_ij*l_ij, k_21 = n_ij*m_ij, k_22 = n_ij*n_ij
	// This can be used to asseembly the elements stiffness matrix entries as,
	// [ k_00,  k_01,  k_01, -k_00, -k_01, -k_02,
	//   k_10,  k_11,  k_12, -k_10, -k_11, -k_12
	//	 K_20,  k_21,  k_22, -k_20, -k_21, -k_22
	//	-k_00, -k_01, -k_02,  k_00,  k_01,  k_02,
	//	-k_10, -k_11, -k_12,  k_10,  k_11,  k_12
	//	-k_20, -k_21, -k_22,  k_20,  k_21,  k_22 ]

	int i, j; // row column identifies
	float *direction_cosines = (float *) malloc(3*sizeof(float));

	// Fill direction cosine vector with the current element
	direction_cosines[0] = direction_cosine_l[element_num];
	direction_cosines[1] = direction_cosine_m[element_num];
	direction_cosines[2] = direction_cosine_n[element_num];
	
	// Fill first 3 rows of stiffness matrix
	for(i = 0; i<3; i++){
		for(j = 0; j<3; j++){
			element_stiffness_matrix[(i*6)+j] = direction_cosines[i]*direction_cosines[j];
			element_stiffness_matrix[(i*6)+3+j] = -1*direction_cosines[i]*direction_cosines[j];
		}
	}
	// Fill last 3 rows of stiffness matrix
	for(i = 0; i<3; i++){
		for(j = 0; j<3; j++){
			element_stiffness_matrix[(i*6)+j+18] = -1*direction_cosines[i]*direction_cosines[j];
			element_stiffness_matrix[(i*6)+3+j+18] = direction_cosines[i]*direction_cosines[j];
		}
	}

	// print the stiffness matrix for debugging
	printf("Printing Element Stiffness Matrix:\n");
	for(i=0; i<mat_row; i++){
		for(j=0; j<mat_col; j++){
			printf("%f\t", element_stiffness_matrix[(i*mat_col)+j]);
		}
		printf("\n");
	}
}

int main(int argc, char* argv[]){
	log_set_level(0);
	log_set_quiet(0);
	//read_mesh_paramaters("/home/ramen_newdals/Documents/ECE1755/project/fe_anyls/mesh/sample_mesh_0_bin.msh");

	num_node = 3;
	num_element = 3;
	nodes_per_element = 2;
	nodes_x = (float*) malloc(num_node*sizeof(float));
	nodes_y = (float*) malloc(num_node*sizeof(float));
	nodes_z = (float*) malloc(num_node*sizeof(float));
	element_connectiviity = (int*) malloc(num_element*nodes_per_element*sizeof(int));
	direction_cosine_l = (float*) malloc(num_element*sizeof(float));
	direction_cosine_m = (float*) malloc(num_element*sizeof(float));
	direction_cosine_n = (float*) malloc(num_element*sizeof(float));
	element_length = (float*) malloc(num_element*sizeof(float));

	// TEST MATRIX for generating element stiffness matricies
	element_stiffness_matrix = (float*) malloc(6*6*sizeof(float));
	
	// Fill arrays with sample data
	// X-Node data
	nodes_x[0] = 0; nodes_x[1] = 1; nodes_x[2] = 0;
	// Y-Node data
	nodes_y[0] = 0; nodes_y[1] = 0; nodes_y[2] = 1;
	// Z-Node data
	nodes_z[0] = 0;	nodes_z[1] = 0; nodes_z[2] = 0;
	// Setting up the element connectivity
	element_connectiviity[0] = 0; element_connectiviity[1] = 1; // Element 0
	element_connectiviity[2] = 0; element_connectiviity[3] = 2; // Element 1
	element_connectiviity[4] = 1; element_connectiviity[5] = 2; // Element 2

	// Generate the stiffness and mass matricies
	calc_element_length(num_element, nodes_per_element, element_connectiviity, num_node, nodes_x, nodes_y, nodes_z, element_length);
	calc_direction_cosine(num_element, nodes_per_element, element_connectiviity, num_node, nodes_x, nodes_y, nodes_z, element_length, direction_cosine_l, direction_cosine_m, direction_cosine_n);

	int mat_row, mat_col;
	mat_row = 6; mat_col = 6;
	float dyadic_result[9];
	float a[3] = {1.0, 0, 0};
	float b[3] = {1,0, 0, 0};
	generate_element_stiffness_matricies(direction_cosine_l, direction_cosine_m, direction_cosine_n, 0, element_stiffness_matrix, mat_row, mat_col);
	generate_element_stiffness_matricies(direction_cosine_l, direction_cosine_m, direction_cosine_n, 1, element_stiffness_matrix, mat_row, mat_col);
	generate_element_stiffness_matricies(direction_cosine_l, direction_cosine_m, direction_cosine_n, 2, element_stiffness_matrix, mat_row, mat_col);

	free(nodes_x);
	free(nodes_y);
	free(nodes_z);
	free(element_connectiviity);
	free(direction_cosine_l);
	free(element_stiffness_matrix);
	
	printf("aw helllllllllll yeah\n");
	return 0;
}