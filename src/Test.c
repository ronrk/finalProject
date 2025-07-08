#include "Test.h"
#include "Port.h"
#include "Menu.h"
#include "SystemData.h"
#include "BinaryTree.h"
#include "Utilis.h"
#include <assert.h>
#include <stdio.h>


// Test function
void testQueueOperations() {
    // Create queue
    qCar *queue = createQueue();
    if (!queue) {
        printf("Failed to create queue\n");
        return;
    }

    // Create some test cars
    Car car1 = {.nLicense = "12345678", .portType = FAST};
    Car car2 = {.nLicense = "87654321", .portType = SLOW};
    Car car3 = {.nLicense = "23145768", .portType = FAST};
    Car car4 = {.nLicense = "23513684", .portType = SLOW};

    // Enqueue cars
    enqueue(queue, &car1);
    enqueue(queue, &car2);
    enqueue(queue, &car3);
    enqueue(queue, &car4);

    printf("Queue after enqueueing 4 cars:\n");
    printQueue(queue);

    // Dequeue one car normally (FIFO)
    Car *dCar = dequeue(queue);
    if (dCar) {
        printf("Dequeued car (FIFO): %s\n", dCar->nLicense);
    }

    printf("Queue after dequeue FIFO:\n");
    printQueue(queue);

    // Dequeue by port type FAST
    Car *dCarFast = dequeueByPortType(queue, FAST);
    if (dCarFast) {
        printf("Dequeued car by port type FAST: %s\n", dCarFast->nLicense);
    } else {
        printf("No car with port type FAST found to dequeue.\n");
    }

    printf("Queue after dequeue by port type FAST:\n");
    printQueue(queue);

    // Cleanup
    destroyQueue(queue);
}

void printStationPorts(const void *data) {
    if (!data) return;
    const Station *station = (const Station *)data;
    printf("--- Ports for Station %s (ID: %d) ---\n", station->name, station->id);
    printPortList(station->portsList);
}

void printQueueForStation(const Station* station) {
    if (!station) return;

    printf("Station %s (ID: %u):\n", station->name, station->id);

    if (station->qCar && !isEmpty(station->qCar)) {
        CarNode* current = station->qCar->front;
        while (current) {
            printf(" - Car %s | PortType: %s\n", current->data->nLicense, portTypeToStr(current->data->portType));
            current = current->next;
        }
    } else {
        printf(" - No cars in queue\n");
    }
    printf("------\n");
}

// Helper function to print queue contents

// TEST LOAD FILES
// test parsing station to line
void test_stationParser_valid(){
    printf("\n=== Test: stationParser ===\n");
    // valid line
    const char *line = "101,Tel Aviv Center,4,32.0853,34.7818";
    Station *station = stationParser(line);
    // check NULL
    assert(station!=NULL);
    assert(station->id == 101);
    assert(station->nPorts == 4);
    assert(station->coord.x == 32.0853f);
    assert(station->coord.y == 34.7818f);
    printf("[PASS] stationParser: valid input parsed successfully\n");
    StationDestroy(station);
}

void test_stationParser_invalid() {
    const char *badLine = "invalid,line,with,bad,data";
    Station *station = stationParser(badLine);
    assert(station == NULL);
    printf("[PASS] stationParser: invalid input handle correctly\n");
}

void run_parser_tests() {
    printf("\n==== TEST: stationParser ====\n");
    test_stationParser_valid();
    test_stationParser_invalid();
    printf("==== DONE: stationParser ====\n");
}

