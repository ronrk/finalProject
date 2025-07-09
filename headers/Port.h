#ifndef PORT_H
#define PORT_H


#include "Utilis.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct Car Car;
typedef struct Station Station;

typedef struct Port Port;
struct Port
{
  unsigned int num;
  PortType portType;
  PortStatus status;
  Car *p2Car; // car in port
  Date tin;   // time for start charge
  struct Port *next;
};

// functions

// crate new port
Port *createPort(unsigned int num, PortType type, PortStatus status, Date t);

// insert port to the head of the list
Port *insertPort(Port *head, Port *newPort);

// assign car2Port
BOOL assignCar2Port(Port *port, Car *car, Date date);
// unlink carPort
void unlinkCarPort(Car* car, double bill);
void tryAssignNextCarFromQueue(Station *station, Port *port, Date now);

// find port by num
Port *findPort(Port *head, unsigned int num);

int getOutOrderPortNum(Port *port);

void removePortFromStation(Station *station, unsigned int portNum);

void printPortList(Port *head);
void printPort(const Port *port);

// destroy list
void destroyPortList(Port *head);
void destroyPort(void *data);

// count free ports
int countFreePorts(const Port *head);

BOOL isCompatiblePortType(PortType carType, PortType portType);


BOOL isPortAvailable(Port *port);

// validate port
BOOL isPortTypeValid(const char *pTypeKey);

int calculateChargeTime(Port* port);

// find available port
// Port *findAvailablePort(Port *portList, PortType type);

// get next available port num at station
int getNextPortNum(Station* station);
#endif