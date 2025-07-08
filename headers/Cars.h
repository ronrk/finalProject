#ifndef CARS_H
#define CARS_H

#include "Utilis.h"

typedef struct Port Port;
typedef struct Station Station;
typedef struct BinaryTree BinaryTree;
typedef struct TreeNode TreeNode;

typedef struct Car Car;
struct Car
{
  char nLicense[LICENSE_SIZE];
  PortType portType;
  double totalPayed;
  Port *pPort;  // ptr to port
  BOOL inqueue; // is car in queue?
};

// create a new car
Car *createCar(const char *license, PortType type);
// 
// parses line from .txt and create Car
Car *parseCarLine(const char *line);
// 
// validate car
BOOL isLicenseValid(const char *license);
// compare 2 cars by license number
int compareCarsByLicense(const void *a, const void *b);
// 
// search for car
Car *searchCar(const BinaryTree *carTree, const char *license);
// 
// get the car chargin port
Port *getCarPort(Car *car);
// 
// print car
void printCar(const void *data);
// 
// Destroy car
void destroyCar(void *data);
// 
// find station of queue car
Station *findStationOfQueueCar(const TreeNode *node, const Car *car);
// collects cars in array
void collectCarsInArray(TreeNode* node,Car** carrArr,int* size);
// 
// compare cars array by toal payed
int compareCarsByTotalPayed(const void* a,const void*b);
// 


#endif