// load stations
void test_loadStations_basic() {
    printf("\n==== TEST: loadStations ====\n");

    BinaryTree stationTree = initTree(compareStation, printStation, StationDestroy);
    int count = loadStations(&stationTree);

    assert(count > 0);
    printf("[PASS] loadStations: Loaded %d stations successfully\n", count);

    printf("[INFO] Display loaded stations :\n");
    inorderBST(&stationTree, printStation);

    destroyTree(stationTree.root, stationTree.destroy);
}
// load cars
void test_loadCars_basic() {
    printf("\n==== TEST: loadCars ====\n");

    BinaryTree carTree = initTree(compareCars, printCar, destroyCar);
    int count = loadCars(&carTree);

    assert(count > 0);
    printf("[PASS] loadCars: Loaded %d cars successfully\n", count);

    printf("[INFO] Display loaded cars:\n");
    inorderBST(&carTree, printCar);

    destroyTree(carTree.root, carTree.destroy);
}
// load ports
void test_loadPorts_basic() {
    printf("\n==== TEST: loadPorts ====\n");

    SystemData *sys = malloc(sizeof(SystemData));
    assert(sys != NULL);

    sys->carTree = initTree(compareCars, printCar, destroyCar);
    sys->stationTree = initTree(compareStation, printStation, StationDestroy);

    int stationsLoaded = loadStations(&sys->stationTree);
    int carsLoaded = loadCars(&sys->carTree);
    assert(stationsLoaded > 0 && carsLoaded > 0);

    int portsLoaded = loadPorts(sys);
    assert(portsLoaded > 0);

    printf("[PASS] loadPorts: Loaded %d ports and linked them to stations\n", portsLoaded);

    printf("[INFO] Display ports at station:\n");
    inorderBST(&sys->stationTree, printStationPorts);

    saveAndCleanupSystem(sys);
}
// load lines of cars
void test_loadLinesOfCars_basic() {
    printf("\n==== TEST: loadLineOfCars ====\n");

    SystemData *sys = malloc(sizeof(SystemData));
    assert(sys != NULL);

    sys->stationTree = initTree(compareStation, printStation, StationDestroy);
    sys->carTree = initTree(compareCars, printCar, destroyCar);

    int stationsLoaded = loadStations(&sys->stationTree);
    int carsLoaded = loadCars(&sys->carTree);
    int portsLoaded = loadPorts(sys);
    assert(stationsLoaded > 0 && carsLoaded > 0 && portsLoaded > 0);

    int queuedCars = loadLineOfCars(sys);
    assert(queuedCars > 0);

    printf("[PASS] loadLineOfCars: Loaded %d queued cars into station queues\n", queuedCars);

    printf("[INFO] Car queues at station:\n");
    inorderBST(&sys->stationTree, (PrintFunc)printQueueForStation);

    saveAndCleanupSystem(sys);
}

void run_loader_tests() {
    test_loadStations_basic();
    test_loadCars_basic();
    test_loadPorts_basic();
    test_loadLinesOfCars_basic();
}

// 
// FEATURES
// 1. locate nearest station
void test_locateNearSt_full_run(BinaryTree* stationTree, const char* simulatedInput) {
    printf("\n[TEST] locateNearSt Feature - simulate input: \"%s\"\n", simulatedInput);

    FILE* fakeInput = fmemopen((void*)simulatedInput, strlen(simulatedInput), "r");
    if (!fakeInput) {
        printf("[FAIL] Could not simulate input\n");
        return;
    }

    FILE* origStdin = stdin;
    stdin = fakeInput;

    // run the real feature function
    locateNearSt(stationTree);

    // Restore stdin
    stdin = origStdin;
    fclose(fakeInput);
}

void test_locateNearSt_feature(SystemData* sys) {

    if(!sys||!sys->stationTree.root || !sys->carTree.root) {
        printf("No station loaded\n");
        return;
    }
    printf("\n========== TEST: locateNearSt ========== \n");

    // Valid input
    test_locateNearSt_full_run(&sys->stationTree, "32.1\n34.8\n");

    // Invalid coords (non-numeric)
    test_locateNearSt_full_run(&sys->stationTree, "abc\nxyz\n");

    // Negative coords
    test_locateNearSt_full_run(&sys->stationTree, "-10\n-20\n");

    // Boundary values
    test_locateNearSt_full_run(&sys->stationTree, "0\n0\n");
    test_locateNearSt_full_run(&sys->stationTree, "1000000\n1000000\n");

    printf("\n========== END TEST: locateNearSt ==========\n");
}   

