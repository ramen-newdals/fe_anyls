#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include "logging.h"
#include <immintrin.h>
#include <time.h>

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
int *element_color;							// The elements color for parallel assembly

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
	//print_element_stiffness_matrix(element_stiffness_matricies, element_num, 6, 6);
}


void initialize_global_stiffness_matrix(int *element_connectiviity, int *row_index, float *value, int num_values){
	for(int i = 0; i<num_values; i++){
		row_index[i] = 0;
		col_index[i] = 0;
		value[i] = 0;
	}
}

void assemble_global_stiffness_matrix(float *element_stiffness_matricies, int *element_connectiviity, int *row_index, float *col_index, float *value, int dof_per_node){
	// Takes in all of the element stiffness matricies and then bassed on the color of the element adds them to the element stiffness matrix
	int element, node, dof, node_value_offset;
	int *node_num; // holds the node numbers for each node in the element
	int *node_row_offset, *node_col_offset; // holds the row and colum offsets for each node
	node_num = (int *)malloc(nodes_per_element*sizeof(int));
	node_row_offset = (int *)malloc((nodes_per_element+1)*sizeof(int));
	node_col_offset = (int *)malloc((nodes_per_element+1)*sizeof(int));

	node_value_offset = dof_per_node*nodes_per_element*dof_per_node; // The offset in the local stiffness matrix to get the next regions value for the same row col movment

	for(element = 0; element<num_element; element++){  // Go through each element
		for(node = 1; node<=nodes_per_element; node++){
			node_num[node] = element_connectiviity[element*nodes_per_element + node]; /* calculates the current node using the element_connectivity array [0, 1, 1, 2, 0, 2]*/;
			node_col_offset[node] = node_num[node]*dof_per_node;
			node_row_offset[node] = node_num[node]*dof_per_node;
		}
		/* The sparsity pattern can not be known "exatly" at compile time, so to overcome this will malloc the upper-bounded size for the number of elements in the sparse matrix!!!
			For accessing the sparse matrix entities just do it off the dof_num and the global_node_num as this will allow for the easseist access pattern and with a colored
			element connectivity graph will still prevent data-races*/
		//int element_num_offset = num_row*num_col*element;
		for(node = 0; node<nodes_per_element; node++){
			for(int row = 0; row<dof_per_node; row++){
				for(int col = 0; col<3; col++){
					printf("elemnent:%d, node:%d\n", element, node_num[node]);
					printf("row_index[%d]=%d\n", (element + (node*dof_per_node) + row + col), (node_num[node]*dof_per_node + row));
					printf("col_index[%d]=%d\n", (element + (node*dof_per_node) + row + col), (node_num[node]*dof_per_node + col));
					printf("value[%d]=%d\n", (element*dof_per_node + row + col), (element_stiffness_matricies[(row*dof_per_node + col) + (dof_per_node*node)]));
					printf("==========================\n");
					//row_index[element*dof_per_node + row + col] = node_num[node]*dof_per_node + row;	// global_stiffness_matrix row for nodes 0th region
					//row_index[element*dof_per_node + row + col] = node_num[node]*dof_per_node + row + node_row_offset[node + 1]; // global_stiffness_matrix row for nodes 1st region
					//col_index[element*dof_per_node + row + col] = node_num[node]*dof_per_node + col; // global_stiffness_matrix col for nodes 0th region
					//col_index[element*dof_per_node + row + col] = node_num[node]*dof_per_node + col + node_col_offset[node + 1]; // global_stiffness_matrix col for nodes 1st region
					//value[element*dof_per_node + row + col] += element_stiffness_matricies[(row*dof_per_node + col) + (dof_per_node*node)]; // global_sitiffness_matrix val for nodes 0th region
					//value[element*dof_per_node + row + col] += element_stiffness_matricies[(row*dof_per_node + col) + (dof_per_node*node) + node_value_offset]; // global_sitiffness_matrix val for nodes 1st region
				}
			}
		}
	}
}


void print_global_stiffness_matrix(int *row_index, int *col_index, float *value, int num_node){
	printf("The global_stiffness_matrix is:\n");
	for(int i = 0; i<num_node; i++){
		printf("K(%d, %d) = %f\n", row_index[i], col_index[i], value[i]);
	}
}

