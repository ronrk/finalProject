#include "../headers/SystemData.h"
#include "../headers/BinaryTree.h"
#include "../headers/Cars.h"
#include "../headers/Station.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

SystemData* loadFiles(){
  // init systemData struct to handle data
  SystemData *sys = malloc(sizeof(SystemData));
  if(!sys) {
    perror("Failed to allocate memory in SystemData\n");
    return NULL;
  }


  // init car tree
  sys->carTree = initTree(compareCars,printCar,destroyCar);
  sys->stationTree = initTree(compareStation,printStation,StationDestroy);

  // load stations
  int stationCount = loadStations(&sys->stationTree);
   // load cars
   int carCount = loadCars(&sys->carTree);

  // load ports and attach to stations
  int portsCount = loadPorts(sys);

  // load queue
  loadLineOfCars(sys);

  return sys;
}

void* stationParser(const char* line) {
  return Station_parseLine(line);
}

void* carsParser(const char* line){
  return parseCarLine(line);
}

void* portParser(const char*line) {
  // parse port line
  unsigned int stationId,portNum;
  char typeStr[10],license[LICENSE_SIZE];
  int status,y,m,d,h,min;

  if(sscanf(line,"%u,%u,%9[^,],%d,%d,%d,%d,%d,%d,%8s",
    &stationId, &portNum, typeStr, &status, &y, &m, &d, &h, &min, license) 
      != 10){
        // DEBUGGG
        fprintf(stderr, "[portParser] Failed to parse line: %s\n", line);

        return NULL;
      }

      PortType parsedType = Util_parsePortType(typeStr);
  if (parsedType == -1) {
    fprintf(stderr, "[portParser] Invalid port type in line: %s\n", line);
    return NULL;
  }

  return createPort(stationId,portNum,Util_parsePortType(typeStr),(PortStatus)status,NULL,
                    (Date){y,m,d,h,min},license);
}

void* lineOfCarParser(const char* line){
  if(!line) return NULL;
  LineOfCarsEntry* entry = malloc(sizeof(LineOfCarsEntry));
  if(!entry) return NULL;

  if(sscanf(line,"%u,%8s",&entry->stationId,entry->license) != 2) {
    free(entry);
    return NULL;
  }
  return entry;
}

void lineOfCarsProcessor(void* obj,void* context){
  if(!obj||!context) return;

  LineOfCarsEntry* entry = (LineOfCarsEntry* )obj;
  SystemData* sys = (SystemData*) context;

  Station searchKey = {.id = entry->stationId};
  Station * station = searchBST(&sys->stationTree,&searchKey);
  if (!station) {
        fprintf(stderr, "Station %u not found for line of cars entry\n", entry->stationId);
        return;
    }
  Car* car = searchCar(&sys->carTree, entry->license);
    if (!car) {
        fprintf(stderr, "Car %s not found for line of cars entry\n", entry->license);
        return;
    }
    if (!enqueueCarToStationQueue(station, car)) {
        fprintf(stderr, "Failed to enqueue car %s to station %u queue\n", entry->license, entry->stationId);
    }
}

void destroyLineOfCars(void* obj){
  free(obj);
}

// processor to link port to station
void linkPortToStation(void*obj, void*context) {
  Port *port = (Port*)obj;
  SystemData* sys = (SystemData*)context;

  // find the station for this port
  Station searchKey = {.id=port->stationId};
  Station *station = searchBST(&sys->stationTree,&searchKey);

  if(!station) {
    fprintf(stderr,"Station %u no found for port: %u\n",port->stationId,port->num);
    return;
  } 

  // add port to station
  station->portsList = insertPort(station->portsList,port);

  // link car to port
  if(strcmp(port->license,"-1")!=0) {
    Car* car = searchCar(&sys->carTree,port->license);
    if(car) {
      port->p2Car = car;  // port to car
      car->pPort = port;  // car to port
      // ////[DEBUG]
      // printf("Linked car %s to port %u\n", port->license, port->num);
    }
  }

}

void destroyFiles(SystemData *sys) {
  if(!sys) return;

  // free stations
  destroyTree(sys->stationTree.root,sys->stationTree.destroy);

  // free cars
  destroyTree(sys->carTree.root,sys->carTree.destroy);
  
  free(sys);
}

int loadStations(BinaryTree *stationTree){
  FileLoaderConfig config = {
    .filename="data/Stations.txt",
    .targetTree =stationTree,
    .parser = stationParser,
    .processor = NULL,
    .context = NULL,
    .destroyObject=StationDestroy,
    .skipHeader = 1
  };
  return loadDataFile(&config);
}

int loadCars(BinaryTree *carTree){
  FileLoaderConfig config = {
    .filename = "data/Cars.txt",
    .targetTree =carTree,
    .parser = carsParser,
    .processor = NULL,
    .context = NULL,
    .destroyObject = destroyCar,
    .skipHeader = 1
  };
  return loadDataFile(&config);
}

int loadPorts(SystemData *sys){
  FileLoaderConfig config = {
    .filename = "data/Ports.txt",
    .targetTree = NULL,
    .parser = portParser,
    .processor = linkPortToStation,
    .context = sys,
    .destroyObject = destroyPort,
    .skipHeader = 1
  };
  return loadDataFile(&config);
}

int loadLineOfCars(SystemData *sys) {
  if(!sys) return 0;
  FileLoaderConfig config = {
    .filename = "data/LineOfCars.txt",
    .targetTree = NULL,
    .parser = lineOfCarParser,
    .processor = lineOfCarsProcessor,
    .context = sys,
    .destroyObject = destroyLineOfCars,
    .skipHeader = 1
  }; 

  return loadDataFile(&config);
}

