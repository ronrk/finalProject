#ifndef MENU_H
#define MENU_H
#include "Utilis.h"


typedef struct MenuSystem MenuSystem;
typedef struct SystemData SystemData;
typedef struct BinaryTree BinaryTree;
typedef struct Station Station;
typedef struct Port Port;
typedef struct Car Car;

struct MenuSystem
{
  SystemData *sys;
};


// main menu
void mainMenu(SystemData *menu);

// 1: Locate nearest station
void locateNearSt(const BinaryTree *stationTree);

// 2: Charge Car
void chargeCar(BinaryTree *stationTree, BinaryTree *carTree);

// 3: Check Car Status
void checkCarStatus(const BinaryTree* carTree,const BinaryTree* stationTree);

// 4: Stop Charge
void stopCharge(BinaryTree* stationTree,BinaryTree* carTree);

// 5. Display all stations
void dispAllSt(BinaryTree* stationTree);

// 6. display cars at station
void dispCarsAtSt(BinaryTree* stationTree);

// 7. Report of statistis, station
void reportStStat(const BinaryTree* stationTree);

// 8.Display top customers
void dispTopCustomers(const BinaryTree *carTree);

// 9.Add new port
void addNewPort(BinaryTree* stationTree);

// 10.Realease Charging Ports
void releasePorts(BinaryTree* stationTree);

// 11.Remove Out Of Order Ports
void removeOutOrderPort(BinaryTree* stationTree);

// 12.Remove Customer
void remCustomer(BinaryTree* carTree);

// 13.Close Station
void closeSt(BinaryTree* stationTree);
#endif