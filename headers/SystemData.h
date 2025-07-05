#ifndef SYSTEMDATA_H
#define SYSTEMDATA_H

#include "Station.h"
#include "BinaryTree.h"
#include "Cars.h"

typedef struct
{
  BinaryTree carTree;     //  cars binary tree
  BinaryTree stationTree; //  stations binary tree
} SystemData;

typedef struct LineOfCarsEntry
{
  unsigned int stationId;
  char license[LICENSE_SIZE];
} LineOfCarsEntry;


// Functions
// load files
SystemData *loadFiles();
// destroy files
void destroyFiles(SystemData *sys);

// load individual files
int loadStations(BinaryTree *stationTree);
int loadCars(BinaryTree *carTree);
int loadPorts(SystemData *sys);
int loadLineOfCars(SystemData *sys);


#endif