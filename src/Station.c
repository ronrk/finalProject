#include "../headers/Station.h"
#include "../headers/Queue.h"
#include "../headers/Port.h"
#include "Cars.h"
#include "BinaryTree.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <float.h>
#include <math.h>

Station *searchById(BinaryTree *tree, SearchKey *key);
Station *searchByName(BinaryTree *tree, SearchKey *key);
Station *searchByDistance(BinaryTree *tree, SearchKey *key);

Station *StationCreate(unsigned int id, const char *name, int nPorts, Coord coord)
{
  // printf("[StationCreate] Creating station %u\n", id);
  Station *s = malloc(sizeof(Station));
  if (!s)
  {
    perror("Failed Allocation Memory Create Station\n");
    return NULL;
  }
  // printf("[Create3] Allocated station struct\n");
  // copy name
  s->name = strdup(name);
  if (!s->name)
  {
    fprintf(stderr, "Failed copy name to station for : %s\n", name);
    free(s);
    return NULL;
  }
  // printf("[Create5] Name duplicated: %s\n", name);

  // creat Queue
  s->qCar = createQueue();
  if (!s->qCar)
  {
    fprintf(stderr, "Queue creataion failed for Station: %u\n", id);
    free(s->name);
    free(s);
    return NULL;
  }

  s->id = id;
  s->nPorts = nPorts;
  s->coord = coord;
  s->portsList = NULL;
  s->nCars = 0;
  s->left = NULL;
  s->right = NULL;

  return s;
}

void printFullStation(const void *data)
{

  const Station *station = (const Station *)data;
  printf("\n========== Station: %s (ID: %u) ==========\n", station->name, station->id);
  printf("Location: (%.2f, %.2f)\n", station->coord.x, station->coord.y);
  printf("Total Ports: %d\n", station->nPorts);
  printf("Cars in queue: %d\n", countQueueItems(station->qCar));

  printf("\n-- Ports --\n");
  if (station->portsList)
  {
    printPortList(station->portsList); // assumes you already implemented this
  }
  else
  {
    printf("No ports available\n");
  }

  if (!isEmpty(station->qCar))
  {
    printf("\n-- Queue --\n");
    printQueue(station->qCar); // assumes you already implemented this
  }
  else
  {
    printf("\nNo cars in queue.\n");
  }
}

void StationDestroy(void *data)
{
  Station *station = (Station *)data;
  if (!station)
    return;
  // free dynamic name
  free(station->name);
  // free dynamic qCar
  if (station->qCar)
    destroyQueue(station->qCar);

  // free dynamic ports linked list
  if (station->portsList)
    destroyPortList(station->portsList);
  free(station);
}

int compareStation(const void *a, const void *b)
{
  Station *s1 = (Station *)a;
  Station *s2 = (Station *)b;

  // Add null checks
  if (!s1 || !s2)
  {
    return 0;
  }

  int result = (s1->id > s2->id) - (s1->id < s2->id);
  return result;
}

void printStation(const void *data)
{
  const Station *station = data;
  printf("Station ID: %u  |  Name: %s |  Ports: %d  |  Cars: %d\n", station->id, station->name, station->nPorts, station->nCars);
}

void *Station_parseLine(const char *line)
{

  unsigned int id;
  char name[100];
  int nPorts;
  double x, y;

  int scan = sscanf(line, "%u,%99[^,],%d,%lf,%lf", &id, name, &nPorts, &x, &y);
  if (scan != 5)
  {
    fprintf(stderr, "Failed to parse line: %s\n", line);
    return NULL;
  }

  Coord coord = {x, y};
  Station *station = StationCreate(id, name, nPorts, coord);

  return station;
}

// search station
static SearchFunc searchFunctions[SEARCH_TYPE_COUNT] = {
    searchById,
    searchByName,
    searchByDistance};

Station *searchStation(BinaryTree *tree, SearchKey *key, SearchType type)
{
  if (!tree || !key)
    return NULL;
  if (type < 0 || type >= SEARCH_TYPE_COUNT)
    return NULL;
  return searchFunctions[type](tree, key);
}

typedef struct
{
  Station *closest;
  double minDistance;
  double userX;
  double userY;
} DistanceSearchHelper;

Station *searchByNameHelper(TreeNode *node, const char *name)
{
  if (node == NULL)
    return NULL;
  Station *station = (Station *)node->data;

  // search correct node
  if (strcasestr(station->name, name) != NULL)
    return station;

  // search lef
  Station *found = searchByNameHelper(node->left, name);
  if (found)
    return found;

  // search right
  return searchByNameHelper(node->right, name);
}

Station *searchByName(BinaryTree *tree, SearchKey *key)
{
  if (!tree || !key)
    return NULL;
  return searchByNameHelper(tree->root, key->name);
}

Station *findStationById(TreeNode *node, int id)
{
  if (node == NULL)
    return NULL;

  Station *station = (Station *)node->data;

  if (station->id == id)
  {
    return station;
  }
  else if (id < station->id)
  {
    return findStationById(node->left, id);
  }
  else
  {
    return findStationById(node->right, id);
  }
}

Station *searchById(BinaryTree *tree, SearchKey *key)
{
  if (!tree || !key)
    return NULL;
  return findStationById(tree->root, key->id);
}

void searchByDistanceHelper(TreeNode *node, DistanceSearchHelper *helper)
{
  if (node == NULL)
    return;
  Station *station = (Station *)node->data;

  double dx = station->coord.x - helper->userX;
  double dy = station->coord.y - helper->userY;
  double distance = sqrt(dx * dx + dy * dy);

  if (distance < helper->minDistance)
  {
    helper->minDistance = distance;
    helper->closest = station;
  }

  searchByDistanceHelper(node->left, helper);
  searchByDistanceHelper(node->right, helper);
}

static void findMax(TreeNode *node, unsigned int *maxId)
{

  if (!node)
    return;
  Station *s = (Station *)node->data;
  if (s->id > *maxId)
    *maxId = s->id;
  findMax(node->left, maxId);
  findMax(node->right, maxId);
}

unsigned int generateUniqueStationId(BinaryTree *tree)
{
  unsigned int maxId = 100; // start from 100 at least
  if (tree && tree->root)
    findMax(tree->root, &maxId);

  return maxId + 1;
}

Station *searchByDistance(BinaryTree *tree, SearchKey *key)
{
  if (!tree)
    return NULL;

  DistanceSearchHelper helper = {
      .closest = NULL,
      .minDistance = DBL_MAX,
      .userX = key->location.userX,
      .userY = key->location.userY};

  searchByDistanceHelper(tree->root, &helper);
  return helper.closest;
}

static Car *searchInQueueHelper(const TreeNode *node, const char *license)
{
  if (!node)
    return NULL;

  Station *station = (Station *)node->data;
  CarNode *current = station->qCar->front;

  while (current)
  {
    if (strcmp(current->data->nLicense, license) == 0)
    {
      return current->data;
    }
    current = current->next;
  }

  // search left
  Car *found = searchInQueueHelper(node->left, license);
  if (found)
    return found;
  return searchInQueueHelper(node->right, license);
}

Car *searchCarInAllQueues(const BinaryTree *stationTree, const char *license)
{
  if (!stationTree || !license)
    return NULL;
  return searchInQueueHelper(stationTree->root, license);
}

BOOL enqueueCarToStationQueue(Station *station, Car *car)
{
  if (!station || !car)
    return FALSE;
  return enqueue(station->qCar, car);
}