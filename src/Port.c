#include "../headers/Port.h"
#include "../headers/Cars.h"
// #include "../headers/Utilis.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


Port *createPort(unsigned int stationId,unsigned int num, PortType type,PortStatus status,Car* car,Date t,const char* license) {
  Port* newPort = malloc(sizeof(Port));
  if(!newPort) {
    perror("Failed alocate memory on Port");
    return NULL;
  }
  newPort->stationId = stationId;
  newPort->num = num;
  newPort->portType = type;
  newPort->status = status;

  if(car) newPort->p2Car = car;
  else NULL;
  
  strncpy(newPort->license,license,8);
  newPort->license[8] = '\0';

  // initialize time
  newPort->tin.year=t.year;
  newPort->tin.month=t.month;
  newPort->tin.day = t.day;
  newPort->tin.hour = t.hour;
  newPort->tin.min = t.min;

  newPort->next = NULL;

  return newPort;
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

Port* findAvailablePort(Port* portList, PortType type) {
  printf("[DEBUG] Scanning port list for type: %s\n", portTypeToStr(type));
  Port* current = portList;

  while (current)
  {
      if(current->status == FREE && isCompatiblePortType(type,current->portType)) {
        printf("[DEBUG] Found matching port: #%u (Status: %s)\n",
       current->num, statusToStr(current->status));
        return current;
      }
      current = current->next;

  }
  
  return NULL;
}

BOOL assignCar2Port(Port* port, Car* car, Date startTime) {

  if(!port||!car) {
    fprintf(stderr, "assignCar2Port: Invalid port or car pointer.\n");
    return FALSE;
  }

  // check if car type == port type
  if(!isCompatiblePortType(car->portType,port->portType)) {
    return FALSE;
  }

  // check if port is available
  if(isPortAvailable(port)) {
    port->p2Car = car;

    strncpy(port->license,car->nLicense,8);
    port->license[8] = '\0';

    port->status = OCC;
    port->tin = startTime;
    car->pPort = port;
    car->inqueue = FALSE;
    return TRUE;
  }
  return FALSE;
}

BOOL isCompatiblePortType(PortType carType, PortType portType) {
  return(carType == portType);
}

BOOL isPortAvailable(Port* port){
  if(port->status == OCC) {
    fprintf(stderr, "assignCar2Port: Port %u is already occupied.\n", port->num);
    return FALSE;
  }
  if (port->status == OOD) {
        fprintf(stderr, "assignCar2Port: Port %u is out-of-order.\n", port->num);
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
  printf("Port Number: %u  |  Type: %s  |  Status: %s\n",port->num,portTypeToStr(port->portType),statusToStr(port->status));

      if(port->p2Car) {
        printf("  Charging Car: %s\n",port->p2Car->nLicense);

        printf("Since: %04d-%02d-%02d %02d:%02d\n",port->tin.year,port->tin.month,port->tin.day,port->tin.hour,port->tin.min);
      } else {
        printf("  Charging Car: None\n");
      }
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