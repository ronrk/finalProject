#include "Station.h"
#include "Queue.h"
#include "Port.h"
#include "Cars.h"
#include "BinaryTree.h"
#include "Utilis.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <float.h>
#include <math.h>

Station *searchById(const BinaryTree *tree, SearchKey *key);
Station *searchByName(const BinaryTree *tree, SearchKey *key);
Station *searchByDistance(const BinaryTree *tree, SearchKey *key);

Station *StationCreate(unsigned int id, const char *name, int nPorts, Coord coord)
{
  // printf("[StationCreate] Creating station %u\n", id);
  Station *s = (Station *)malloc(sizeof(Station));
  if (!s)
  {
    perror("Failed Allocation Memory Create Station\n");
    return NULL;
  }

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
    // printQueue(station->qCar); // assumes you already implemented this
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

int compareStationById(const void *a, const void *b)
{
  const Station *s1 = (const Station *)a;
  const Station *s2 = (const Station *)b;

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
  const Station *station = (Station *)data;
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

void addPortToStation(Station *station, Port *port, BOOL increment)
{
  if (!station || !port)
  {
    return;
  }

  station->portsList = insertPort(station->portsList, port);
  if (increment) station->nPorts++;

  // count how many ports really in the linked list now
  int actualCount = 0;
  Port *cur = station->portsList;
  while (cur)
  {
    actualCount++;
    cur = cur->next;
  }

}

// search station
static SearchFunc searchFunctions[SEARCH_TYPE_COUNT] = {
    searchById,
    searchByName,
    searchByDistance};

Station *searchStation(const BinaryTree *tree, SearchKey *key)
{
  if (!tree || !key)
    return NULL;
  if (key->type < 0 || key->type >= SEARCH_TYPE_COUNT)
    return NULL;
  return searchFunctions[key->type](tree, key);
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

Station *searchByName(const BinaryTree *tree, SearchKey *key)
{
  if (!tree || !key)
    return NULL;
  return searchByNameHelper(tree->root, key->name);
}

Station *searchById(const BinaryTree *tree, SearchKey *key)
{
  if (!tree || !key)
    return NULL;
  Station temp = {.id = key->id};
  return (Station *)searchBST((BinaryTree *)tree, &temp);
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

unsigned int generateUniqueStationId(BinaryTree *tree)
{
  unsigned int baseId = 1000; // start from 100 at least
  if (!tree || !tree->root)
    return baseId + 1;

  Station *maxStation = (Station *)findMaxData(tree->root, compareStationById);
  if (maxStation && maxStation->id > baseId)
    return maxStation->id + 1;
  else
    return baseId + 1;
}

Station *searchByDistance(const BinaryTree *tree, SearchKey *key)
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

Station *findStationByPort(const BinaryTree *tree, const Port *port)
{
  if (!tree || !tree->root || !port)
    return NULL;

  // Iterative in-order traversal stack
  TreeNode *stack[1000];
  int top = -1;
  TreeNode *current = tree->root;

  while (current || top >= 0)
  {

    while (current)
    {
      stack[++top] = current;
      current = current->left;
    }

    current = stack[top--];
    Station *station = (Station *)current->data;

    // Check if the port exists in this station's list
    Port *p = station->portsList;
    while (p)
    {

      // printf("  Port at %p, p->p2Car = %p\n", (void *)p, (void *)p->p2Car);
      if (p == port)
      { // Compare by pointer (memory address)
        return station;
      }
      p = p->next;
    }

    current = current->right;
  }
  return NULL;
}

BOOL enqueueCarToStationQueue(Station *station, Car *car)
{
  if (!station || !car)
    return FALSE;

  if (enqueue(station->qCar, car))
  {
    car->inqueue = TRUE;
    station->nCars++;
    return TRUE;
  }

  return FALSE;
}

Station *findStationByCar(BinaryTree *stationTree, Car *car)
{
  if (!stationTree || !car || !car->pPort)
    return NULL;

  return findStationByPort(stationTree, car->pPort);
}

// //////////////////////////////////
void printStationSummary(const void *data)
{
  const Station *station = (const Station *)data;
  if (!station)
    return;

  printf("\t|%s (%u):|\n", station->name, station->id);

  // Count ports
  int totalPorts = 0, occupied = 0, ood = 0, fast = 0, mid = 0, slow = 0;
  Port *p = station->portsList;
  while (p)
  {
    totalPorts++;
    if (p->status != OOD)
      ood++;
    if (p->status == OCC)
      occupied++;

    if (p->portType == SLOW)
      slow++;
    else if (p->portType == MID)
      mid++;
    else if (p->portType == FAST)
      fast++;

    p = p->next;
  }

  // Count queue length
  int queueCount = countQueueItems(station->qCar);
  printf("Total Ports: %d | Out-Of-Order: %d | Occupied: %d | Queue: %d\n", totalPorts, ood, occupied, queueCount);
  printf("Port Types: FAST: %d | MID: %d | SLOW: %d\n", fast, mid, slow);
}