// 
// 2.charge car
void test_chargeCar_run(SystemData* sys, const char* simulatedInput) {
    printf("\n[TEST] chargeCar Feature - simulate input:\n%s\n", simulatedInput);

    FILE* fakeInput = fmemopen((void*)simulatedInput, strlen(simulatedInput), "r");
    if (!fakeInput) {
        printf("[FAIL] Could not simulate input\n");
        return;
    }

    FILE* origStdin = stdin;
    stdin = fakeInput;

    chargeCar(&sys->stationTree, &sys->carTree);
    stdin = origStdin;
    fclose(fakeInput);
}

void test_chargeCar_feature(SystemData* sys) {
    if (!sys || !sys->stationTree.root || !sys->carTree.root) {
        printf("No system data loaded\n");
        return;
    }

    printf("\n========== TEST: chargeCar ========== \n");

    // Test 1: Existing car, valid license, station and port selection
    // Inputs: license, station ID, port number
    test_chargeCar_run(sys,
        "29145178\n"    // existing car license MID
        "205\n"         // station ID Haifa
        "2\n"           // port number MID
    );

    // Test 2: New car creation with valid inputs
    // Inputs: license, station ID, port type, port number
    test_chargeCar_run(sys,
        "99999999\n"    // new car license
        "205\n"         // Haifa
        "FAST\n"        // port type
        "1\n"           // port number
    );

    // Test 3: Invalid license input
    test_chargeCar_run(sys,
        "abc12345\n123456789\nabcd1234\n12312312\n"
        "123\ntte\nhhaif\nasdasas\ntel\n"
        "0\n"    // invalid license format & station
    );

    // Test 4: Cancel license input (simulate user cancel)
    test_chargeCar_run(sys,
        "\n"            // empty input triggers cancel
    );

    // Test 5: Cancel station selection (simulate user entering 0)
    test_chargeCar_run(sys,
        "27205295\n"    // valid license
        "0\n"           // cancel station selection
    );

    // Test 6: Invalid port type input for new car (simulate invalid type + cancel)
    test_chargeCar_run(sys,
        "88888888\n"    // new car license
        "412\n"         // Jerusalem
        "INVALID\ngoood\n12\n1\n"     // invalid port type
        "\n"            // cancel port type input
    );

    printf("\n========== END TEST: chargeCar ==========\n");
}


// 
// 4. stop charge
void test_stopCharge_run(SystemData* sys, const char* simulatedInput) {
    printf("\n[TEST] stopCharge Feature - simulate input:\n%s", simulatedInput);

    FILE* fakeInput = fmemopen((void*)simulatedInput, strlen(simulatedInput), "r");
    if (!fakeInput) {
        printf("\n[FAIL] Could not simulate input\n");
        return;
    }

    FILE* origStdin = stdin;
    stdin = fakeInput;

    stopCharge(&sys->stationTree, &sys->carTree);
    printf("\t[END]\n");
    stdin = origStdin;
    fclose(fakeInput);
}

void test_stopCharg(SystemData* sys) {
    if (!sys || !sys->stationTree.root || !sys->carTree.root) {
        printf("[FAIL] No system data loaded for stopCharge test\n");
        return;
    }

    printf("\n========== TEST: stopCharge ==========\n");

    // TEST 1: Valid car currently charging
    test_stopCharge_run(sys, "66778899\n");

    // TEST 2: Car exists but not charging
    Car* car = createCar("33333333",MID);
    insertBST(&sys->carTree,car);
    test_stopCharge_run(sys, "33333333\n"); 

    // TEST 3: Car does not exist
    test_stopCharge_run(sys, "12141516\n");

    // TEST 4: User cancels license input
    test_stopCharge_run(sys, "\n");

    // TEST 5: Car stops charging and queue has cars (manually simulate in test setup)
    test_stopCharge_run(sys, "12345678\n"); // Car assumed to be charging and has queue

    printf("\n========== END TEST: stopCharge ==========\n");
}

// 7.

