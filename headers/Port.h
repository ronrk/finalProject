#ifndef PORT_H
#define PORT_H

#include "Utilis.h"

typedef struct Car Car;

typedef struct Port
{
  unsigned int stationId;
  unsigned int num;
  PortType portType;
  PortStatus status;
  Car *p2Car;
  Date tin;
  struct Port *next;
  char license[9];
} Port;

// functions

// crate new port
Port *createPort(unsigned int stationId, unsigned int num, PortType type, PortStatus status, Car *car, Date t, const char *license);

// insert port to the head of the list
Port *insertPort(Port *head, Port *newPort);

// assign car2Port
BOOL assignCar2Port(Port* port, Car* car, Date date);

// find port by num
Port *findPort(Port *head, unsigned int num);

void printPortList(Port *head);
void printPort(const Port *port);

// destroy list
void destroyPortList(Port *head);
void destroyPort(void *data);

// count free ports
int countFreePorts(const Port* head);

BOOL isCompatiblePortType(PortType carType, PortType portType);
// find available port
Port* findAvailablePort(Port* portList,PortType type);

BOOL isPortAvailable(Port* port);

// validate port
BOOL isPortTypeValid(const char* pTypeKey);

#endif