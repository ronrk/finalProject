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

// static functions
static char* getNextToken(char* context,const char* fieldName){
  char *token = strtok(NULL, ",");
  if(!token && fieldName) {
    char msg[128];
    snprintf(msg, sizeof(msg), "parseCarLine Missing field: %s", fieldName);
    displayError(ERR_PARSING, msg);
  }
  return token;
}

// public functions
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

Car* parseCarLine(const char* line) 
{
  // COPY OF THE LINE
  char buffer[256];
  strncpy(buffer,line,sizeof(buffer)-1);

  // CREATE NEW CAR DYNAMIC
  Car* car = (Car*)malloc(sizeof(Car));
  if (!car) {
    displayError(ERR_MEMORY, "Failed to allocate memory for Car");
    return NULL;
  }

  // tokenize license
  char* token = strtok(buffer,",");
  if (!token) {
    displayError(ERR_PARSING, "Missing license field");
    free(car);
    return NULL;
  }
  strncpy(car->nLicense, token, LICENSE_SIZE - 1);
    car->nLicense[LICENSE_SIZE - 1] = '\0';
  
  // tokenize portType
  token = getNextToken(buffer,"PortType");
  if (!token) { free(car); return NULL; }
  car->portType = Util_parsePortType(token);

   // tokenize totalPayed
  token = getNextToken(buffer, "TotalPayed");
  car->totalPayed = token ? atof(token) : 0.0;
  
  //tokenize StationID
  getNextToken(buffer, "StationID");  // 

  // tokenize portNum
  getNextToken(buffer, "PortNumber"); // 

  // tokenize inQueue
  token = getNextToken(buffer, "inQueue");
  car->inqueue = (token && strcmp(token, "1") == 0) ? TRUE : FALSE;

  car->pPort = NULL;

  return car;
};

BOOL isLicenseValid(const char* license) {
  return strlen(license) == (LICENSE_SIZE - 1) && isNumericString(license);
}

int compareCarsByLicense(const void* a, const void*b){
  const Car *c1 = (const Car*)a;
  const Car *c2 = (const Car*)b;

  return strcmp(c1->nLicense,c2->nLicense);
}

Car* searchCar(const BinaryTree *carTree,const char *license) {
  if(!carTree||!license) {
    return NULL;
  }
  Car tmp = {0};
  strncpy(tmp.nLicense,license, LICENSE_SIZE- 1);
  tmp.nLicense[LICENSE_SIZE - 1] = '\0';
  return (Car*)searchBST((BinaryTree*)carTree,&tmp);
}

Port* getCarPort(Car* car){
  if(!car) return NULL;
  Port* port = car->pPort;
  if(!port) return NULL;
  return port;
}

void printCar(const void* data) 
{
  Car* car = (Car*) data;
  printf("\nRequested car- \n\t|License number: {%s} |\n",car->nLicense);
  printf("| PortType: %s , TotalPayed: %.2f , inQueue: %u |\n",portTypeToStr(car->portType),car->totalPayed,car->inqueue);
}


void destroyCar(void *data) {
  if(!data)return;
  Car *car = (Car*)data;

  // dynamic fields
  free(car);
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

void collectCarsInArray(TreeNode* node,Car** carrArr,int* count) {
  if(!node) return;;

  collectCarsInArray(node->left,carrArr,count);
  carrArr[(*count)++] = node->data;
  collectCarsInArray(node->right,carrArr,count);

}

int compareCarsByTotalPayed(const void* a,const void*b){
  Car * carA = *(Car**)a;
  Car * carB = *(Car**)b;

  if(carB->totalPayed > carA->totalPayed) return 1;
  else if(carB->totalPayed < carA->totalPayed) return -1;

  return 0;
}







