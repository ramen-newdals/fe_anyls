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

void calc_element_length(int num_element, int nodes_per_element, int *element_connectiviity, int num_node, float *nodes_x, float *nodes_y, float *nodes_z){
	// calulate the element length for each element
	float x_i, x_j, y_i, y_j, z_i, z_j, length_e;
	printf("Calculating Element Lengths\n");
	for(int i = 0; i<num_element; i++){
		printf("================element %d================\n", i);
		x_i = nodes_x[element_connectiviity[i*nodes_per_element]]; 
		printf("x_i = %f\n", x_i);
		x_j = nodes_x[element_connectiviity[(i*nodes_per_element)+1]];
		printf("x_j = %f\n", x_j);
		y_i = nodes_y[element_connectiviity[i*nodes_per_element]];
		printf("y_i = %f\n", y_i);
		y_j = nodes_y[element_connectiviity[(i*nodes_per_element)+1]];
		printf("y_j = %f\n", y_j);
		z_i = nodes_z[element_connectiviity[i*nodes_per_element]];
		printf("z_i = %f\n", z_i);
		z_j = nodes_z[element_connectiviity[(i*nodes_per_element)+1]];
		printf("z_j = %f\n", z_i);
		length_e = sqrt(((x_j - x_i)*(x_j - x_i)) + ((y_j - y_i)*(y_j - y_i)) + ((z_j - z_i)*(z_j - z_i)));
		printf("l_e = %f\n", length_e);
	}
}

void calc_direction_cosine(int num_element, int nodes_per_element, int *element_connectiviity, int num_node, float *nodes_x, float *nodes_y, float *nodes_z, float *direction_cosine_l){
	// For each element calculate the l, m, and n direction cosines
	for(int i = 0; i<num_element*nodes_per_element; i++){
		printf("%d\n", element_connectiviity[i]); // Print the element connectivity array
	}
}

void print_problem_parameters(rod_elements_1d problem_paramaters){
	printf("Number of Node: %d\n", problem_paramaters.num_node);
	printf("Number of Elements: %d\n",problem_paramaters.num_els);
	printf("np_types: %d\n", problem_paramaters.np_types);
	printf("Prop: %f\n", problem_paramaters.prop);
	// print out the element size array
	for(int i = 0; i<problem_paramaters.num_els; i++){
		if(i == 0){
			printf("Element Array:\n");
			printf("element_array[%d]: %f\n", i, problem_paramaters.element_array[i]);
		}
		printf("element_array[%d]: %f\n", i, problem_paramaters.element_array[i]);
	}
	// print out the restrained node array
	for(int i = 0; i<problem_paramaters.num_restrained_nodes; i++){
		if(i == 0){
			printf("Restrained Node Arraay:\n");
			printf("restrained_node[%d] = %f\n", problem_paramaters.restrained_node_numbers[i], problem_paramaters.restrained_node_values[i]);
		}
		printf("restrained_node[%d] = %f\n", problem_paramaters.restrained_node_numbers[i], problem_paramaters.restrained_node_values[i]);
	}
	// print out the loaded node array
	for(int i = 0; i<problem_paramaters.num_loaded_nodes; i++){
		if(i == 0){
			printf("Loaded Node Arraay:\n");
			printf("loaded_node[%d] = %f\n", problem_paramaters.loaded_node_numbers[i], problem_paramaters.loaded_node_values[i]);
		}
		printf("loaded_node[%d] = %f\n", problem_paramaters.loaded_node_numbers[i], problem_paramaters.loaded_node_values[i]);
	}
	// print out the fixed freedoms? 
	printf("fixed freedom: %d\n", problem_paramaters.fixed_freedoms);	
}

int read_mesh_paramaters(char* file_name){
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

int read_mesh_nodes(char* mesh_node_data, float* node_x_pos, float* node_y_pos, float node_z_pos){
	/*  x_pos, y_pos, z_pos are arrays of size: sizeof(float)*num_nodes 
		each array holds the x, y, locations of each node
		going through the mesh_node data need to populate the arrays
	*/

	int num_entity_blocks, // The number of blocks?
		 	num_nodes, // The total number of nodes in the mesh
			min_node_tag, max_node_tag; // The start and end node index tags
	
	int c; // Current charecter in the string
	for(int i = 0; i<2; i++){
		while(c != EOF){
			num_entity_blocks = (long) c;
		}
	}

	for(long i = 0; i<num_nodes; i++){

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

	calc_direction_cosine(num_element, nodes_per_element, element_connectiviity, num_node, nodes_x, nodes_y, nodes_z, direction_cosine_l);
	calc_element_length(num_element, nodes_per_element, element_connectiviity, num_node, nodes_x, nodes_y, nodes_z);
	free(nodes_x);
	free(nodes_y);
	free(nodes_z);
	free(element_connectiviity);
	free(direction_cosine_l);
	
	printf("aw helllllllllll yeah\n");
	return 0;
}