// 
//  Unsused?
void assignCarsToAvailablePorts(SystemData *sys, Date now) {
    if (!sys) return;

    TreeNode *stack[100];
    int top = -1;
    TreeNode *current = sys->stationTree.root;

    while (current || top >= 0) {
        while (current) {
            stack[++top] = current;
            current = current->left;
        }

        current = stack[top--];
        Station *station = (Station *)current->data;
        printf("🚏 Checking station: %s (ID: %u)\n", station->name, station->id);

        if (station->portsList == NULL) {
          printf("⚠️  No ports found for station %u\n", station->id);
        }

        Port *p = station->portsList;
        while (p) {
          printf("🔌 Port %u (%s) is %s\n",
                p->num, portTypeToStr(p->portType),
                isPortAvailable(p) ? "AVAILABLE" : "OCCUPIED");
            if (isPortAvailable(p)) {
                Car *car = dequeueByPortType(station->qCar, p->portType);
                if (car) {
                  printCarQueue(station->qCar, station->name);
                    if (assignCar2Port(p, car, now)) {
                        printf("✅ Assigned car %s to port %u (%s) at station %u\n",
                            car->nLicense, p->num, portTypeToStr(p->portType), station->id);
                    } else {
                        printf("❌ Failed to assign car %s to port %u\n", car->nLicense, p->num);
                    }
                } else {
                    printf("⚠️ No car in queue matching port type %s\n", portTypeToStr(p->portType));
                }
            }
            p = p->next;
        }

        current = current->right;
    }
}

void printCarQueue(qCar* queue, const char* stationName) {
    if (!queue || !queue->front) {
        printf("[Queue] Station %s queue is EMPTY.\n", stationName);
        return;
    }
    CarNode* current = queue->front;
    printf("[Queue] Station %s cars in queue:\n", stationName);
    while (current) {
        printf("  Car %s, PortType %s\n", current->data->nLicense, portTypeToStr(current->data->portType));
        current = current->next;
    }
}

void testStopCharge(BinaryTree* stationTree, BinaryTree* carTree) {
  printf("\n=== TEST: Stop Charge Simulation ===\n");

  char license[LICENSE_SIZE];
  if (!getLicenseFromUser(license, sizeof(license))) {
    printf("Canceled.\n");
    return;
  }

  Car* car = searchCar(carTree, license);
  if (!car) {
    printf("Car not found.\n");
    return;
  }

  if (!car->pPort) {
    printf("Car is not charging.\n");
    return;
  }

  printf("Found car %s currently charging...\n", car->nLicense);
  printf("Simulating stop charge and checking queue assignment...\n");

  processStopCharge(car, stationTree);
}

void initTestSystem(SystemData *sys) {
    sys->carTree = initTree(compareCars, printCar, destroyCar);
    sys->stationTree = initTree(compareStation, printStation, StationDestroy);
}

void destroyBinaryTree(BinaryTree *tree) {
    if (!tree) return;
    destroyTree(tree->root, tree->destroy);
    tree->root = NULL;  // Optional: avoid dangling pointer
}


void testLoadFiles(){
  SystemData *data = loadFiles();
  if (!data) {
    printf("❌ loadFiles() failed.\n");
    return;
  }
  printf("✅ System data loaded successfully!\n");

  // 1. number of stations
  int numStations = countNodes(data->stationTree.root);
  printf("Stations loaded: %d\n", numStations);

  // 2. show number of cars
  int numCars = countNodes(data->carTree.root);
  printf("Cars loaded: %d\n", numCars);

  // 3. check if ports and queues exist
  printf("Checking ports and queues:\n");
  inorderBST(&data->stationTree, printStationSummary);

  // 4. Clean up
  saveAndCleanupSystem(data);
  printf("🧹 System cleaned up successfully\n");
}

void printStationQueues(SystemData *sys) {
    if (!sys) return;

    printf("=== Station Queues ===\n");
    inorderBST(&sys->stationTree, printSingleStationQueue);
    printf("======================\n");
}

void printSingleStationQueue(const void *data) {
    const Station *station = (const Station *)data;
    if (!station) return;

    printf("Station %u - %s queue:\n", station->id, station->name);

    qCar *queue = station->qCar;
    if (!queue || isEmpty(queue)) {
        printf("  [Empty]\n");
        return;
    }

    CarNode *current = queue->front;
    while (current) {
        Car *car = current->data;
        if (car) {
            printf("  - %s\n", car->nLicense);
        }
        current = current->next;
    }
}
