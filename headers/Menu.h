#ifndef MENU_H
#define MENU_H

#include "SystemData.h"

typedef struct menu
{
  SystemData *sys;
} MenuSystem;

// init main menu
MenuSystem initMenu(SystemData *sys);

// main menu
void mainMenu(SystemData *menu);
// clean menu
void destroyMenu(MenuSystem *menu);

// 1: Locate nearest station
void locateNearSt(const BinaryTree *stationTree);
// 2: Charge Car
void chargeCar(BinaryTree *stationTree, BinaryTree *carTree);
// 3: Check Car Status
void checkCarStatus(const BinaryTree* carTree,const BinaryTree* stationTree);
// 4: Stop Charge
void stopCharge(BinaryTree* stationTree,BinaryTree* carTree);

// TEST OPTIONS
void displaySystemStatus(const BinaryTree *stationTree, const BinaryTree *carTree);
void createStationInteractive(BinaryTree* stationTree);
#endif