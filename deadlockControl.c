#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <unistd.h>
#include "queue.h"

int num_clients = 0;
int tranks_clients = 0;
//int num_payment = 0;
int num_chairs = 3;
int num_barbers = 3;
int num_seats_sofa = 4;
int num_max_costumers = 20;

pthread_t *thr_costumers;
pthread_t *thr_barbers;

struct Queue *q_waiting_room, *q_sofa; 

sem_t mutex;
sem_t queue_wr;
sem_t queue_s;
sem_t queue_c;
sem_t chair;
sem_t barber;
sem_t costumer;
sem_t payment;
sem_t cash_register;

// sem_t waiting_room[2], sofa[2];
sem_t waiting_room_empty, sofa_empty;

char* barbers [13] = {"Jerry Smith", "Nando DK", "Jhowzinho", "Kadinho", "Livinho", "Kevinho", "G15", "Gustta", "Lucas Lucco", "Yuri Martins", "Lan", "WM", "Don Juan"};



// void print_initial_instances(resource resource);
// int num_allocated_instances(resource resource);
// int num_instances_left_to_be_requested(resource resource);
// int num_instances_available(resource resource);
// void insecure_state_blocked_processes(resource resource);


void shop_is_full(int clientID) {
	printf("C%d: X The shop is full. No more costumers are allowed to enter. Client is leaving the store.\n", clientID);
}

void cutting_hair(int barberID) {
	printf("Barber %s is going to cut the hair.\n", barbers[barberID]);
	sleep(rand()%6+4);
}

void receiving_payment(int barberID) {
	printf("Barber %s is receiving the payment.\n", barbers[barberID]);
	sleep(rand()%4);
}

void going_to_waiting_room(int clientID) {
	if(isFull(q_sofa)){
		printf("C%d: 0 The client is entering the waiting room.\n", clientID);
		sleep(rand()%2);
	}
	sleep(1);
}

void going_to_the_sofa(int clientID) {
	printf("C%d: 1 The client is seating on the sofa.\n", clientID);
	sleep(rand()%2);
}

void going_to_the_chair(int clientID) {
	printf("C%d: 2 Ah, ah, aah. Now you gonna seat!.\n", clientID);
	sleep(rand()%2);
}

void got_hair_cut(int clientID) {
	printf("C%d: 3 The client is now avonts and is going to pay the bill.\n", clientID);
	sleep(rand()%3);
}

void client_is_leaving(int clientID) {
	printf("C%d: 4 The client is tranks and is leaving the shop.\n", clientID);
    ++tranks_clients;
	sleep(rand()%3);
}

void* f_costumer (void *v) {
	int clientID = *(int *) v;
	sem_wait(&mutex);
	if (num_clients >= num_max_costumers) {
		sem_post(&mutex);
		shop_is_full(clientID);
	}
	++num_clients;
	sem_post(&mutex);


	sem_wait(&waiting_room_empty);
	//sem_wait(&queue_wr);
	sem_wait(&queue_wr);
	enqueue(q_waiting_room,clientID);
	going_to_waiting_room(clientID);
	sem_post(&queue_wr);
	//down_queue(&waiting_room_full, &waiting_room_empty);
	
//	sem_post(&queue_wr);
	//sem_post(&queue_wr);
	sem_wait(&sofa_empty);
	//sem_wait(&queue_wr);
	sem_wait(&queue_s);
	clientID = dequeue(q_waiting_room);
	sem_post(&waiting_room_empty);
	enqueue(q_sofa,clientID);
    //down_queue(&sofa_full, &sofa_empty);
	going_to_the_sofa(clientID);
	sem_post(&queue_s);
	
		//up_queue(&waiting_room_full, &waiting_room_empty);
		//sem_post(&queue_wr);
	//}


    sem_wait(&chair);
    sem_wait(&queue_c);
	clientID = dequeue(q_sofa);
	going_to_the_chair(clientID);
	sem_post(&queue_c);
	sem_post(&sofa_empty);
	//up_queue(&sofa_full, &sofa_empty);
    //sem_post(&queue);
	sem_post(&costumer);
	sem_wait(&barber);
	got_hair_cut(clientID);
	sem_post(&payment);
	//sem_post(&mutex);
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
		sem_post(&chair);
		//sem_wait(&payment);
		//while(num_payment>0)
		sem_wait(&payment);
		receiving_payment(barberID);
		sem_post(&cash_register);

		
		
	}
}

/*
 * Checks if the number of command line arguments is correct. The first argument is always the program name
 */
int check_arguments(int argc, char* argv[]) {
	if (argc != 5) {
		//printf("Usage: ./<number of processes> <number of ressources> <number of instances per ressource>\n");

		//exit(0);
		return 0;
	}
	return 1;
}


int main(int argc, char* argv[]) {
	if(check_arguments(argc, argv)){

		num_chairs = atoi(argv[1]);
		num_barbers = atoi(argv[2]);
		num_seats_sofa = atoi(argv[3]);
		num_max_costumers = atoi(argv[4]);
	}	
    srand( (unsigned)time(NULL) );
	thr_costumers = (pthread_t *)malloc(sizeof(pthread_t) * num_max_costumers);
	thr_barbers = (pthread_t *)malloc(sizeof(pthread_t) * num_barbers);
	
	q_waiting_room = createQueue(num_max_costumers-num_seats_sofa);
	q_sofa = createQueue(num_seats_sofa);
	//q_payment = createQueue(num_max_costumers);

	int client[num_max_costumers];

	sem_init(&mutex, 0, 1);
	sem_init(&chair, 0, num_chairs);
	sem_init(&barber, 0, 0);
	sem_init(&costumer, 0, 0);
	sem_init(&payment, 0, 0);
	sem_init(&cash_register, 0, 0);
    sem_init(&queue_wr,0,1);
    sem_init(&queue_s,0,1);
    sem_init(&queue_c,0,1);
	sem_init(&waiting_room_empty, 0, num_max_costumers - num_seats_sofa);
	sem_init(&sofa_empty, 0, num_seats_sofa);

	int i = 0;

    int al = rand()%10;
    int shuffle[num_barbers];


	for (i = 0; i < num_max_costumers; i++) {
		client[i] = i;

		pthread_create(&thr_costumers[i], 0, f_costumer, &client[i]);
	}

    for (i = 0; i < num_barbers; i++) {
        shuffle[i]= al+i;
		pthread_create(&thr_barbers[i], 0, f_barber, &shuffle[i]);
	}
    //sleep(11000);
    for (i = 0; i < num_max_costumers; i++) {

		pthread_join(thr_costumers[i], NULL);
	}
	

	free(thr_costumers);
	free(thr_barbers);
	
	return 0;
}
