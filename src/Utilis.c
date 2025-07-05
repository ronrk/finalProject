#include "../headers/Utilis.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>



Date getCurrentDate() {
  time_t now = time(NULL);
  struct tm *tm_now = localtime(&now);

  Date date;
  date.year = tm_now->tm_year + 1900;
  date.month = tm_now->tm_mon + 1;
  date.day = tm_now->tm_mday;
  date.hour = tm_now->tm_hour;
  date.min = tm_now->tm_min;

  return date;
}

int diffInMin(Date start,Date end) {
  struct tm tm_start = {0};
  tm_start.tm_year = start.year - 1900;
  tm_start.tm_mon = start.month - 1;
  tm_start.tm_mday = start.day;
  tm_start.tm_hour = start.hour;
  tm_start.tm_min = start.min;

  struct tm tm_end = {0};
  tm_end.tm_year = end.year - 1900;
  tm_end.tm_mon = end.month - 1;
  tm_end.tm_mday = end.day;
  tm_end.tm_hour = end.hour;
  tm_end.tm_min = end.min;

  time_t time_start = mktime(&tm_start);
  time_t time_end = mktime(&tm_end);

  if (time_start == (time_t)(-1) || time_end == (time_t)(-1)) {
        // Invalid time conversion
      return 0;
  }

  double diff_seconds = difftime(time_end, time_start);

  if (diff_seconds < 0) return 0;

  int diff_minutes = (int)(diff_seconds / 60);

  return diff_minutes;
}

int strEqualsIgnoreCase(const char* a, const char* b){
  while(*a&&*b) {
    char ca = *a;
    char cb = *b;

    // 
    if(ca >= 'A' && ca <= 'Z') ca += 32;
    if(cb >= 'A' && cb<= 'Z') cb += 32;

    if(ca!=cb) return 0;

    a++;
    b++;
  }
  return *a == *b;
}

BOOL getDoubleFromUser(double* outValue,const char* prompt) {
  char input[64];
  while (TRUE)
  {
    printf("%s",prompt);
    if(!getLineFromUser(input,sizeof(input))) {
      return FALSE;
    }
    if(isCancelInput(input)) {
      return FALSE;
    }

    char* endptr;
    double val = strtod(input,&endptr);

    if(*endptr!='\0') {
      printf("Invalid number, please try again.\n");
      continue;
    }
    *outValue = val;
    return TRUE;
  }
  
}
BOOL getCoordFromUser(Coord *coord, const char* promptX, const char* promptY){
  if(!coord) return FALSE;

  if(!getDoubleFromUser(&coord->x,promptX)) {
    return FALSE;
  }

  if(!getDoubleFromUser(&coord->y,promptY)) {
    return FALSE;
  }

  return TRUE;

}

int loadDataFile(const FileLoaderConfig *config) {
  // 1. Validate configuration
  if(!config || !config->filename || !config->parser || !config->destroyObject) {
    fprintf(stderr, "Invalid loader configuration\n");
    return -1;
  }

  // 2. Open file
  FILE* file = fopen(config->filename, "r");
  if(!file) {
    perror(config->filename);
    return -1;
  }

  // 3. Skip header if requested
  if(config->skipHeader) {
    char header[256];
    if(!fgets(header, sizeof(header), file)) {
      fclose(file);
      return 0;  // Empty file is not an error
    }
  }

  // 4. Process lines
  char line[512];
  int count = 0;
  int lineNum = 0;
  
  while(fgets(line, sizeof(line), file)) {
    lineNum++;
    
    // Remove newline
    line[strcspn(line, "\r\n")] = '\0';
    
    // Check for overflow
    if(strlen(line) == sizeof(line)-1 && line[sizeof(line)-2] != '\n') {
      fprintf(stderr, "Line %d too long in %s\n", lineNum, config->filename);
      // Consume remaining line
      int ch;
      while((ch = fgetc(file)) != EOF && ch != '\n');
      continue;
    }

    // 5. Parse line
    void* obj = config->parser(line);
    if(!obj) {
      fprintf(stderr, "Parse error at %s:%d\n", config->filename, lineNum);
      continue;
    }

    // 6. Insert to tree if requested
    if(config->targetTree) {
      if(!insertBST(config->targetTree, obj)) {
        fprintf(stderr, "Insertion failed at %s:%d\n", config->filename, lineNum);
        config->destroyObject(obj);
        continue;
      }
    }

    // 7. Post-process
    if(config->processor) {
      config->processor(obj, config->context);
    }
    
    count++;
  }

  // 8. Cleanup
  fclose(file);
  return count;
}

