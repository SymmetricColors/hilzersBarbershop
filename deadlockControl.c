#include <stdio.h>
#include <stdlib.h>
#include "queue.h"


int num_chairs = 3;
int num_barbers = 3;
int num_seats_sofa = 4;
int num_total_customers = 20;


// void print_initial_instances(resource resource);
// int num_allocated_instances(resource resource);
// int num_instances_left_to_be_requested(resource resource);
// int num_instances_available(resource resource);
// void insecure_stete_blocked_processes(resource resource);

void check_arguments(int argc, char* argv[]);


int main(int argc, char* argv[]) {
	check_arguments(argc, argv);

	int num_processes = atoi(argv[1]);
	int num_ressources = atoi(argv[2]);
	int num_instances_per_resource = atoi(argv[3]);

	struct Queue* sofa = createQueue(num_seats_sofa);
	struct Queue* standing_line = createQueue(num_total_customers - num_seats_sofa);

	return 0;
}

/*
 * Checks if the number of command line arguments is correct. The first argument is always the program name
 */
void check_arguments(int argc, char* argv[]) {
	if (argc != 4) {
		printf("Usage: ./<number of processes> <number of ressources> <number of instances per ressource>\n");

		exit(0);
	}
}