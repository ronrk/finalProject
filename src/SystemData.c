#include "SystemData.h"

#include "Station.h"
#include "Cars.h"
#include "Port.h"
#include "Utilis.h"
#include "ErrorHandler.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct 
{
  Port* port;
  unsigned int stationId;
  char license[LICENSE_SIZE];
}PortTempData;

// static Station* getStationById(BinaryTree* tree,unsigned int id){
//   if(!tree||!tree->root) return NULL;
// return findStationById(tree->root,id);
// }

// Parsers
void* portParser(const char*line) {
  // parse port line
  unsigned int stationId,portNum;
  char typeStr[10],license[LICENSE_SIZE];
  int status,y,m,d,h,min;
  
  //1. parse line
  if (sscanf(line, "%u,%u,%9[^,],%d,%d,%d,%d,%d,%d,%8s",
               &stationId, &portNum, typeStr, &status, &y, &m, &d, &h, &min, license) != 10) {
      displayError(ERR_PARSING,"[portParser] Failed to parse line");
      return NULL;
    }
  // 2.convert string to portType
  PortType parsedType = Util_parsePortType(typeStr);
  if (parsedType == -1) {
    displayError(ERR_PARSING,"[portParser] Invalid port type in line");
    return NULL;
  }

  if(status <OCC|| status > OOD){
    displayError(ERR_PARSING,"[portParser] invalid status value");
    return NULL;
  }

  // 3.create port
  Port* port = createPort(portNum,parsedType,(PortStatus)status,(Date){y,m,d,h,min});
  if (!port) {
    displayError(ERR_PARSING,"[portParser] Failed to create port");
    return NULL;
  }

  // 4.allocate temp
  PortTempData* temp = malloc(sizeof(PortTempData));
  if (!temp) {
    displayError(ERR_MEMORY,"[portParser] Failed to allocate memory");
    destroyPort(port);
    return NULL;
  }
    

  temp->port = port;
  temp->stationId = stationId;
  strncpy(temp->license, license, LICENSE_SIZE);
  return temp;
}

void* stationParser(const char* line) {
  return Station_parseLine(line);
}

void* carsParser(const char* line){
  return parseCarLine(line);
}

void* lineOfCarParser(const char* line){
  if(!line) return NULL;

  LineOfCarsEntry* entry = malloc(sizeof(LineOfCarsEntry));
  if(!entry) {
    displayError(ERR_PARSING,"[lineOfCarParser] Memory allocation failed");
    return NULL;
  }

  if(sscanf(line,"%8s,%u",entry->license,&entry->stationId) != 2) {
    displayError(ERR_PARSING, "[lineOfCarParser] Failed to parse line");
    free(entry);
    return NULL;
  }
  return entry;
}

// processors
void linkPortToStation(void*obj, void*context) {
  
  PortTempData* temp = (PortTempData*)obj;
  SystemData* sys = (SystemData*)context;
  if(!temp||!sys) {
    displayError(ERR_LOADING_DATA,"[linkPortToStation]Null Data");
    return;
  }

  Port* port = temp->port;

  // 1.find the station for this port
  SearchKey key = {.type = SEARCH_BY_ID,.id = temp->stationId};
  Station* station = searchStation(&sys->stationTree, &key);
  if (!station) {
    destroyPort(port);
    free(temp);
    return;
  }
  
  // 2.add port to station
  addPortToStation(station,temp->port);

  // 3.link car to port
  if (isLicenseValid(temp->license)) {
    Car* car = searchCar(&sys->carTree, temp->license);
    if (car) {
      assignCar2Port(port,car,port->tin);
    } else {
      displayError(ERR_LOADING_DATA, "[linkPortToStation] Car not found for assignment");
    }
  }
    free(temp);
}