const char* portTypeToStr(PortType type) {
  switch (type)
  {
  case FAST: return "FAST";
  case MID: return "MID";
  case SLOW: return "SLOW";
  
  default: return "Unknown";
  }
}

PortType Util_parsePortType (const char* str) { 
  if(!str) return SLOW;
  if(strEqualsIgnoreCase(str,"Fast")) return FAST;
  if(strEqualsIgnoreCase(str,"Slow")) return SLOW;
  if(strEqualsIgnoreCase(str,"Mid")) return MID;

  // if invalid
  fprintf(stderr,"Unknown port type: '%s'\n",str);

  return -1;
}

const char* statusToStr(PortStatus status) {
  switch (status)
  {
  case OCC: return "Occupied";
  case FREE: return "Free";
  case OOD: return "Out-Of-Order";
  
  default: return "Unknown";
  }
}

PortStatus Util_parsePortStatus (const char* str) { 

  if(!str) return FREE;

  if(strEqualsIgnoreCase(str,"Occupied")||strEqualsIgnoreCase(str,"occ")) return OCC;
  if(strEqualsIgnoreCase(str,"Free")) return FREE;
  if(strEqualsIgnoreCase(str,"out-of-order")||strEqualsIgnoreCase(str,"ood")||strEqualsIgnoreCase(str,"out_of_order")||strEqualsIgnoreCase(str,"out of order")) return OOD;
  
  return -1;
}

double calculateDistance(Coord c1, Coord c2) {
  double dx = c1.x - c2.x;
  double dy = c1.y - c2.y;
  return sqrt(dx*dx + dy*dy);
}

void clearInputBuffer() {
  int c;
  while ((c = getchar()) != '\n' && c != EOF);
}

void removeTrailingNewline(char *str){
  if(!str) return;
  size_t len = strlen(str);
  
  if(len > 0 && str[len-1] == '\n')
    str[len - 1] = '\0';
}

BOOL getInputAndCancel(char* buffer, size_t size, const char* prompt) {
  printf("%s",prompt);
  if(!getLineFromUser(buffer,size)) return FALSE;
  return !isCancelInput(buffer);
}

BOOL getLineFromUser(char* buffer,size_t size){

  if (!fgets(buffer, size, stdin)) {
    clearInputBuffer();
      printf("Input error.\n");
      return FALSE;
      }

size_t len = strlen(buffer);
  if (len > 0 && buffer[len - 1] != '\n') {
    clearInputBuffer();
  }

  removeTrailingNewline(buffer);


  if (buffer[0] == '\0') {
    return FALSE;
  }

  return TRUE;
}

BOOL isCancelInput(const char *str){
  if(!str) return FALSE;
  return strcmp(str, "0") == 0 ? TRUE : FALSE;
}

BOOL parseStationInput(const char* input,SearchKey* key,SearchType *outType){
  if(!input || !key || !outType) return FALSE;

  if(isNumericString(input)) {
    key->id = atoi(input);
    *outType = SEARCH_BY_ID;
  } else {
    strncpy(key->name,input,sizeof(key->name) - 1);
    key->name[sizeof(key->name) - 1] = '\0';
    *outType = SEARCH_BY_NAME;
  }
  return TRUE;
}

BOOL isNumericString(const char* str) {
  if(!str||!*str) return FALSE;

  while(*str) {
    if(*str < '0' || *str>'9') return FALSE;
    str++;
  }
  return TRUE;
}

