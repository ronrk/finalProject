#include "SystemData.h"

#include "Station.h"
#include "Cars.h"
#include "Port.h"
#include "Utilis.h"
#include "ErrorHandler.h"
#include "Queue.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct 
{
  Port* port;
  unsigned int stationId;
  char license[LICENSE_SIZE];
}PortTempData;


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
  addPortToStation(station,temp->port,FALSE);

  // 3.link car to port
  if (isLicenseValid(temp->license)) {
    Car* car = searchCar(&sys->carTree, temp->license);
    if (car&& port->status == OCC) {
      port->p2Car = car;
      port->status = OCC;
      car->pPort = port;
      car->inqueue = 0;
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

// 
// 
// update files
// save stations
void saveStationLine(FILE* fp, Station* station){
  if(!station) return;
  fprintf(fp,"%u,%s,%d,%.2lf,%.2lf\n",station->id,station->name,station->nPorts,station->coord.x,station->coord.y);
}
void saveStationInorder(TreeNode* root, FILE* fp){
  if(!root) return;
  saveStationInorder(root->left,fp);
  saveStationLine(fp,(Station*) root->data);
  saveStationInorder(root->right,fp);
}
int saveStationToFile(TreeNode* root,const char* fileName){
  FILE* fp = fopen(fileName,"w");
  if(!fp){
    perror("Failed to open stations file for writing");
    return 0;
  }
  fprintf(fp, "ID,StationName,NumOfPorts,CoordX,CoordY\n");
  saveStationInorder(root,fp);
  fclose(fp);

  return 1;
}

// save cars
void saveCarLine(BinaryTree* stationTree,FILE* fp, Car* car){
  unsigned int stationId = 0;
  unsigned int portNum = 0;
  const char* portType = portTypeToStr(car->portType);

  if(car->pPort) {
    portNum = car->pPort->num;
    Station* station = findStationByCar(stationTree,car);
    if(station) stationId = station->id;
  }

  Station* station = findStationByCar(stationTree,car);
  fprintf(fp,"%s,%s,%.2lf,%u,%d,%d\n",
  car->nLicense,portType,car->totalPayed,stationId,portNum,car->inqueue);
}
void saveCarInorder(TreeNode* root, FILE* fp,BinaryTree* stationTree){
  if(!root) return;
  saveCarInorder(root->left,fp,stationTree);
  saveCarLine(stationTree,fp,(Car*) root->data);
  saveCarInorder(root->right,fp,stationTree);
}
int saveCarToFile(TreeNode* root,const char* fileName,BinaryTree* stationTree){
  FILE* fp = fopen(fileName,"w");
  if(!fp){
    perror("Failed to open cars file for writing");
    return 0;
  }
  fprintf(fp, "License,PortType,TotalPayed,StationID,PortNumber,InQueue\n");
  saveCarInorder(root,fp,stationTree);
  fclose(fp);

  return 1;
}

// save cars
void savePortLine(FILE* fp, unsigned int stationId,Port* port){
  const char* portType = portTypeToStr(port->portType);
  const char* license = "-1";

  if(port->p2Car) {
    license = port->p2Car->nLicense;
  }

  fprintf(fp,"%d,%d,%s,%d,%d,%d,%d,%d,%d,%s\n",
    stationId,port->num,portType,port->status,port->tin.year,port->tin.month,port->tin.day,port->tin.hour,port->tin.min,
  license);
}
void savePortsInStation(TreeNode* root,FILE* fp){
  if(!root) return;

  savePortsInStation(root->left,fp);

  Station*station = (Station*) root->data;
  Port* current = station->portsList;

  while(current){
    savePortLine(fp,station->id,current);

    current = current->next;
  }

  savePortsInStation(root->right,fp);
}
int savePortsToFile(TreeNode* root,const char* fileName){
  FILE* fp = fopen(fileName,"w");
  if(!fp){
    perror("Failed to open cars file for writing");
    return 0;
  }
  fprintf(fp, "StationID,PortNumber,PortType,Status,Year,Month,Day,Hour,Min,CarLicense\n");
  savePortsInStation(root,fp);
  fclose(fp);

  return 1;
}

void saveQueueOfStations(FILE* fp, Station* station){
  if(!station||!fp) return;
  CarNode* current = station->qCar->front;
  while (current)
  {
    fprintf(fp,"%s,%u\n",current->data->nLicense,station->id);
    current= current->next;
  }
  
}
void saveAllQueueInOrder(TreeNode* root,FILE*fp){
  if(!root) return;

  saveAllQueueInOrder(root->left,fp);

  Station* station = (Station*) root->data;
  saveQueueOfStations(fp,station);

  saveAllQueueInOrder(root->right,fp);
}
int saveLinesOfCars(TreeNode* root, const char* fileNmae){
  FILE*fp = fopen(fileNmae,"w");
  if(!fp) return 0;

  fprintf(fp,"License,StationID\n");
  saveAllQueueInOrder(root,fp);
  fclose(fp);
  return 1;
}

int updateFiles(SystemData* sys){
  if(!sys) return 0;
  saveStationToFile(sys->stationTree.root,"data/Stations.txt");
  saveCarToFile(sys->carTree.root,"data/Cars.txt",&sys->stationTree);
  savePortsToFile(sys->stationTree.root,"data/Ports.txt");
  saveLinesOfCars(sys->stationTree.root,"data/LineOfCars.txt");

  return 1;
}