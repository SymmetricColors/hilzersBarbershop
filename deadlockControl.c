#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <unistd.h>

int num_clients = 0;
int tranks_clients = 0;
int num_chairs = 3;
int num_barbers = 3;
int num_seats_sofa = 4;
int num_total_customers = 20;

pthread_t *thr_costumers;
pthread_t *thr_barbers;

sem_t mutex;
sem_t chair;
sem_t barber;
sem_t costumer;
sem_t payment;
sem_t cash_register;

// sem_t waiting_room[2], sofa[2];
sem_t waiting_room_empty, waiting_room_full, sofa_empty, sofa_full;

char* barbers [13] = {"Jerry Smith", "Nando DK", "Jhowzinho", "Kadinho", "Livinho", "Kevinho", "G15", "Gustta", "Lucas Lucco", "Yuri Martins", "Lan", "WM", "Don Juan"};



// void print_initial_instances(resource resource);
// int num_allocated_instances(resource resource);
// int num_instances_left_to_be_requested(resource resource);
// int num_instances_available(resource resource);
// void insecure_state_blocked_processes(resource resource);

void check_arguments(int argc, char* argv[]);

void up_queue (sem_t *full, sem_t *empty) {
	sem_wait(full);
	sem_post(empty);
}

void down_queue (sem_t *full, sem_t *empty) {
	sem_wait(empty);
	sem_post(full);
}

void shop_is_full(int clientID) {
	printf("The shop is full. No more costumers are allowed to enter. Client %d is leaving the store.\n", clientID);
}

void cutting_hair(int barberID) {
	printf("Barber %s is going to cut the hair.\n", barbers[barberID]);
	sleep(rand()%3);
}

void receiving_payment(int barberID) {
	printf("Barber %s is receiving the payment.\n", barbers[barberID]);
	sleep(rand()%3);
}

void going_to_waiting_room(int clientID) {
	printf("The client %d is entering the waiting room.\n", clientID);
	sleep(rand()%3);
}

void going_to_the_sofa(int clientID) {
	printf("The client %d is seating on the sofa.\n", clientID);
	sleep(rand()%3);
}

void going_to_the_chair(int clientID) {
	printf("Ah, ah, aah. Client %d, now you gonna seat!.\n", clientID);
	sleep(rand()%3);
}

void got_hair_cut(int clientID) {
	printf("The client %d is now avonts and is going to pay the bill.\n", clientID);
	sleep(rand()%3);
}

void client_is_leaving(int clientID) {
	printf("The client %d is tranks and is leaving the shop.\n", clientID);
    ++tranks_clients;
	sleep(rand()%3);
}

void* f_costumer (void *v) {
	int clientID = *(int *) v;

	sem_wait(&mutex);
	if (num_clients >= num_total_customers) {
		sem_post(&mutex);
		shop_is_full(clientID);
	}
	++num_clients;
	sem_post(&mutex);
	down_queue(&waiting_room_full, &waiting_room_empty);
	going_to_waiting_room(clientID);
	down_queue(&sofa_full, &sofa_empty);
	going_to_the_sofa(clientID);
	up_queue(&waiting_room_full, &waiting_room_empty);
	sem_wait(&chair);
	going_to_the_chair(clientID);
	up_queue(&sofa_full, &sofa_empty);
	sem_post(&costumer);
	sem_wait(&barber);
	got_hair_cut(clientID);
	sem_post(&payment);
	sem_wait(&cash_register);
	sem_wait(&mutex);
	--num_clients;
	sem_post(&mutex);
	client_is_leaving(clientID);
}

void* f_barber (void *v) {
	int barberID = *(int *) v;
	
	while(1) {
		sem_wait(&costumer);
		sem_post(&barber);
		cutting_hair(barberID);
		sem_wait(&payment);
		receiving_payment(barberID);
		sem_post(&cash_register);
		sem_post(&chair);
	}
}

/*
 * Checks if the number of command line arguments is correct. The first argument is always the program name
 */
void check_arguments(int argc, char* argv[]) {
	if (argc != 5) {
		printf("Usage: ./<number of processes> <number of ressources> <number of instances per ressource>\n");

		exit(0);
	}
}


int main(int argc, char* argv[]) {
	/*check_arguments(argc, argv);

	num_chairs = atoi(argv[1]);
	num_barbers = atoi(argv[2]);
	num_seats_sofa = atoi(argv[3]);
	num_total_customers = atoi(argv[4]);*/
    srand( (unsigned)time(NULL) );
	thr_costumers = (pthread_t *)malloc(sizeof(pthread_t) * num_total_customers);
	thr_barbers = (pthread_t *)malloc(sizeof(pthread_t) * num_barbers);

	int client[num_total_customers];

	sem_init(&mutex, 0, 1);
	sem_init(&chair, 0, num_chairs);
	sem_init(&barber, 0, 0);
	sem_init(&costumer, 0, 0);
	sem_init(&payment, 0, 0);
	sem_init(&cash_register, 0, 0);

	sem_init(&waiting_room_full, 0, 0);
	sem_init(&waiting_room_empty, 0, num_total_customers - num_seats_sofa);
	sem_init(&sofa_full, 0, 0);
	sem_init(&sofa_empty, 0, num_seats_sofa);

	int i = 0;

    int al = rand()%10;
    int shuffle[num_barbers];
	for (i = 0; i < num_barbers; i++) {
        shuffle[i]= al+i;
		pthread_create(&thr_barbers[i], 0, f_barber, &shuffle[i]);
	}

	for (i = 0; i < num_total_customers; i++) {
		client[i] = i;
		pthread_create(&thr_costumers[i], 0, f_costumer, &client[i]);
	}
   
    
    while (tranks_clients<num_total_customers)
    {
        sleep (1);
    }
	

	free(thr_costumers);
	free(thr_barbers);
	
	return 0;
}
