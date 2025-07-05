#include "../headers/Utilis.h"
#include "../headers/BinaryTree.h"
#include "../headers/Cars.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int compareCars(const void* a, const void*b){
  const Car *c1 = (const Car*)a;
  const Car *c2 = (const Car*)b;

  return strcmp(c1->nLicense,c2->nLicense);
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
    perror("Failed Allocation Memory to Car\n");
    return NULL;
  }
  // SPLIT THE LINE BY ","
  char *token = strtok(buffer,",");
  // 
  // nLICENSE
  if(token) {
    strncpy(car->nLicense,token,8);
    car->nLicense[8] = '\0';
  }
  // PortType
  token = strtok(NULL,",");
  car->portType = Util_parsePortType(token);
  
  // TotalPayed
  token = strtok(NULL,",");
  if(token) 
    car->totalPayed = atof(token);

  // StationId *****
  token = strtok(NULL,",");
  // PortNumber *****
  token = strtok(NULL,",");
  // inqueue
  token = strtok(NULL,",");
  if(token) {
    car->inqueue = strcmp(token,"1") == 0 ? TRUE : FALSE;
  }
  // nPort *****
  car->pPort = NULL;

  return car;
};

void printCar(const void* data) 
{
  Car* car = (Car*) data;
  printf("License: %s | PortType: %d | TotalPayed: %.2f | inQueue: %u\n",car->nLicense,car->portType,car->totalPayed,car->inqueue);
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

void destroyCar(void *data) {
  if(!data)return;
  Car *car = (Car*)data;

  // dynamic fields
  free(car);
}

Car* searchCar(BinaryTree *carTree,const char *lisence) {
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
  return(strlen(license) == 8 && strspn(license,"0123456789")==8);
}
