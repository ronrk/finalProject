#include "Queue.h"

#include "Port.h"
#include "Cars.h"
#include "ErrorHandler.h"

#include <stdlib.h>
#include <stdio.h>


static Car* removeNode(qCar* queue,CarNode *prev, CarNode *toRemove){
  if(!queue||!toRemove) return NULL;

  if(prev ==NULL) {
    // remove front
    queue->front = toRemove->next;
    if(queue->front == NULL) {
      queue->rear = NULL;
    }
  } else {
    prev->next = toRemove->next;
    if(toRemove == queue->rear) {
      queue->rear = prev;
    }
  }

  Car *car = toRemove->data;
  free(toRemove);
  return car;
}

qCar *createQueue() {
  qCar *queue = malloc(sizeof(qCar));
  if(!queue) {
    perror("Failed to allocate memory for queue");
    return NULL;
  }
  initQueue(queue);
  return queue;
}

void destroyQueue(qCar*queue) {
  CarNode* current = queue->front;
  while (current)
  {
    CarNode *tmp = current;
    current = current->next;
    free(tmp);
  }
  free(queue);
  
}

int isEmpty(const qCar *queue) 
{
  return(queue->front == NULL);
}

BOOL enqueue(qCar *queue, Car *car) 
{
  CarNode *newNode = malloc(sizeof(CarNode));
  if(!newNode) {
    perror("Failed to allocate memory for QueueNode");
    return FALSE; // FAILED
  }
  newNode->data = car;
  newNode->next = NULL;

  if(queue->rear == NULL) {
    // empty queue
    queue->front = queue->rear = newNode;
  } else {
    queue->rear->next = newNode;
    queue->rear = newNode;
  }
  return TRUE; //SUCCESS
}

Car *dequeue(qCar *queue) {
  if(isEmpty(queue)) return NULL;
  return removeNode(queue,NULL,queue->front);
}

Car* dequeueByPortType(qCar* queue,PortType portType){
  if(!queue||isEmpty(queue)) {
    printf("[Dequeue] Queue empty or NULL.\n");
    return NULL;
  }

  CarNode* current = queue->front;
  CarNode *prev = NULL;

  while (current)
  {
    if(current->data&&current->data->portType == portType) {
      return removeNode(queue,prev,current);
    }

    prev = current;
    current = current->next;
  }
  char msg[128];
  snprintf(msg,sizeof(msg),"No car in queue matching port type %s",portTypeToStr(portType));
  displayError(UI_WARNING,msg);

  return NULL;
}

Car *getFront(const qCar *queue) {
  if(queue == NULL|| queue->front == NULL) {
    return NULL; //empty queue
  }

  return queue->front->data;
}

void printQueue(const qCar *queue) {
  if(queue == NULL || queue->front == NULL) {
    printf("Queue is empty\n");
    return;
  }

  printf("Cars in queue:\n");
  CarNode *current = queue->front;
  while (current!=NULL)
  {
    printCar(current->data);
    current=current->next;
  }
}

int countQueueItems(const qCar* queue) {
  if(!queue || !queue->front) return 0;

  int count = 0;
  CarNode* current = queue->front;
  while (current)
  {
    count++;
    current = current->next;
  }

  return count;
}

void initQueue(qCar*queue) 
{
  queue->front = NULL;
  queue->rear = NULL;
}
