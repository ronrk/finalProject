#include "Cars.h"
#include "Port.h"
#include "ErrorHandler.h"
#include "Utilis.h"
#include "BinaryTree.h"
#include "Station.h"
#include "Queue.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int compareCars(const void* a, const void*b){
  const Car *c1 = (const Car*)a;
  const Car *c2 = (const Car*)b;

  return strcmp(c1->nLicense,c2->nLicense);
}

static char* getNextToken(char* context,const char* fieldName){
  char *token = strtok(NULL, ",");
  if(!token && fieldName) {
    displayError(ERR_PARSING,"parseCarLine Missing name field '%s'");
  }
  return token;
}

Car* parseCarLine(const char* line) 
{
  // COPY OF THE LINE
  char buffer[256];
  strcpy(buffer,line);

  // CREATE NEW CAR DYNAMIC
  Car *car = malloc(sizeof(Car));
  if(!car) 
  {
    displayError(ERR_MEMORY,"Failed Allocation Memory to Car");
    return NULL;
  }
  // SPLIT THE LINE BY ","
  char *token = strtok(buffer,",");

  // 
  // nLICENSE
  if(token) {
    strncpy(car->nLicense,token,8);
    car->nLicense[8] = '\0';
  } else {
    displayError(ERR_PARSING,"carParseLine Missing license field");
    free(car);
    return NULL;
  }
  
 token = strtok(NULL, ","); // portType
  if(!token) {
    displayError(ERR_PARSING, "Missing PortType field in car data");
    free(car);
    return NULL;
  }
  car->portType = Util_parsePortType(token);

  token = strtok(NULL, ","); // totalPayed
  car->totalPayed = token ? atof(token) : 0.0;

  token = strtok(NULL, ","); // stationId (skip or handle if needed)
  token = strtok(NULL, ","); // portNumber (skip or handle if needed)

  token = strtok(NULL, ","); // inQueue
  car->inqueue = (token && strcmp(token, "1") == 0) ? TRUE : FALSE;

  car->pPort = NULL;

  return car;
};

void printCar(const void* data) 
{
  Car* car = (Car*) data;
  printf("\nRequested car- \n\t|License number: {%s} |\n",car->nLicense);
  printf("| PortType: %s , TotalPayed: %.2f , inQueue: %u |\n",portTypeToStr(car->portType),car->totalPayed,car->inqueue);
}

Car *createCar(const char *license, PortType type) {
  Car *car = malloc(sizeof(Car));
  if(!car) {
    perror("Failed alocate memory on createCar\n");
    return NULL;
  }
  strncpy(car->nLicense,license,8);
  car->nLicense[8] = '\0';
  car->portType = type;
  car->totalPayed = 0.0;
  car->pPort = NULL;
  car->inqueue = FALSE;
  return car;
}

Port* getCarPort(Car* car){
  Port* port = car->pPort;
  if(!port) return NULL;
  return port;
}

void destroyCar(void *data) {
  if(!data)return;
  Car *car = (Car*)data;

  // dynamic fields
  free(car);
}

Car* searchCar(const BinaryTree *carTree,const char *lisence) {
  if(!carTree||!lisence||!carTree->root) {
    return NULL;
  }

  TreeNode *current = carTree->root;

  while (current!=NULL)
  {
    Car *car=(Car *)current->data;
    int cmp= strcmp(lisence,car->nLicense);

    if(cmp ==0 ) {
      return car;
    }
    else if(cmp<0) {
      current = current->left;
    }
    else  {
      current = current->right;
    }
  }
  
  return NULL;
}

BOOL isLicenseValid(const char* license) {
  return strlen(license) == (LICENSE_SIZE - 1) && isNumericString(license);
}

Station* findStationOfQueueCar(const TreeNode* node,const Car* car) {
  if(!node || !car) return NULL;
  Station* station =  (Station*) node->data;
  CarNode* current = station->qCar->front;

  while (current)
  {
    if(current->data == car) {
      return station;
    }
    current = current->next;

  }
  Station* stationFound = findStationOfQueueCar(node->left,car);
  if(stationFound) return stationFound;
  return findStationOfQueueCar(node->right,car);
  
}