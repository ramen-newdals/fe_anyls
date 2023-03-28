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

void generate_stiffness_matricies(float *direction_cosine_l, float *direction_cosine_m, float*direction_cosine_n, int column_num, float *stiffness_matrix){
	//	INPUTS: direction_cosine_l, direction_cosine_m, direction_cosine_n, column_num, stiffness_column
	//  -------
	int i, j; // stiffness_matrix row and colum indicies
	switch (column_num)
	{
	case /* constant-expression */:
		/* code */
		break;
	
	default:
		break;
	}
}

int read_mesh(char* file_name){
	// Reads in a .msh file to store the nodes and elements into arrays
	FILE* mesh_file = fopen(file_name, "rb"); // Reading binary version of the mesh file remove the b for ASCI 
	fseek(mesh_file, 0, SEEK_END);
	long fsize = ftell(mesh_file);
	fseek(mesh_file, 0, SEEK_SET);
	char *string = malloc(fsize+1);

	fread(string, fsize, 1, mesh_file);
	fclose(mesh_file);
	string[fsize] = 0;

	// Find $Nodes section of string
	char* nodes_start_key = "$Nodes";
	int nodes_start_offset = 7;
	char* nodes_end_key = "$EndNodes";
	char* nodes_start = strstr(string, nodes_start_key);
	char* nodes_end = strstr(string, nodes_end_key);
    if((!nodes_start) || (!nodes_end)){
		printf("Mesh file does not contain a $Nodes section check file and try again\n");
		return -1;
	}
	// Building the number of nodes array

	printf("Nodes Section starts at: %d\n Nodes Section ends at: %d\n", nodes_start-string, nodes_end-string);
	// Itterate throuhg each chatecter of the string and assign them to the approptiate arrays
	for(long i = ((nodes_start-string) + nodes_start_offset); i<(nodes_end-string); i++){
		printf("%c", string[i]);
		// The first row of $Nodes contains the following entries
	}
	return 0;
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

	free(nodes_x);
	free(nodes_y);
	free(nodes_z);
	free(element_connectiviity);
	free(direction_cosine_l);
	
	printf("aw helllllllllll yeah\n");
	return 0;
}