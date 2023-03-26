#include <stdio.h>
#include <stdlib.h>
#include <math.h>

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
		//putchar(c);
	}

	fclose(input_file);
}

int main(int argc, int* argv[]){
	printf("Hello World\n");

	rod_elements_1d problem_1; // This is filled with garbage data for now
	problem_1.num_loaded_nodes = 70;
	problem_1.num_els = 69;
	problem_1.np_types = 1;
	problem_1.prop = 100000.0;
	problem_1.element_array = malloc(problem_1.num_els*sizeof(float));
	for(int i = 0; i<problem_1.num_els; i++){
		problem_1.element_array[i] = 4.20;
	}
	problem_1.num_restrained_nodes = 4;
	problem_1.restrained_node_numbers = malloc(problem_1.num_restrained_nodes*sizeof(int));
	problem_1.restrained_node_values = malloc(problem_1.num_restrained_nodes*sizeof(float));
	for(int i = 0; i<problem_1.num_restrained_nodes; i++){
		problem_1.restrained_node_numbers[i] = i;
		problem_1.restrained_node_values[i] = 6.9;
	}
	problem_1.num_loaded_nodes = 6;
	problem_1.loaded_node_numbers = malloc(problem_1.num_loaded_nodes*sizeof(int));
	problem_1.loaded_node_values = malloc(problem_1.num_loaded_nodes*sizeof(float));
	for(int i = 0; i<problem_1.num_loaded_nodes; i++){
		problem_1.loaded_node_numbers[i] = i;
		problem_1.loaded_node_values[i] = 1.87;
	}
	problem_1.fixed_freedoms = 187;

	print_problem_parameters(problem_1);

	read_problem_parameters("p41_1.dat");

	return 0;
}