#include "Menu.h"
#include "Queue.h"
#include "ErrorHandler.h"
#include "Station.h"
#include "Utilis.h"
#include "Cars.h"
#include "SystemData.h"
#include "Port.h"
#include "Test.h"

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <string.h>
#include <time.h>

Station* getStationFromUser(const BinaryTree *stationTree);
BOOL getLicenseFromUser(char *input,size_t size);
PortType getPortTypeFromUser();

void mainMenu(SystemData* sys) {
  int choice;
  char input[10];
  do
  {
    printf(
      "\n\t***** EV CHARGING MANAGER *****\n"
      "*1. Locate Nearest Station\n"
      "*2. Charge Car\n"
      "*3. Check Car Status\n"
      "*4. Stop Charge\n"
      "*5. Display all stations\n"
      "*6. Display Cars at station\n"
      "*7. Report of stations\n"
      "*8. Display top customers\n"
      "*9. Add New Port\n"
      "###### TESTS ######\n"
      "*91. Test Locate Nearest Station\n"
      "*92. Test Charge Car with Inputs\n"
      "*93. Test Stop Charge with Inputs\n"
      "*0. Exit\n"
      "********************************\n");

    printf("Enter your choice: ");
    if(!getLineFromUser(input,sizeof(input))) {
      printf("Error reading input\n");
      continue;
    } 

    if(!isNumericString(input)) {
      printf("Invalid input. Enter a number.\n");
      continue;
    }

    choice = atoi(input);

      switch (choice)
      {
      case 1:
        locateNearSt(&sys->stationTree);
        break;
      case 2:
        chargeCar(&sys->stationTree,&sys->carTree);
        break;
      case 3:
        checkCarStatus(&sys->carTree, &sys->stationTree);
        break;
      case 4:
        stopCharge(&sys->stationTree,&sys->carTree);
        break;
      case 5:
        dispAllSt(&sys->stationTree);
        break;
      case 6:
        dispCarsAtSt(&sys->stationTree);
        break;
      case 91:
        test_locateNearSt_feature(sys);
        break;
      case 92:
        test_chargeCar_feature(sys);
        break;
      case 93:
      test_stopCharg(sys);
        break;
      case 0:
        printf("Exiting system...\n");
        break;
      default:
      
      printf("Invalid choice try again\n");
      }
      
    }
  while (choice!=0); 
  
}

// 1.locate nearst station

void locateNearSt(const BinaryTree * stationTree) {

    if(!stationTree||!stationTree->root) {
      displayError(UI_WARNING,"No Stations availabe");
      return;
    }

    // get user location
    Coord userLoc;
    

    if(!getCoordFromUser(&userLoc,"Enter your X : ","Enter your Y : ")) {
      displayError(UI_WARNING,"Invalid coordinates");
      return;
    }

    // search
    SearchKey key = {.type = SEARCH_BY_DISTANCE,.location={.userX = userLoc.x,.userY=userLoc.y}};
    Station* nearest = searchStation((BinaryTree*)stationTree,&key);

    // print result
    if(nearest) {
      double dist = calculateDistance(userLoc,nearest->coord);
      printf("\n\t*** Nearest Station ***\t\n");
      printf("Name: %s\n",nearest->name);
      printf("Id: %u\n",nearest->id);
      printf("Distance: %.2f\n",dist);
      printf("Ports Available: %d/%d\n",
      countFreePorts(nearest->portsList),nearest->nPorts);

        // printQueue
        if(nearest->qCar && !isEmpty(nearest->qCar)) {
          printf("Cars in queue: %d\n", countQueueItems(nearest->qCar));
          if(!getFront(nearest->qCar)){
            return;
          }
          printf("First in queue: %s\n", getFront(nearest->qCar)->nLicense);
        } else {
           printf("No cars in queue\n");
        }
        
    } else {
      displayError(UI_WARNING,"No station found");
      return;
    }
  }

// 2. charge car