void generate_mesh(int num_node, int num_element, int *element_connectiviity, float *nodes_x, float *nodes_y, float *nodes_z){
	// Given the number of nodes calulate the number of elements generate the trivial truss tiled
	int num_truss;
	num_truss = num_element/3;

	// Generate the truss nodes and element connectivity
	for(int n = 0; n<num_truss; n++){
		// NODE 0
		nodes_x[n*3] = n; // Add the 0th node in the truss x-component
		nodes_y[n*3] = 0; // Add the 0th node in the truss y-compoent
		nodes_z[n*3] = 0; // Add the 0th node in the truss z-compoent
		// NODE 1
		nodes_x[n*3 + 1] = n+1; // Add the 1st node in the truss x-component
		nodes_y[n*3 + 1] = 1; // Add the 1st node in the truss y-compoent
		nodes_z[n*3 + 1] = 0; // Add the 1st node in the truss z-compoent
		// NODE 2
		nodes_x[n*3 + 2] = n; // Add the 2nd node in the truss x-component
		nodes_y[n*3 + 2] = 0; // Add the 2nd node in the truss y-compoent
		nodes_z[n*3 + 2] = 0; // Add the 2nd node in the truss z-compoent

		// Element connectivity
		element_connectiviity[n*2] = n; element_connectiviity[(n*2)+1] = n+1; // Element 0
		element_connectiviity[(n*2)+2] = n; element_connectiviity[(n*2)+3] = n+2; // Element 1
		element_connectiviity[(n*2)+4] = n+1; element_connectiviity[(n*2)+5] = n+2; // Element 2
	}

	// Print out data for testing
	if(0){
		for(int i = 0; i<num_node; i++){
			printf("Node %d x = %f, y = %f\n", i, nodes_x[i], nodes_y[i]);
		}
		for(int i = 0; i<num_element; i++){
			printf("Element %d: node[0]=%d, node[1]=%d\n", i, element_connectiviity[i*2], element_connectiviity[(i*2)+1]);
		}
	}
}

void initalize_problem(int num_node, int num_element, int nodes_per_element, int dof_per_node){
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
	row_index = (int*) malloc((num_node*dof_per_node)*(num_node*dof_per_node)*sizeof(int));
	col_index = (int*) malloc((num_node*dof_per_node)*(num_node*dof_per_node)*sizeof(int));
	value = (float*) malloc((num_node*dof_per_node)*(num_node*dof_per_node)*sizeof(int));

	// MATRIX for generating element stiffness matricies
	element_stiffness_matricies = (float*) malloc(6*6*sizeof(float)*num_element);
	
	// // Fill arrays with sample data
	// // X-Node data
	// nodes_x[0] = 0; nodes_x[1] = 1; nodes_x[2] = 0; nodes_x[3] = 1;
	// // Y-Node data
	// nodes_y[0] = 0; nodes_y[1] = 0; nodes_y[2] = 1; nodes_y[3] = 1;
	// // Z-Node data
	// nodes_z[0] = 0;	nodes_z[1] = 0; nodes_z[2] = 0; nodes_y[3] = 0;
	// // Setting up the element connectivity
	// element_connectiviity[0] = 0; element_connectiviity[1] = 1; // Element 0
	// element_connectiviity[2] = 0; element_connectiviity[3] = 2; // Element 1
	// element_connectiviity[4] = 1; element_connectiviity[5] = 2; // Element 2

	generate_mesh(num_node, num_element, element_connectiviity, nodes_x, nodes_y, nodes_z);

	// perform the numerical integration for all elements
	calc_element_length(num_element, nodes_per_element, element_connectiviity, num_node, nodes_x, nodes_y, nodes_z, element_length);
	calc_direction_cosine(num_element, nodes_per_element, element_connectiviity, num_node, nodes_x, nodes_y, nodes_z, element_length, direction_cosine_l, direction_cosine_m, direction_cosine_n);

	for(int i = 0; i<num_element; i++){
		generate_element_stiffness_matricies(direction_cosine_l, direction_cosine_m, direction_cosine_n, i, element_stiffness_matricies, 6, 6);
		//assemble_global_stiffness_matrix(element_stiffness_matricies, element_connectiviity, row_index, col_index, value, 3);
		//print_global_stiffness_matrix(row_index, col_index, value, 18);
	}

}

void cleanup_problem(){
	free(nodes_x);
	free(nodes_y);
	free(nodes_z);
	free(element_connectiviity);
	free(direction_cosine_l);
	free(element_stiffness_matricies);
	free(row_index);
	free(col_index);
	free(value);
}

int main(int argc, char* argv[]){
	// Set up logging
	log_set_level(0);
	log_set_quiet(0);

    // struct timespec ts;
    // timespec_get(&ts, TIME_UTC);
    // char buff[100];
    // strftime(buff, sizeof buff, "%D %T", gmtime(&ts.tv_sec));
    // printf("Current time: %s.%09ld UTC\n", buff, ts.tv_nsec);
    // printf("Raw timespec.time_t: %jd\n", (intmax_t)ts.tv_sec);
    // printf("Raw timespec.tv_nsec: %09ld\n", ts.tv_nsec);  

	// initialize problem
	num_node = 9;
	num_element = 12;
	nodes_per_element = 2;
	dof_per_node = 3;
	initalize_problem(num_node, num_element, nodes_per_element, dof_per_node);

	// Free memory and exit gracefully
	cleanup_problem();
	
	// Testing features
	truss_element test_element;
	test_element.A = 6.9;
	test_element.E = 260;
	test_element.node_0[0] = 0; test_element.node_0[1] = 0; test_element.node_0[2] = 0;
	test_element.node_1[0] = 1; test_element.node_1[1] = 0; test_element.node_1[2] = 0;

	// time_t end = time(0);

	// double seconds = difftime(start, end);
	printf("aw helllllllllll yeah\n");
	// printf("%.f seconds have passed since the beginning of the month.\n", seconds);
	return 0;
}