#include "Test.h"
#include "Port.h"
#include "Menu.h"
#include "SystemData.h"
#include "BinaryTree.h"
#include "ErrorHandler.h"
#include "Utilis.h"
#include <assert.h>
#include <stdio.h>


// static
static BOOL getLicenseFromUser(char *input,size_t size) {
  while (1)
  {
    /* code */
    if(!getInputAndCancel(input,size,"Enter car license number (8 digits): ")) {
      displayError(UI_WARNING,"Canceled by user");
      return FALSE;
    }

    if(!isLicenseValid(input)) {
      displayError(UI_WARNING,"Invalid license format. Try again");
      continue;
    }
    return TRUE;
  }

}


static void printStationPorts(const void *data) {
    if (!data) return;
    const Station *station = (const Station *)data;
    printf("--- Ports for Station %s (ID: %d) ---\n", station->name, station->id);
    printPortList(station->portsList);
}

static void printQueueForStation(const Station* station) {
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


// Test function

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

    BinaryTree stationTree = initTree(compareStationById, printStation, StationDestroy);
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

    BinaryTree carTree = initTree(compareCarsByLicense, printCar, destroyCar);
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

    sys->carTree = initTree(compareCarsByLicense, printCar, destroyCar);
    sys->stationTree = initTree(compareStationById, printStation, StationDestroy);

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

    sys->stationTree = initTree(compareStationById, printStation, StationDestroy);
    sys->carTree = initTree(compareCarsByLicense, printCar, destroyCar);

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
    printf("\n[TEST] locateNearSt Feature - simulate input: \n%s\n", simulatedInput);

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

// 8.

// 9. Add a new port
void test_addNewPort_run(SystemData* sys,const char* simulatedInput) {
    printf("\n[TEST] addNewPort feature - simulate input:\n%s\n ",simulatedInput);

    FILE *fakeInput = fmemopen((void* )simulatedInput,strlen(simulatedInput),"r");

    if (!fakeInput) {
        printf("[FAIL] Could not simulate input\n");
        return;
    }

    FILE* originStd = stdin;
    stdin = fakeInput;

    addNewPort(&sys->stationTree);

    stdin = originStd;
    fclose(fakeInput);
}

void test_addNewPort_feature(SystemData*sys){
    if(!sys||!sys->stationTree.root) {
        printf("[FAIL] No system data loaded for addNewPort test\n");
        return;
    }

    printf("\n========== TEST: addNewPort ==========\n");

    // TEST 1: valid input (station by id) + valid port type
    test_addNewPort_run(sys, "101\nFAST\n"); // add FAST port to 101

    // TEST 2: valid input (station by name) + valid port type
    test_addNewPort_run(sys, "Haifa Port\nSLOW\n"); //add SLow port to Haifa

    // TEST 3: cancel station selection
    test_addNewPort_run(sys, "0\n");

    // TEST 4: invalid station input then valid
    test_addNewPort_run(sys, "invalidStation\n101\nFAST\n"); //invalid -> 101 -> add FAST

    // TEST 5: cancel at port type input
    test_addNewPort_run(sys, "101\n\n"); // 101 -> invalid portType

    // TEST 6: invalid port type, then valid
    test_addNewPort_run(sys, "101\nWRONGTYPE\nFAST\n"); // 101 -> invalidPort -> FAST

    // TEST 7: station has no ports initially
    test_addNewPort_run(sys, "999\nMID\n"); // add to empty station (Test Station) MID port

    printf("\n========== END TEST: addNewPort ==========\n");
}


// run all tests
void test_runAllTests(SystemData *sys) {
    printf("\n====================TEST:Run All Features====================\n");
    test_locateNearSt_feature(sys);
    test_chargeCar_feature(sys);
    test_stopCharg(sys);
    test_addNewPort_feature(sys);

    printf("\n====================END:Run All Features====================\n");
}
// 
