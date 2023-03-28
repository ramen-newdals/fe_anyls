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

void calc_dyadic(int num_element, float *a, float *b, float *c){
	// Calculate the dyadic of two equally sized vectors a and b and stores the result in c
	int i,j;
	for(i = 0; i<num_element; i++){
		for(j = 0; j<num_element; j++){
			c[(i*num_element)+j] = a[i]*b[j];
		}
	}
	// Print out the dyadic for testing correctness
	for(i = 0; i<num_element; i++){
		for(j = 0; j<num_element; j++){
			printf("%f\n", c[(i*num_element)+j]);
		}
	}
}

void generate_element_stiffness_matricies(float *direction_cosine_l, float *direction_cosine_m, float*direction_cosine_n, int element_num, float *element_stiffness_matrix, int mat_row, int mat_col){
	//	INPUTS: direction_cosine_l, direction_cosine_m, direction_cosine_n, column_num, stiffness_column
	int i, j; // row column identifies
	printf("Printing Element Stiffness Matrix:\n");
	for(i=0; i<mat_row; i++){
		for(j=0; j<mat_col; j++){
			element_stiffness_matrix[(i*mat_col)+j] = 6.9;
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
	calc_dyadic(3, a, b, dyadic_result);
	generate_element_stiffness_matricies(direction_cosine_l, direction_cosine_m, direction_cosine_n, 0, element_stiffness_matrix, mat_row, mat_col);


	free(nodes_x);
	free(nodes_y);
	free(nodes_z);
	free(element_connectiviity);
	free(direction_cosine_l);
	free(element_stiffness_matrix);
	
	printf("aw helllllllllll yeah\n");
	return 0;
}