void chargeCar(BinaryTree *stationTree,BinaryTree* carTree){
  printf("\n\t*** Charge Car ***\t\n");
  char license[100];
  Car* car = NULL;
  Port* port = NULL;
  Station* station = NULL;
  PortType pType;
  int availablePorts;

  // get license
  if(!getLicenseFromUser(license,sizeof(license))) {
    return; // Canceled
  }

  car = searchCar(carTree,license);

  if(car) {
    port = getCarPort(car);
    if(port !=NULL) {
      printf("Car %s is already connected on port %d\n", car->nLicense, port->num);
      return;
    }
    if(car->inqueue){
      printf("Car %s is already in queue\n", car->nLicense);
      return;
    }
  }

  // get station
  station = getStationFromUser(stationTree);
  if (!station) {
    displayError(UI_WARNING,"Station selection canceled or failed");
    return;
  }

  if(car) { //found car
    printCar(car);
    availablePorts = printCompatibleFreePorts(station->portsList,car->portType,station->name);
    if(availablePorts == 0) {
      printf("Sorry no available port for %s type\n",portTypeToStr(car->portType));
      return;
    }
    port = getPortNumFromUser(station->portsList,car->portType);
    if(!port){
      printf("Invalid or canceled port selection\n");
      return;
    }
  } else { //create new car
    // get port type
    pType = getPortTypeFromUser();
    if (pType == INVALID_PORT || pType == -1) {
      printf("Invalid or canceled port type selection\n");
      return;
    }

    availablePorts = printCompatibleFreePorts(station->portsList,pType,station->name);

    if(availablePorts == 0) {
      printf("Sorry no available port for %s type\n",portTypeToStr(pType));
      return;
    }
    port = getPortNumFromUser(station->portsList,pType);
    if(!port) {
      printf("Invalid or canceled port selection\n");
      return;
    }


    car = createCar(license,pType);
    if(!car) {
      printf("Failed to creat car\n");
      return;
    }

    // inser to tree
    if(!insertBST(carTree,car)){
      displayError(ERR_MEMORY, "Failed to insert car into system");
      destroyCar(car);
      return;
    }
  }
 
  // link car port
  if(isPortAvailable(port)){
    // port free
    Date now = getCurrentDate();
    if(assignCar2Port(port,car,now)) {
      printf("\nCar %s assigned to port %u at station %s\n", car->nLicense, port->num, station->name);
      printf("\t");
      printPort(port);

      } else {
        displayError(UI_WARNING, "Failed to assign car to port");
      }

    } else {
      // port occupied
      if(station->qCar) {
        enqueue(station->qCar,car);
        car->inqueue = TRUE;
        printf("Port is occupied. Car %s added to queue at station %s\n", car->nLicense, station->name);
      } else {
        displayError(UI_WARNING, "Station queue does not exist");
      }

    }
}



//3. Check car status

void checkCarStatus(const BinaryTree* carTree,const BinaryTree* stationTree) {
  Station* station = NULL;
  Port* port = NULL;

  printf("\n\t=== Check Car Status ===\t\n");

  // get license
  char license[128];
  if(!getLicenseFromUser(license,sizeof(license))) {
    return;
  }

  // search car
  Car* car = searchCar(carTree,license);
  
  if(car) {
    printCar(car);

    if(car->pPort) {
      port = car->pPort;
      station = findStationByPort(stationTree,port);

      Date now = getCurrentDate();
      int min = diffInMin(port->tin, now);

      printf("\nCar is charging at station %s (%d), port #%d\n",station->name,station->id,port->num);
      printf("\tCharging time: %d minutes\n",min);

    } else if (car->inqueue){
      station = findStationOfQueueCar(stationTree->root,car);
      if(station) {
        printf("\ncar is currently waitng at station %s (%d) for type %s\n",station->name,station->id,portTypeToStr(car->portType));
      } else {
        printf("[ERROR]car is queued but not found at station\n");
      }

    } else {
      printf("\ncar is not charging and not in queue.\n");
    }
    return;
  }

}

// 4. Stop Charge

void stopCharge(BinaryTree* stationTree,BinaryTree* carTree) {
  if(!stationTree || !carTree) {
    printf("No Data\n");
    return;
  }
  printf("\n\t=== Stop Charging ===\t\n");

  char license[LICENSE_SIZE];
  if(!getLicenseFromUser(license,sizeof(license))){
    return;
  }

  Car* car = searchCar(carTree,license);
  if(!car) {
    printf("No car in system\n");
    return;
  }
  if(!car->pPort) {
    printf("This car is not currently chargin\n");
    return;
  }

  processStopCharge(car, stationTree);
}

void processStopCharge(Car *car,BinaryTree* stationTree) {
  // init variable, duration, date, payed
  Port* port = car->pPort;
  Date now = getCurrentDate();
  int minutesCharged = diffInMin(car->pPort->tin,now);
  if(minutesCharged<1) minutesCharged = 1;
  double bill = minutesCharged*RATE_CHARGE;
  car->totalPayed += bill;

  printf("Charging stopped for car %s after %d minutes, bill: %.2f\n", car->nLicense, minutesCharged,bill);
  printf("Total payment updated: %.2f\n", car->totalPayed);

  // find station
  Station* station = findStationByCar(stationTree,car);

  // unlink carPort
  unlinkCarPort(car);

  // check queue for compatible cars if yes dequeue and assignCar2Port
  tryAssignNextCarFromQueue(station,port,now);
  printf("\n");

}

