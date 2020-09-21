/**
 *   librib - router
 *   Developed by Christian Visintin
 * 
 * MIT License
 * Copyright (c) 2019 Christian Visintin
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
**/

#define PROGRAM_NAME "router"
#define PROGRAM_VERSION "1.0.0"
#define USAGE PROGRAM_NAME " <routingTableFile>"

#include <rib/rib.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CMD_QUT "QUIT"
#define CMD_HLP "HELP"
#define CMD_ADD "ADD"
#define CMD_DEL "DELETE"
#define CMD_UPD "UPDATE"
#define CMD_CLR "CLEAR"
#define CMD_SLT "SELECT"
#define CMD_ROT "ROUTE"
#define CMD_DMP "DUMP"
#define CMD_CMT "COMMIT"
#define CMD_RLB "ROLLBACK"

#define USAGE_QUIT "QUIT"
#define USAGE_ADD "ADD <networkAddr> <netmask> <gateway> <iface> <metric> - add a new record in the routing table"
#define USAGE_DEL "DELETE <networkAddr> <netmask/*> - delete a record in the routing table"
#define USAGE_UPD "UPDATE <networkAddr> <netmask> <newNetmask> <newGateway> <newIface> <newMetric> - update a record in the routing table"
#define USAGE_CLR "CLEAR - clear routing table"
#define USAGE_SLT "SELECT <networkAddr> <netmask/*> - retrieve routing information for a network address"
#define USAGE_ROT "ROUTE <destination> - find gateway for the provided destination"
#define USAGE_DMP "DUMP - dump all the records in the routing table"
#define USAGE_CMT "COMMIT - commit changes to the routing table"
#define USAGE_RLB "ROLLBACK - abort changes to the routing table"

typedef enum route_cmd_t {
  QUIT,
  HELP,
  ADD,
  DELETE,
  UPDATE,
  CLEAR,
  SELECT,
  ROUTE,
  DUMP,
  COMMIT,
  ROLLBACK,
  UNKNOWN
} route_cmd_t;

/**
 * @function usage
 * @description print router usage
 */

void usage() {

  printf("\t%s\n", USAGE_QUIT);
  printf("\t%s\n", USAGE_ADD);
  printf("\t%s\n", USAGE_DEL);
  printf("\t%s\n", USAGE_UPD);
  printf("\t%s\n", USAGE_CLR);
  printf("\t%s\n", USAGE_SLT);
  printf("\t%s\n", USAGE_ROT);
  printf("\t%s\n", USAGE_DMP);
  printf("\t%s\n", USAGE_CMT);
  printf("\t%s\n", USAGE_RLB);
  printf("\n");

}

/**
 * @function getCommand
 * @description get command enum from string
 * @param char*
 * @returns route_cmd_t
 */

route_cmd_t getCommand(char* commandStr) {
  if (strcmp(commandStr, CMD_ADD) == 0) {
    return ADD;
  } else if (strcmp(commandStr, CMD_DEL) == 0) {
    return DELETE;
  } else if (strcmp(commandStr, CMD_UPD) == 0) {
    return UPDATE;
  } else if (strcmp(commandStr, CMD_CLR) == 0) {
    return CLEAR;
  } else if (strcmp(commandStr, CMD_SLT) == 0) {
    return SELECT;
  } else if (strcmp(commandStr, CMD_ROT) == 0) {
    return ROUTE;
  } else if (strcmp(commandStr, CMD_DMP) == 0) {
    return DUMP;
  } else if (strcmp(commandStr, CMD_CMT) == 0) {
    return COMMIT;
  } else if (strcmp(commandStr, CMD_RLB) == 0) {
    return ROLLBACK;
  } else if (strcmp(commandStr, CMD_HLP) == 0) {
    return HELP;
  } else if (strcmp(commandStr, CMD_QUT) == 0) {
    return QUIT;
  } else {
    return UNKNOWN;
  }
}

void printRoute(const Route* r) {
  if (r->ipv == 4) {
    printf("%s\t%s\t%s\t%s\t%d\n", r->destination, r->netmask, r->gateway, r->iface, r->metric);
  } else if (r->ipv == 6) {
    printf("%s\t%d\t%s\t%s\t%d\n", r->destination, r->prefixLength, r->gateway, r->iface, r->metric);
  }
}

