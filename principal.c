/*
 * principal.c
 *
 *  Created on: 24 oct. 2019
 *      Author: antonio
 */

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define TAM_BUFFER 10
#define NUM_ITEMS 20

typedef struct{
	char buffer[TAM_BUFFER];
	int indexP, indexC;
	pthread_mutex_t mutex;
	sem_t sem_huecos;
	sem_t sem_items;

}TBuffer;

char generarItem(){
	char c;
	c = 65 + rand()%26;
	printf("\nProductor --> %c", c);
	return c;
}

void consumirItem(char c){
	printf("\nConsumidor --> %c", c);
}

void *productor(void *d){
	int i, j;
	TBuffer *buf = (TBuffer *) d;
	char c;

	for (i = 0; i < NUM_ITEMS; i++){
		c = generarItem();
		sem_wait(&buf->sem_huecos);
		pthread_mutex_lock(&buf->mutex);
//		Colocamos el char dentro del buffer
		buf->buffer[buf->indexP] = c;
		buf->indexP = (buf->indexP + 1)%TAM_BUFFER;	//	Avanza de forma circular
		puts("");
		pthread_mutex_unlock(&buf->mutex);
		sem_post(&buf->sem_items);	//	Avisa de que ha dejado un hueco
		for (j = 0; j < TAM_BUFFER; j++)
			printf("\t%c", buf->buffer[j]);
		sleep(rand()%2);
	}

	return NULL;
}

void *consumidor (void *d){
	int i, j;
	TBuffer *buf = (TBuffer *) d;
	char c;

	for (i = 0; i < NUM_ITEMS; i++){
		sem_wait(&buf->sem_items);
		pthread_mutex_lock(&buf->mutex);
//		Consumimos el char dentro del buffer
		c = buf->buffer[buf->indexC];
		buf->indexC = (buf->indexC + 1)%TAM_BUFFER;
		pthread_mutex_unlock(&buf->mutex);
		sem_post(&buf->sem_huecos);
		consumirItem(c);
		puts("");
		for (j = 0; j < TAM_BUFFER; j++)
			printf("\t%c", buf->buffer[j]);
		sleep(rand()%3);
	}

	return NULL;
}

int main(){
	srand(time(NULL));
	TBuffer buf;
	buf.buffer[TAM_BUFFER] = ('*');
	pthread_t prod, consum;

//	Init estructura
	pthread_mutex_init(&buf.mutex, 0);
	buf.indexC = buf.indexP = 0;
	sem_init(&buf.sem_huecos, 0, TAM_BUFFER);
	sem_init(&buf.sem_items, 0, 0);

//	Lanzar los hilos
	pthread_create(&prod, 0, productor, &buf);
	pthread_create(&consum, 0, consumidor, &buf);

	pthread_join(prod, 0);
	pthread_join(consum, 0);

//	Liberar recursos
	sem_destroy(&buf.sem_huecos);
	sem_destroy(&buf.sem_items);

	pthread_mutex_destroy(&buf.mutex);

}
