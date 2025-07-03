#ifndef STATION_H
#define STATION_H
#include "Utilis.h"
#include "Queue.h"
#include "Port.h"

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

typedef enum
{
  SEARCH_BY_ID,
  SEARCH_BY_NAME,
  SEARCH_BY_DISTANCE,
  SEARCH_TYPE_COUNT
} SearchType;

typedef struct
{
  SearchType type;
  union
  {
    char *name;
    int id;
  };
  struct
  {
    double userX;
    double userY;
  } location;
} SearchKey;

typedef Station *(*SearchFunc)(BinaryTree *tree, SearchKey *key);

void StationDestroy(void *data);
int compareStation(const void *a, const void *b);
void printStation(const void *data);

Station *insertStation(Station *root, Station *newStation);
void inorderStationTraversal(Station *root);
void destroyStationTree(Station *root);
void *parseStationLine(const char *line);

// search
Station *searchStation(BinaryTree *tree, SearchKey *key, SearchType type);

#endif