RIB_ret_code_t command_add(RIB* rtab, char* argv) {
  char* arg = strtok(argv, " ");
  //Define data
  char* destination = NULL;
  char* netmask = NULL;
  char* gateway = NULL;
  char* iface = NULL;
  int metric = 0;
  int argIndex;
  for (argIndex = 0; arg != NULL; argIndex++) {
    switch (argIndex) {
      case 0: {
        destination = (char*) malloc(sizeof(char) * (strlen(arg) + 1));
        strcpy(destination, arg);
        break;
      }
      case 1: {
        netmask = (char*) malloc(sizeof(char) * (strlen(arg) + 1));
        strcpy(netmask, arg);
        break;
      }
      case 2: {
        gateway = (char*) malloc(sizeof(char) * (strlen(arg) + 1));
        strcpy(gateway, arg);
        break;
      }
      case 3: {
        iface = (char*) malloc(sizeof(char) * (strlen(arg) + 1));
        strcpy(iface, arg);
        break;
      }
      case 4: {
        metric = atoi(arg);
        break;
      }
    }
    arg = strtok(NULL, " ");
  }
  if (argIndex != 5) {
    printf("%s\n", USAGE_ADD);
  }
  //Abort in case there are missing arguments
  if (destination == NULL) {
    return RIB_INVALID_ADDRESS;
  }
  if (netmask == NULL) {
    free(destination);
    return RIB_INVALID_ADDRESS;
  }
  if (gateway == NULL) {
    free(destination);
    free(netmask);
    return RIB_INVALID_ADDRESS;
  }
  if (iface == NULL) {
    free(destination);
    free(netmask);
    free(gateway);
    return RIB_INVALID_ADDRESS;
  }
  //Add route
  RIB_ret_code_t rc = RIB_add(rtab, destination, netmask, gateway, iface, metric);
  free(destination);
  free(netmask);
  free(gateway);
  free(iface);
  return rc;
}

RIB_ret_code_t command_delete(RIB* rtab, char* argv) {
  char* destination = NULL;
  char* netmask = NULL;
  char* arg = strtok(argv, " ");
  const int args = 2;
  int argIndex;
  for (argIndex = 0; arg != NULL; argIndex++) {
    switch (argIndex) {
      case 0: {
        destination = (char*) malloc(sizeof(char) * (strlen(arg) + 1));
        strcpy(destination, arg);
        break;
      }
      case 1: {
        netmask = (char*) malloc(sizeof(char) * (strlen(arg) + 1));
        strcpy(netmask, arg);
        break;
      }
    }
    arg = strtok(NULL, " ");
  }
  if (argIndex < args) {
    printf("%s\n", USAGE_DEL);
  }
  if (destination == NULL) {
    return RIB_INVALID_ADDRESS;
  }
  if (netmask == NULL) {
    free(destination);
    return RIB_INVALID_ADDRESS;
  }
  RIB_ret_code_t rc = RIB_delete(rtab, destination, netmask);
  free(destination);
  free(netmask);
  return rc;
}

RIB_ret_code_t command_update(RIB* rtab, char* argv) {
  const int args = 6;
  char* arg = strtok(argv, " ");
  //Define data
  char* destination = NULL;
  char* netmask = NULL;
  char* newNetmask = NULL;
  char* gateway = NULL;
  char* iface = NULL;
  int metric = 0;
  int argIndex;
  for (argIndex = 0; arg != NULL; argIndex++) {
    switch (argIndex) {
      case 0: {
        destination = (char*) malloc(sizeof(char) * (strlen(arg) + 1));
        strcpy(destination, arg);
        break;
      }
      case 1: {
        netmask = (char*) malloc(sizeof(char) * (strlen(arg) + 1));
        strcpy(netmask, arg);
        break;
      }
      case 2: {
        newNetmask = (char*) malloc(sizeof(char) * (strlen(arg) + 1));
        strcpy(newNetmask, arg);
        break;
      }
      case 3: {
        gateway = (char*) malloc(sizeof(char) * (strlen(arg) + 1));
        strcpy(gateway, arg);
        break;
      }
      case 4: {
        iface = (char*) malloc(sizeof(char) * (strlen(arg) + 1));
        strcpy(iface, arg);
        break;
      }
      case 5: {
        metric = atoi(arg);
        break;
      }
    }
    arg = strtok(NULL, " ");
  }
  if (argIndex < args) {
    printf("%s\n", USAGE_ADD);
  }
  //Abort in case there are missing arguments
  if (destination == NULL) {
    return RIB_INVALID_ADDRESS;
  }
  if (netmask == NULL) {
    free(destination);
    return RIB_INVALID_ADDRESS;
  }
  if (newNetmask == NULL) {
    free(destination);
    free(netmask);
    return RIB_INVALID_ADDRESS;
  }
  if (gateway == NULL) {
    free(destination);
    free(netmask);
    free(newNetmask);
    return RIB_INVALID_ADDRESS;
  }
  if (iface == NULL) {
    free(destination);
    free(netmask);
    free(newNetmask);
    free(iface);
    return RIB_INVALID_ADDRESS;
  }
  //Add route
  RIB_ret_code_t rc = RIB_update(rtab, destination, netmask, newNetmask, gateway, iface, metric);
  free(destination);
  free(netmask);
  free(newNetmask);
  free(gateway);
  free(iface);
  return rc;
}

