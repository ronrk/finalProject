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
Station* getStationFromUser(const BinaryTree *stationTree);
PortType getPortTypeFromUser();
Port* getPortNumFromUser(Port* portList,const PortType portType);
BOOL getLicenseFromUser(char *input,size_t size);
int printCompatibleFreePorts(Port* head,PortType carType,char * stationName);

// 3: Check Car Status
void checkCarStatus(const BinaryTree* carTree,const BinaryTree* stationTree);


// 4: Stop Charge
void stopCharge(BinaryTree* stationTree,BinaryTree* carTree);
void processStopCharge(Car *car,BinaryTree* stationTree);


// 5. Display all stations
void dispAllSt(BinaryTree* stationTree);

// 6. display cars at station
void dispCarsAtSt(BinaryTree* stationTree);
void displayWaitingsCars(const Station* station);
void displayChargingCars(const Station* station);

// TEST OPTIONS
void displaySystemStatus(const BinaryTree *stationTree, const BinaryTree *carTree);
void createStationInteractive(BinaryTree* stationTree);
#endif