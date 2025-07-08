#include "Port.h"
#include "ErrorHandler.h"
#include "Cars.h"
#include "Station.h"
#include "Queue.h"



Port *createPort(unsigned int num, PortType type,PortStatus status,Date t) {
  Port* port = (Port*)malloc(sizeof(Port));
  if(!port) {
    perror("Failed alocate memory on Port");
    return NULL;
  }

  port->num = num;
  port->portType = type;
  port->status = status;
  port->p2Car = NULL;
  port->tin = t;

  port->next = NULL;

  return port;
}

Port *insertPort(Port* head, Port*newPort) {
  if(!newPort) return head;

  newPort->next = head;
  return newPort;
}

Port *findPort(Port *head, unsigned int num){
  Port* current = head;

  while (current != NULL)
  {
    if(current->num == num) {
      return current;
    }
    current = current->next;
  }
 
  return NULL;
}

BOOL assignCar2Port(Port* port, Car* car, Date startTime) {
  if(!port||!car) {
    displayError(ERR_LOADING_DATA, "[assignCar2Port] Invalid port or car pointer");
    return FALSE;
  }

  // Check if port is available
  if (!isPortAvailable(port)) {
    return FALSE;
  }

  //cheack portType compatiblity
  if (!isCompatiblePortType(car->portType, port->portType)) {
    return FALSE;
  }
   // assignCar2Port
  port->p2Car = car;
  port->tin = startTime;
  port->status = OCC;
  car->pPort = port;
  car->inqueue = FALSE;
    
    return TRUE;

}

void unlinkCarPort(Car* car){
  if(!car||!car->pPort) return;

  Port *port = car->pPort;
  port->status = FREE;
  port->p2Car = NULL;
  car->pPort = NULL;
}

void tryAssignNextCarFromQueue(Station *station, Port *port, Date now) {
  if( !station|| !port|| !station->qCar) return;
  if(isEmpty(station->qCar)) {
    printf("No cars waiting in station");
    return;
  }

  PortType pType = port->portType;
  Car* nextCar = dequeueByPortType(station->qCar,pType);

  if(nextCar){
    printf("Next compatible car in queue: %s\n", nextCar->nLicense);
    if(assignCar2Port(port,nextCar,now)){
      printf("Assigned car %s to port %u at station %s.\n", nextCar->nLicense, port->num, station->name);
    } else {
      printf("failed to assign car\n");
      enqueue(station->qCar,nextCar);
      nextCar->inqueue = TRUE;
    }
  } else {
    printf("No compatible car in queue with port type %s\n",portTypeToStr(port->portType));
  }

}

BOOL isCompatiblePortType(PortType carType, PortType portType) {
  return(carType == portType);
}

BOOL isPortAvailable(Port* port){
  char msg[128];
  if(!port) {
    printf("[DEBUG] isPortAvailable called with NULL port pointer\n");
    return FALSE;
  }

  if(port->status == OCC && port->p2Car != NULL) {
    return FALSE;
  }
  if (port->status == OOD) {
    return FALSE;
  }
  
  return TRUE;
}

void printPortList(Port *head) {
  if(head== NULL) 
  {
    printf("No Ports\n");
    return;
  }

  printf("---PortList---\n");
  Port* current = head;
  while (current != NULL) {
    printPort(current);
    current=current->next;
  }
  printf("------\n");
  
}

void printPort(const Port* port) {

  printf("| Port Number: %u  ,  Type: %s  ,  Status: %s |\n",port->num,portTypeToStr(port->portType),statusToStr(port->status));

}

void destroyPortList(Port *head) {
  Port* current = head;
  while (current != NULL)
  {
    Port* tmp = current;
    current = current->next;

    free(tmp);
  }
  
}

void destroyPort(void *data){
  Port* port = (Port*)data;
  if(port){
    free(port);
  }
}

int countFreePorts(const Port* head) {
  int count = 0;
  const Port* current = head;
  while (current)
  {
    if(current->status == FREE) count++;
    current=current->next;
  }
  return count;
}

BOOL isPortTypeValid(const char* pTypeKey) {
  return (
    Util_parsePortType(pTypeKey) != -1
  );
}

int getNextPortNum(Station* station){
  if(!station || !station->portsList) return 0;
  int nextPortNum = 1;
  Port *current = station->portsList;

  while (current)
  {
    if(current->num >= nextPortNum)
      nextPortNum = current->num + 1;

    current = current->next;
  }
  
  return nextPortNum;
}


// Port* findAvailablePort(Port* portList, PortType type) {
//   printf("[DEBUG] Scanning port list for type: %s\n", portTypeToStr(type));
//   Port* current = portList;

//   while (current)
//   {
//       if(current->status == FREE && isCompatiblePortType(type,current->portType)) {
//         printf("[DEBUG] Found matching port: #%u (Status: %s)\n",
//        current->num, statusToStr(current->status));
//         return current;
//       }
//       current = current->next;

//   }
  
//   return NULL;
// }
