#include "BinaryTree.h"
#include "Menu.h"
#include "ErrorHandler.h"
#include "Test.h"

#include <stdio.h>
#include <stdlib.h>



int main() {
    // testQueueOperations();
    // run_loader_tests();

    SystemData *sys = loadFiles();
    if (!sys) {
        fprintf(stderr, "Failed to load system data\n");
        return 1;
    }
        
    mainMenu(sys); 
    saveAndCleanupSystem(sys);

    return 0;
}