RIB_ret_code_t command_clear(RIB* rtab, char* argv) {
  return RIB_clear(rtab);
}

RIB_ret_code_t command_select(RIB* rtab, char* argv) {
  char* destination = NULL;
  char* netmask = NULL;
  char* arg = strtok(argv, " ");
  const int args = 2;
  int argIndex;
  for (argIndex = 0; arg != NULL; argIndex++) {
    switch (argIndex) {
      case 0: {
        destination = (char*) malloc(sizeof(char) * (strlen(arg) + 1));
        strcpy(destination, arg);
        break;
      }
      case 1: {
        netmask = (char*) malloc(sizeof(char) * (strlen(arg) + 1));
        strcpy(netmask, arg);
        break;
      }
    }
    arg = strtok(NULL, " ");
  }
  if (argIndex < args) {
    printf("%s\n", USAGE_SLT);
  }
  if (destination == NULL) {
    return RIB_INVALID_ADDRESS;
  }
  if (netmask == NULL) {
    free(destination);
    return RIB_INVALID_ADDRESS;
  }
  Route* result = NULL;
  RIB_ret_code_t rc = RIB_find(rtab, destination, netmask, &result);
  if (rc == RIB_NO_ERROR) {
    printRoute(result);
  }
  free(destination);
  free(netmask);
  return rc;
}

RIB_ret_code_t  command_route(RIB* rtab, char* argv) {
  char* destination = argv;
  if (destination == NULL) {
    printf("%s\n", USAGE_DEL);
  }
  Route* result = NULL;
  RIB_ret_code_t rc = RIB_match(rtab, destination, &result);
  if (rc == RIB_NO_ERROR) {
    printRoute(result);
  }
  return rc;
}

RIB_ret_code_t  command_dump(RIB* rtab, char* argv) {
  printf("Destination\tNetmask\t\tGateway\t\tIface\tMetric\n");
  for (int i = 0; i < rtab->entries; i++) {
    printRoute(rtab->routes[i]);
  }
  return RIB_NO_ERROR;
}

/**
 * @function parseRoutingTable
 * @description parse routing table file and store its entries to the passed RIB
 * @param RIB*
 * @param char*
 * @returns int
 */

int parseRoutingTable(RIB* rtab, char* filename) {
  //Read routing table file
  FILE* filePtr;
  filePtr = fopen(filename, "r");
  if (!filePtr) {
    printf("Could not open file %s\n", filename);
    return 0;
  }
  
  char line[256];
  //Read file line by line
  while (fgets(line, sizeof(line), filePtr)) {
    size_t len = strlen(line);
    //remove CRLF
    if (line[len - 1] == 0x0a) {
      line[len - 1] = 0x00;
    }
    if (line[len - 2] == 0x0d) {
      line[len - 2] = 0x00;
    }
    //Add to routing table
    RIB_ret_code_t rc;
    printf("ADD %s\n", line);
    if ((rc = command_add(rtab, line) != RIB_NO_ERROR)) {
      printf("ERROR: %s (%d)", line, rc);
    }
  }
  fclose(filePtr);

  return 0;
}

/**
 * @function commitRoutingTable
 * @description commit routing table changes to file
 * @param RIB*
 * @param char*
 * @returns int
 */

int commitRoutingTable(RIB* rtab, char* filename) {
  if (rtab == NULL) {
    return 1;
  }
  FILE* filePtr;
  filePtr = fopen(filename, "w");
  if (!filePtr) {
    printf("Could not open file %s\n", filename);
    return 1;
  }
  for (int i = 0; i < rtab->entries; i++) {
    Route* currRoute = rtab->routes[i];
    char line[256];
    sprintf(line, "%s %s %s %s %d\n", currRoute->destination, currRoute->netmask, currRoute->gateway, currRoute->iface, currRoute->metric);
    fwrite(&line, sizeof(char), strlen(line), filePtr);
  }
  fclose(filePtr);
  return 0;
}

