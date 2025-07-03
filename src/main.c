#include "../headers/SystemData.h"
#include "../headers/Station.h"
#include "Menu.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void testSearch(BinaryTree *tree);

int main() {
    SystemData *data = loadFiles();
    // if(!sys) return 1;
    printf("Stations loaded. Running search tests...\n");
    printf("\n=== All Stations In Tree ===\n");
    inorderBST(&data->stationTree, printStation);
    printf("============================\n");

    testSearch(&data->stationTree);
    // printf("DEBUG: Loaded %d stations\n", countNodes(sys->stationTree.root));

    // mainMenu(sys);

    destroyFiles(data);
    return 0;
}

void testSearch(BinaryTree *tree)
{

  // Search by ID - valid example
    SearchKey keyId = {.type = SEARCH_BY_ID, .id = 101};
  Station *foundById = searchStation(tree, &keyId, SEARCH_BY_ID);
  if (foundById)
    printf("[TEST] Found by ID: %u => %s\n", foundById->id, foundById->name);
  else
    printf("[TEST] Station ID not found.\n");

  // Search by Name - valid example
SearchKey keyName = {.type = SEARCH_BY_NAME, .name = "Haifa Port"};
  Station *foundByName = searchStation(tree, &keyName, SEARCH_BY_NAME);
  if (foundByName)
    printf("[TEST] Found by Name: %s => ID %u\n", foundByName->name, foundByName->id);
  else
    printf("[TEST] Station Name not found.\n");

  // Search by Distance
  SearchKey keyDist = {.type = SEARCH_BY_DISTANCE};
  keyDist.location.userX = 32.08;
  keyDist.location.userY = 34.78;
  Station *foundByDist = searchStation(tree, &keyDist, SEARCH_BY_DISTANCE);
  if (foundByDist)
    printf("[TEST] Closest Station: %s (ID %u)\n", foundByDist->name, foundByDist->id);
  else
    printf("[TEST] No station found by distance.\n");
}
