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
	float* nodal_freedom_array[2];	// wtf in the book. The restrained node number and a zero following it
	int num_loaded_nodes;
	int* loaded_node_numbers;
	float* loaded_node_loads;
	int fixed_freedoms; // fixed_freedoms. Used for displacment B.C

	/*NOTE: 
	The default for 1d_rod elements is that nodes are not restrained
	
	For thesse types of 1d problems it is simple to generate the steering vector g*
	
	*/ 
} rod_elements_1d;

void print_problem_paramaters(rod_elements_1d problem_paramaters){
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
}

int read_params(char* file_name){
	FILE* input_file;
	fopen(file_name, "w+");
	// Read the 
	
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
	print_problem_paramaters(problem_1);


	return 0;
}