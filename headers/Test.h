#ifndef TEST_U
#define TEST_U

#include "SystemData.h"
#include "Station.h"
#include "BinaryTree.h"
#include "Queue.h"
#include "Utilis.h"
#include "Cars.h"
#include <stdio.h>

void testLoadFiles();
void printSingleStationQueue(const void *data);
void printStationQueues(SystemData *sys);
void testQueueOperations();
void assignCarsToAvailablePorts(SystemData *sys, Date now);
void printCarQueue(qCar* queue, const char* stationName);
void testStopCharge(BinaryTree* stationTree, BinaryTree* carTree);
void initTestSystem(SystemData *sys);


void run_loader_tests();
void run_parser_tests();
void test_locateNearSt_feature(SystemData* sys);
void test_chargeCar_feature(SystemData* sys);
void test_stopCharg(SystemData* sys);

#endif