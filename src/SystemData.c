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
  // sys->stationTree = initTree(compareStation,printStation,StationDestroy);

  // load stations
  stationsLoad(sys->stationTree);
  

  // load ports and attach to stations
  loadPorts(sys);

  // load cars
  carsLoad(sys->carTree);

  // load queue
  loadLineOfCars(sys);
}

void destroyFiles(SystemData *sys) {
  if(!sys) return;

  // free stations
  destroyTree(sys->stationTree.root,StationDestroy);

  // free cars
  destroyTree(sys->carTree.root,destroyCar);

  free(sys);
}

void stationsLoad(BinaryTree *stationTree) {
  if(!stationTree) {
    printf("Stations tree is NULL\n");
    return;
  }

  FILE* file = fopen("/data/Stations.txt","r");
  if(!file) {
    perror("Failed open Stations.txt\n");
    return;
  }

  printf("Stations file opened success\n");

  if(!skipHeader(file)) {
    printf("Station.txt is empty or corrupted\n");
    fclose(file);
    return;
  }

  // read each line and parse
  char line[256];
  while (fgets(line,sizeof(line),file))
  {
    line[strcspn(line,"\r\n")=='\0'];  // remove newline

    // parse line and insert to tree
    Station *station = parseStationLine(line);
    if(station) {
      insertBST(stationTree,station);
    }
  }
  
  // print for test
  printf("\n----All Station in BST---\n");
  inorderTraversal(stationTree->root,printStation);

  fclose(file);
}

void portsLoad(BinaryTree *stationTree, BinaryTree *carTree) {
  FILE* file = fopen("/data/Ports.txt","r");
  if(!file) {
    perror("Error open file Ports.txt\n");
    return;
  }

  if(!skipHeader(file)) {
    fclose(file);
    return;
  }
  char line[256];
  // parse each line
  while (fgets(line,sizeof(line),file))
  {
    line[strcspn(line,"\r\n")] = '\0';  // remove newline

    unsigned int stationId,portNum;
    char portTypeString[10];
    int statusInt,year,month,day,hour,min;
    char license[9];
    int parsed = sscanf(line,"%u,%u,%9[^,],%d,%d,%d,%d,%d,%d,%8s",
      &stationId,&portNum,portTypeString,&statusInt,&year,&month,&day,&hour,&min,license);

    if(parsed !=10) {
      fprintf(stderr,"Failed to pars Ports line: %s\n",line);
      
      continue;
    }
    // convert string to enum , find station, create port
    PortStatus status = (PortStatus) statusInt;
    PortType type = parsePortType(portTypeString);

    Car *car =NULL;
    if(strcmp(license,"-1")!=0) {
      car = searchCar(&carTree,license);

      if(!car) {
        fprintf(stderr,"Warning: Car with license '%s' not found.\n");
      }
    }
  }
  
}

void loadCars(SystemData *sys){}
void loadPorts(SystemData *sys){}
void loadLineOfCars(SystemData *sys){}