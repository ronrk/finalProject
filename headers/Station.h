#ifndef STATION_H
#define STATION_H

#include "Utilis.h"
#include "Queue.h"
#include "Port.h"
#include "BinaryTree.h"

typedef struct Station Station;
struct Station
{
  unsigned int id;
  char *name;
  int nPorts;
  Coord coord;
  Port *portsList;
  int nCars;
  qCar *qCar;
  Station *left;
  Station *right;
};

Station *StationCreate(unsigned int id, const char *name, int nPorts, Coord coord);

typedef Station *(*SearchFunc)(BinaryTree *tree, SearchKey *key);

void StationDestroy(void *data);
int compareStation(const void *a, const void *b);
void printStation(const void *data);
void printFullStation(const void *data);

void *Station_parseLine(const char *line);

unsigned int generateUniqueStationId(BinaryTree *tree);

BOOL enqueueCarToStationQueue(Station *station, Car *car);

// search
Station *searchStation(BinaryTree *tree, SearchKey *key, SearchType type);
Car *searchCarInAllQueues(const BinaryTree *stationTree, const char *license);

#endif