// 5. Display all stations
void dispAllSt(BinaryTree* stationTree){
  if(!stationTree||!stationTree->root) {
    printCar("No Stations Loaded\n");
    return;
  }

  printf("\n\t=== All Charging Stations ===\t\n");
  inorderBST(stationTree,printStationSummary);
  printf("=============================\n");

}

// 6. Display all cars in station
void dispCarsAtSt(BinaryTree* stationTree){
  if(!stationTree) return;
  
  Station* station = getStationFromUser(stationTree);
  if(!station) return;

  printf("\n=== Cars at Station: %s (%d) ===\n",station->name,station->id);

  displayChargingCars(station);

  displayWaitingsCars(station);
  
  printf("==============================\n");
}

void displayChargingCars(const Station* station){
  if(!station) return;
  Port *port = station->portsList;
  Date now = getCurrentDate();
  int foundCars = 0;

  while (port)
  {
    if(port->p2Car){
      int chargeMin = diffInMin(port->tin,now);
      if (chargeMin < 0) chargeMin = 0;
      printf("Charging: Car %s | Port Type: %s | Charging for %d minutes\n",port->p2Car->nLicense,portTypeToStr(port->portType),chargeMin);
      foundCars = 1;
    }

    port = port->next;
  }
  if(foundCars == 0){
    printf("No cars currently in charging\n");
  }
}

void displayWaitingsCars(const Station* station){
  // cars waiting
  if(!station || !station->qCar || !isEmpty(station->qCar)) return;
  if(station->qCar && !isEmpty(station->qCar)){
    CarNode *current = station->qCar->front;
    while (current)
    {
      printf("Waiting car %s | Port type: %s\n",current->data->nLicense,portTypeToStr(current->data->portType));

      current = current->next;
    }
  } else {
    printf("No cars waiting in the queue\n");
  }
}

// 
// 
Station* getStationFromUser(const BinaryTree *stationTree) {
  char input[100];
  SearchKey key;
  SearchType type;
  while (1)
  {
    if (!getInputAndCancel(input,sizeof(input),"Enter Station ID or Name (or '0' to cancel): ")) {
      return NULL;
      }

      if (strlen(input) == 0) {
      displayError(UI_WARNING,"Empty input is invalid. Try again");
      continue; // loop to ask again
      }

      // check valid station
    if(!parseStationInput(input,&key,&type)) {
      printf("Invalid input\n");
      continue;
    };

    key.type = type;
    Station* station = searchStation(stationTree, &key);

    if(station) {
      printf("Found station: %s (ID: %u)\n", station->name, station->id);
      return station;
    } else {
      printf("No station found with input '%s'. Try again.\n", input);
    }
  }
  
}

PortType getPortTypeFromUser() {
  char input[20];
  while (1)
  {
    if(!getInputAndCancel(input,sizeof(input),"Enter Port Type for new car (| FAST | MID | SLOW |): ")) {
      return -1;
    }


    PortType type = Util_parsePortType(input);
    if(type != INVALID_PORT) return type;

    printf("Invalid input. (| FAST | MID | SLOW |)\n");
  } // end while
  
};

Port* getPortNumFromUser(Port* portList,const PortType portType){
  char input[16];
  unsigned int portNum;
  Port* port;
  while (1)
  {    
    if(!getInputAndCancel(input,sizeof(input),"Enter port num : ")){
    return NULL;
  }

    if (!isNumericString(input)) {
    printf("Invalid port number. Try again.\n");
    continue;
  }

  portNum = (unsigned int)atoi(input);
  port = findPort(portList,portNum);
  if(!port) {
    printf("Port number %u not found. Try again\n",portNum);
    continue;
  }
  if(!isCompatiblePortType(portType,port->portType)){
    printf("Invalid port number. Try again.\n");
    continue;
  }
  return port;
  }
 
}

BOOL getLicenseFromUser(char *input,size_t size) {
  while (1)
  {
    /* code */
    if(!getInputAndCancel(input,size,"Enter car license number (8 digits): ")) {
      displayError(UI_WARNING,"Canceled by user");
      return FALSE;
    }

    if(!isLicenseValid(input)) {
      displayError(UI_WARNING,"Invalid license format. Try again");
      continue;
    }
    return TRUE;
  }

}

int printCompatibleFreePorts(Port* head,PortType carType,char * stationName){
  printf("|%s| types ports in %s: \n",portTypeToStr(carType),stationName);
  int found = 0;
  while(head) {
    if(isCompatiblePortType(carType,head->portType)) {
      if(isPortAvailable(head)){
        printf("\t#%d. Port \n",head->num);
      } else {
        printf("\t#%d.Port (occupied, enter to queue?)\n",head->num);
      }
      
      found++;
    }
    head = head->next;
  }
  if(found == 0) {
    printf("No compatible ports for type %s\n",portTypeToStr(carType));
  }
  return found;
}