int main(int argc, char* argv[]) {

  //Get command
  if (argc < 2) {
    printf("%s\n", USAGE);
    return 1;
  }
  //Initialize routing table
  RIB* rtab = NULL;
  RIB_ret_code_t rc = RIB_init(&rtab);
  if (rc != RIB_NO_ERROR) {
    printf("COULD NOT INITIALIZE RIB!\n");
    return rc;
  }
  //Parse routing table
  char* routingTableFile = argv[1];
  if (parseRoutingTable(rtab, routingTableFile) != 0) {
    printf("COULD NOT PARSE ROUTING TABLE!\n");
    RIB_free(rtab);
    return 1;
  }

  int quitCalled = 0;

  while (!quitCalled) {
    size_t bufSize = 255;
    char* inputLine = (char*) malloc(sizeof(char) * bufSize);
    char* origInputLine = inputLine;
    printf("> ");
    size_t len = getline(&inputLine, &bufSize, stdin);
    //remove CRLF
    if (inputLine[len - 1] == 0x0a) {
      inputLine[len - 1] = 0x00;
    }
    if (inputLine[len - 2] == 0x0d) {
      inputLine[len - 2] = 0x00;
    }
    route_cmd_t command;
    if (strchr(inputLine, ' ') == NULL) {
      command = getCommand(inputLine);
    } else {
      char* commandStr = strtok(inputLine, " ");
      inputLine = strtok(NULL, "");
      command = getCommand(commandStr); 
    }
    switch (command) {
      case UNKNOWN:
      case HELP: {
        usage();
        break;
      }
      case QUIT: {
        quitCalled = 1;
        printf("CLOSING RIB...\n");
        break;
      }
      case ADD: {
        RIB_ret_code_t ret;
        if ((ret = command_add(rtab, inputLine)) != RIB_NO_ERROR) {
          printf("COMMAND ERROR: %d\n", ret);
        } else {
          printf("OK\n");
        }
        break;
      }
      case DELETE: {
        RIB_ret_code_t ret;
        if ((ret = command_delete(rtab, inputLine)) != RIB_NO_ERROR) {
          printf("COMMAND ERROR: %d\n", ret);
        } else {
          printf("OK\n");
        }
        break;
      }
      case UPDATE: {
        RIB_ret_code_t ret;
        if ((ret = command_update(rtab, inputLine)) != RIB_NO_ERROR) {
          printf("COMMAND ERROR: %d\n", ret);
        } else {
          printf("OK\n");
        }
        break;
      }
      case CLEAR: {
        RIB_ret_code_t ret;
        if ((ret = command_clear(rtab, inputLine)) != RIB_NO_ERROR) {
          printf("COMMAND ERROR: %d\n", ret);
        } else {
          printf("OK\n");
        }
        break;
      }
      case SELECT: {
        RIB_ret_code_t ret;
        if ((ret = command_select(rtab, inputLine)) != RIB_NO_ERROR) {
          printf("COMMAND ERROR: %d\n", ret);
        } else {
          printf("OK\n");
        }
        break;
      }
      case ROUTE: {
        RIB_ret_code_t ret;
        if ((ret = command_route(rtab, inputLine)) != RIB_NO_ERROR) {
          printf("COMMAND ERROR: %d\n", ret);
        } else {
          printf("OK\n");
        }
        break;
      }
      case DUMP: {
        RIB_ret_code_t ret;
        if ((ret = command_dump(rtab, inputLine)) != RIB_NO_ERROR) {
          printf("COMMAND ERROR: %d\n", ret);
        } else {
          printf("OK\n");
        }
        break;
      }
      case COMMIT: {
        int ret;
        if ((ret = commitRoutingTable(rtab, routingTableFile)) != 0 ) {
          printf("COMMAND ERROR: %d\n", ret);
        } else {
          printf("OK\n");
        }
        break;
      }
      case ROLLBACK: {
        RIB_free(rtab);
        rc = RIB_init(&rtab);
        if (rc != RIB_NO_ERROR) {
          printf("COMMAND ERROR: %d\n", -1);
          return rc;
        }
        if (parseRoutingTable(rtab, routingTableFile) != 0) {
          printf("COMMAND ERROR: %d\n", -1);
          RIB_free(rtab);
          return 1;
        }
        printf("OK\n");
        break;
      }
    }
    free(origInputLine);
  }

  //Commit changes
  int ret;
  if ((ret = commitRoutingTable(rtab, routingTableFile)) != 0) {
    printf("COMMIT FAILED (%d)\n", ret);
  }
  //Free RIB table
  RIB_free(rtab);
  printf("RIB CLOSED.\n");

  return 0;
}
