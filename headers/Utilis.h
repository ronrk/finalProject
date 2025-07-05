#ifndef UTILIS_H
#define UTILIS_H

#define LICENSE_SIZE 9
#define MAX_NAME 100
#define RATE_CHARGE 1.2

#include "BinaryTree.h"
#include <stdio.h>
#include <time.h>

typedef enum
{
  SEARCH_BY_ID,
  SEARCH_BY_NAME,
  SEARCH_BY_DISTANCE,
  SEARCH_TYPE_COUNT
} SearchType;

typedef struct
{
  SearchType type;
  union
  {
    char name[MAX_NAME];
    int id;
  };
  struct
  {
    double userX;
    double userY;
  } location;
} SearchKey;

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

// enums
typedef enum
{
  FAST,
  MID,
  SLOW
} PortType;
typedef enum
{
  FALSE,
  TRUE
} BOOL;
typedef enum
{
  OCC = 1,
  FREE,
  OOD
} PortStatus;

// struct
typedef struct
{
  double x;
  double y;
} Coord;
typedef struct
{
  unsigned int year;
  unsigned int month;
  unsigned int day;
  unsigned int hour;
  unsigned int min;
} Date;

// functions

Date getCurrentDate();
int diffInMin(Date start,Date end);

// clear input buffer avoid new line
void clearInputBuffer();
// remove new line
void removeTrailingNewline(char *str);
// if input is a cancel signal
BOOL isCancelInput(const char *str);

// get and validate coords
BOOL getCoordFromUser(Coord *coord, const char *promptX, const char *promptY);
BOOL getDoubleFromUser(double *outValue, const char *prompt);

// handle enumstoStr and opposite
const char *portTypeToStr(PortType type);
PortType Util_parsePortType (const char* str);
const char *statusToStr(PortStatus status);
PortStatus Util_parsePortStatus(const char *str);

// calculate distance
double calculateDistance(Coord c1, Coord c2);

int strEqualsIgnoreCase(const char *a, const char *b);

BOOL isNumericString(const char *str);

BOOL getLineFromUser(char *buffer, size_t size);

BOOL getInputAndCancel(char *buffer, size_t size, const char *prompt);

BOOL parseStationInput(const char *input, SearchKey *key, SearchType *outType);

#endif // UTILIS_H