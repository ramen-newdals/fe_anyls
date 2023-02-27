#include <stdio.h>
#include <stdlib.h>
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


typedef struct truss_element{

} truss_element;

void local_truss_assembly(){
	/*Takes input a truss_element */
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

int read_problem_parameters(char* file_name){
	FILE* input_file;
	input_file = fopen(file_name, "r");
	int c;
	// read the input file (The format is a bit shit, but the book follows it 0__o)
	while((c = fgetc(input_file)) != EOF){
		printf("%d", (int) c);
		putchar(c);
	}

	fclose(input_file);
	return 0;
}

int main(int argc, char* argv[]){
	log_set_level(0);
	log_set_quiet(0);
	log_debug("debug");
	log_trace("hello%s", "World");

	return 0;
}