void lineOfCarsProcessor(void* obj,void* context){
  if(!obj||!context) {
    displayError(ERR_LOADING_DATA, "[lineOfCarsProcessor] Null data");
    return;
  }

  LineOfCarsEntry* entry = (LineOfCarsEntry* )obj;
  SystemData* sys = (SystemData*) context;

  SearchKey key = {.id = entry->stationId, .type = SEARCH_BY_ID};

  Station * station = searchStation(&sys->stationTree,&key);
  if (!station) {
        displayError(ERR_LOADING_DATA, "[lineOfCarsProcessor] Station not found");
        return;
    }
  Car* car = searchCar(&sys->carTree, entry->license);
    if (!car) {
        displayError(ERR_LOADING_DATA, "[lineOfCarsProcessor] Car not found");
        return;
    }
    if (!enqueueCarToStationQueue(station, car)) {
        displayError(ERR_LOADING_DATA, "[lineOfCarsProcessor] Failed to enqueue car");
    }
}

// destroyes
void saveAndCleanupSystem(SystemData *sys) {
  if(!sys) return;

  // free trees
  if (sys->stationTree.root) {
    destroyTree(sys->stationTree.root, sys->stationTree.destroy);
  }
  if (sys->carTree.root) {
    destroyTree(sys->carTree.root, sys->carTree.destroy);
  }
    
  free(sys);
}

void destroyPortTemp(void* obj) {
  PortTempData* temp = (PortTempData*)obj;
  if (temp) {
    destroyPort(temp->port);
    free(temp);
  }
}

void destroyLineOfCars(void* obj){
  free(obj);
}


// Loading files

SystemData* loadFiles(){
  // init systemData struct to handle data
  SystemData *sys = malloc(sizeof(SystemData));
  if(!sys) {
    displayError(ERR_LOADING_DATA,"Failed to allocate memory in SystemData\n");
    return NULL;
  }


  // init car tree
  sys->carTree = initTree(compareCarsByLicense,printCar,destroyCar);
  // init station tree
  sys->stationTree = initTree(compareStationById,printStation,StationDestroy);


  // load all files and checks if fail free sys
  if(loadStations(&sys->stationTree)<=0 || loadCars(&sys->carTree) <=0 || loadPorts(sys)<=0||loadLineOfCars(sys)<=0) {
    saveAndCleanupSystem(sys);
    return NULL;
  }
  

  return sys;
}

int fileLoader(const FileLoaderConfig *config) {
  char msg[128];
  // 1. Validate configuration
  if(!config || !config->filename || !config->parser || !config->destroyObject) {
    displayError(ERR_LOADING_DATA,"Invalid loader configuration\n");
    return -1;
  }

  // 2. Open file
  FILE* file = fopen(config->filename, "r");
  if(!file) {
    snprintf(msg,sizeof(msg),"Failed to open file from %s",config->filename);
    displayError(ERR_LOADING_DATA,msg);
    perror(config->filename);
    return -1;
  }

  // 3. Skip header if requested
  if(config->skipHeader) {
    char header[256];
    if(!fgets(header, sizeof(header), file)) {
      fclose(file);
      return 0;  // empty file
    }
  }

  // 4. Process lines
  char line[512];
  int count = 0;
  int lineNum = 0;
  
  while(fgets(line, sizeof(line), file)) {
    lineNum++;
    trimNewLine(line);
    if(checkLineOverflow(file,line,sizeof(line),lineNum,config->filename)) {
      continue;
    }

    
    // 5. Parse line
    void* obj = config->parser(line);
    if(!obj) {
      continue;
    }

    // 6. Insert to tree if requested
    if(config->targetTree) {
      if(!insertBST(config->targetTree, obj)) {
        config->destroyObject(obj);
        continue;
      }
    }

    // 7. Post-process
    if(config->processor) {
      config->processor(obj, config->context);
    }
    
    count++;
  }

  // 8. Cleanup
  fclose(file);
  return count;
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
  return fileLoader(&config);
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
  return fileLoader(&config);
}

int loadPorts(SystemData *sys){
  FileLoaderConfig config = {
    .filename = "data/Ports.txt",
    .targetTree = NULL,
    .parser = portParser,
    .processor = linkPortToStation,
    .context = sys,
    .destroyObject = destroyPortTemp,
    .skipHeader = 1
  };
  return fileLoader(&config);
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

  return fileLoader(&config);
}

// update files
SystemData *updateFiles(){

  return NULL;
}