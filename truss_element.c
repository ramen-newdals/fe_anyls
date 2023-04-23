#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include "logging.h"
#include <immintrin.h>

typedef struct truss_element
{
	/* data */
	float *node_0;
	float *node_1;
	float E;
	float A;
	float l_e;
} truss_element;

typedef struct truss_structure
{
	/* data */
}truss_structure;

//===== Nodal Data ====
int num_node;
int dof_per_node;
float *nodes_x;
float *nodes_y;
float *nodes_z;

//==== Element Data ====
int num_element;					// Total number of elements in the mesh
int num_color;						// Number of differnet colors needed to color the mesh
int nodes_per_element = 2;			// Number of nodes per element
int *element_connectiviity;			// Array that stores what nodes belon to what element with global numbering
int *elemnet_color;							// The elements color for parallel assembly

float *cross_section_area; 			// *num_element
float *element_length; 				// *num_element
float *youngs_modulus; 				// *num_element
float *direction_cosine_l; 			// *num_element
float *direction_cosine_m; 			// *num_element
float *direction_cosine_n; 			// *num_element
float *element_stiffness_matricies; 	// *num_element

//=======Global Stifness matrix Data ============
int *row_index; // Rox index
int *col_index; // column index
float *value;	// value at given row, colum

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

void print_element_stiffness_matrix(float *element_stiffness_matricies, int element_num, int num_row, int num_col){
	int i, j, element_num_offset; // Loop iterators for row and col
	element_num_offset = num_row*num_col*element_num;
		printf("Printing Element %d's Stiffness Matrix:\n", element_num);
	for(i=0; i<num_row; i++){
		for(j=0; j<num_col; j++){
			printf("%f\t", element_stiffness_matricies[((i*num_row)+j)+element_num_offset]);
		}
		printf("\n");

	}

}

void generate_element_stiffness_matricies(float *direction_cosine_l, float *direction_cosine_m, float*direction_cosine_n, int element_num, float *element_stiffness_matricies, int num_row, int num_col){
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

	/* element_stiffness_matricies holds all of the element sitffness matricies in a single
		contigious array. Each call of this function using the element_num the section of memory
		that this specific call needs to write to is identified using:
		num_row*num_col*element_num = the starting location for the specified elements stiffness matrix array
		so to write the nth elements stiffness matrix into the single array will look like:
		
		+element_num_offset = num_row*num_col*element_num
		element_stiffness_matricies[(~~Other code to find a local index~~) + +element_num_offset]*/

	int i, j, element_num_offset; // row column identifies
	element_num_offset = num_row*num_col*element_num; // The offset in the array to start populating data with
	float *direction_cosines = (float *) malloc(3*sizeof(float));

	// Fill direction cosine vector with the current element
	direction_cosines[0] = direction_cosine_l[element_num];
	direction_cosines[1] = direction_cosine_m[element_num];
	direction_cosines[2] = direction_cosine_n[element_num];
	
	// Fill first 3 rows of stiffness matrix
	for(i = 0; i<3; i++){
		for(j = 0; j<3; j++){
			element_stiffness_matricies[((i*6)+j)+element_num_offset] = direction_cosines[i]*direction_cosines[j];
			element_stiffness_matricies[((i*6)+3+j)+element_num_offset] = -1*direction_cosines[i]*direction_cosines[j];
		}
	}
	// Fill last 3 rows of stiffness matrix
	for(i = 0; i<3; i++){
		for(j = 0; j<3; j++){
			element_stiffness_matricies[((i*6)+j+18)+element_num_offset] = -1*direction_cosines[i]*direction_cosines[j];
			element_stiffness_matricies[((i*6)+3+j+18)+element_num_offset] = direction_cosines[i]*direction_cosines[j];
		}
	}
	print_element_stiffness_matrix(element_stiffness_matricies, element_num, 6, 6);
}


