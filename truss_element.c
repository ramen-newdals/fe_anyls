#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include "logging.h"

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


typedef struct truss_element_SERIAL{

} truss_element_SERIAL;

typedef struct truss_element_PARALELL{

} truss_element_PARALELL;

void local_truss_assembly_SERIAL(float vertex_table, truss_element_SERIAL truss_elment){
	/*Takes input a truss_element  and assembles its stiffness matrix*/
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
	read_mesh_paramaters("/home/ramen_newdals/Documents/ECE1755/project/fe_anyls/mesh/sample_mesh_0_bin.msh");

	return 0;
}