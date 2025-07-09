#ifndef STATION_H
#define STATION_H

#include "Utilis.h"

typedef struct Port Port;
typedef struct Car Car;
typedef struct qCar qCar;
typedef struct BinaryTree BinaryTree;
typedef struct TreeNode TreeNode;

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
};

Station *StationCreate(unsigned int id, const char *name, int nPorts, Coord coord);

typedef Station *(*SearchFunc)(const BinaryTree *tree, SearchKey *key);

void StationDestroy(void *data);
int compareStationById(const void *a, const void *b);
void printStation(const void *data);
void printFullStation(const void *data);

void *Station_parseLine(const char *line);

unsigned int generateUniqueStationId(BinaryTree *tree);

BOOL enqueueCarToStationQueue(Station *station, Car *car);

void addPortToStation(Station *station, Port *port,BOOL increment);

// search
Station *searchStation(const BinaryTree *tree, SearchKey *key);
Car *searchCarInAllQueues(const BinaryTree *stationTree, const char *license);

Station *findStationByPort(const BinaryTree *tree, const Port *port);

Station *findStationByCar(BinaryTree *stationTree, Car *car);

// ////////////////
// ////////////////
// ////////////////
// ////////////////

void printStationSummary(const void *data);
#endif