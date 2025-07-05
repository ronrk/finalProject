#ifndef CARS_H
#define CARS_H

#include "Port.h"
#include "Utilis.h"
#include "BinaryTree.h"

typedef struct Car
{
  char nLicense[LICENSE_SIZE];
  PortType portType;
  double totalPayed;
  Port* pPort;
  BOOL inqueue;
} Car;

int compareCars(const void* a, const void*b);

// print car
void printCar(const void *data);

// parses line from .txt and create Car
Car *parseCarLine(const char *line);

// create a new car
Car *createCar(const char *license, PortType type);

// Destroy car
void destroyCar(void *data);

// search for car
Car* searchCar(BinaryTree *carTree,const char *lisence);

// validate car
BOOL isLicenseValid(const char* license);


#endif