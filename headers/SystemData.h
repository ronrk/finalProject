#ifndef SYSTEMDATA_H
#define SYSTEMDATA_H
#include "Utilis.h"
#include "BinaryTree.h"

typedef struct SystemData SystemData;

struct SystemData
{
  BinaryTree carTree;     //  cars binary tree
  BinaryTree stationTree; //  stations binary tree
};

typedef struct LineOfCarsEntry
{
  unsigned int stationId;
  char license[LICENSE_SIZE];
} LineOfCarsEntry;

// function pointer for parsing line from text file to struct
typedef void *(*FileLineProcessor)(const char *line, void *context);
int processFileLines(const char *filename, FileLineProcessor processor, void *context, int skipHeader);

// file loader types
typedef void *(*ParseLineFunc)(const char *line);
typedef void (*PostProcessFunction)(void *obj, void *context);

typedef struct
{
  const char *filename;             // file to load
  BinaryTree *targetTree;           // tree to insert to
  ParseLineFunc parser;             // function to parse a line
  PostProcessFunction processor;    // optional post processor
  void *context;                    // additional data for process
  void (*destroyObject)(void *obj); // object destroy
  int skipHeader;                   // skip header?
} FileLoaderConfig;

int loadDataFile(const FileLoaderConfig *config);


// Functions
// load files
SystemData *loadFiles();
// destroy files
void saveAndCleanupSystem(SystemData *sys);

// load individual files
int loadStations(BinaryTree *stationTree);
int loadCars(BinaryTree *carTree);
int loadPorts(SystemData *sys);
int loadLineOfCars(SystemData *sys);

void* stationParser(const char* line);

// update files
int updateFiles(SystemData* sys);

#endif