void assemble_global_stiffness_matrix(float *element_stiffness_matricies, int *element_connectiviity, int *row_index, float *col_index, float *value, int num_row, int num_col){
	// Takes in all of the element stiffness matricies and then bassed on the color of the element adds them to the element stiffness matrix
	int element, node, dof, node_num;
	for(element = 0; element<num_element; element++){  // Go through each element
		for(node = 0; node<nodes_per_element; node++){ // for each node in the element
			for(dof = 0; dof<dof_per_node; dof++){
				node_num = element_connectiviity[element*dof_per_node + node]; /* calculates the current node using the element_connectivity array [0, 1, 1, 2, 0, 2]*/
				row_index[element*dof_per_node + dof] = node_num + dof;	// global_stiffness_matrix row
				row_index[element*dof_per_node + dof] = node_num + dof; // global_stiffness_matrix col
				int element_num_offset = num_row*num_col*element;
				for(int i = 0; i<3; i++){
					value[element*dof_per_node + dof] = element_stiffness_matricies[(dof*i + i)+element_num_offset]; // global_sitiffness_matrix val
				}
			}
		}
	}

}

void print_global_stiffness_matrix(int *row_index, int *col_index, float *value, int num_node, int dof_per_node){
	printf("The global_stiffness_matrix is:\n");
	for(int i = 0; i<num_node*dof_per_node; i++){
		printf("K(%d, %d) = %f", row_index[i], col_index[i], value[i]);
	}
}

void initalize_problem(int num_node, int num_element, int nodes_per_element){
	// element stifness matrix values
	nodes_x = (float*) malloc(num_node*sizeof(float));
	nodes_y = (float*) malloc(num_node*sizeof(float));
	nodes_z = (float*) malloc(num_node*sizeof(float));
	element_connectiviity = (int*) malloc(num_element*nodes_per_element*sizeof(int));
	direction_cosine_l = (float*) malloc(num_element*sizeof(float));
	direction_cosine_m = (float*) malloc(num_element*sizeof(float));
	direction_cosine_n = (float*) malloc(num_element*sizeof(float));
	element_length = (float*) malloc(num_element*sizeof(float));

	// global stiffness matrix values
	row_index = (int*) malloc(num_node*dof_per_node*sizeof(int));
	col_index = (int*) malloc(num_node*dof_per_node*sizeof(int));
	value = (float*) malloc(num_node*dof_per_node*sizeof(int));

	// TEST MATRIX for generating element stiffness matricies
	element_stiffness_matricies = (float*) malloc(6*6*sizeof(float)*num_element);
	
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

	// perform the numerical integration for all elements
	calc_element_length(num_element, nodes_per_element, element_connectiviity, num_node, nodes_x, nodes_y, nodes_z, element_length);
	calc_direction_cosine(num_element, nodes_per_element, element_connectiviity, num_node, nodes_x, nodes_y, nodes_z, element_length, direction_cosine_l, direction_cosine_m, direction_cosine_n);

	generate_element_stiffness_matricies(direction_cosine_l, direction_cosine_m, direction_cosine_n, 0, element_stiffness_matricies, 6, 6);
	generate_element_stiffness_matricies(direction_cosine_l, direction_cosine_m, direction_cosine_n, 1, element_stiffness_matricies, 6, 6);
	generate_element_stiffness_matricies(direction_cosine_l, direction_cosine_m, direction_cosine_n, 2, element_stiffness_matricies, 6, 6);
	assemble_global_stiffness_matrix(element_stiffness_matricies, element_connectiviity, row_index, col_index, value, 6, 6);
	print_global_stiffness_matrix(row_index, col_index, value, num_node, dof_per_node);

}

void cleanup_problem(){
	free(nodes_x);
	free(nodes_y);
	free(nodes_z);
	free(element_connectiviity);
	free(direction_cosine_l);
	free(element_stiffness_matricies);
}

int main(int argc, char* argv[]){
	// Set up logging
	log_set_level(0);
	log_set_quiet(0);

	// initialize problem
	num_node = 3;
	num_element = 3;
	nodes_per_element = 2;
	initalize_problem(num_node, num_element, nodes_per_element);

	// Free memory and exit gracefully
	cleanup_problem();
	
	// Testing features
	truss_element test_element;
	test_element.A = 6.9;
	test_element.E = 260;
	test_element.node_0[0] = 0; test_element.node_0[1] = 0; test_element.node_0[2] = 0;
	test_element.node_1[0] = 1; test_element.node_1[1] = 0; test_element.node_1[2] = 0;

	printf("aw helllllllllll yeah\n");
	return 0;
}