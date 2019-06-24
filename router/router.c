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
#define USAGE_DEL "DELETE <networkAddr> - delete a record in the routing table"
#define USAGE_UPD "UPDATE <networkAddr> <newNetmask> <newGateway> <newIface> <newMetric> - update a record in the routing table"
#define USAGE_CLR "CLEAR - clear routing table"
#define USAGE_SLT "SELECT <networkAddr> - retrieve routing information for a network address"
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

  printf("%s\n", USAGE);
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

/**
 * @function parseRoutingTable
 * @description parse routing table file and store its entries to the passed RIB
 * @param RIB*
 * @param char*
 * @returns int
 */

int parseRoutingTable(RIB* rtab, char* filename) {
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
  return 0;
}

RIB_ret_code_t addCommand(RIB* rtab, char* argv) {
  const int args = 5;
  char* arg = strtok(argv, " ");
  //Define data
  char* destination = NULL;
  char* netmask = NULL;
  char* gateway = NULL;
  char* iface = NULL;
  int metric = 0;
  for (int argIndex = 0; arg != NULL; argIndex++) {
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
    free(iface);
    return RIB_INVALID_ADDRESS;
  }
  //Add route
  return RIB_add(rtab, destination, netmask, gateway, iface, metric);
}

int main(int argc, char* argv[]) {

  //Get command
  if (argc < 2) {
    usage();
    return 1;
  }
  //Initialize routing table
  RIB* rtab;
  RIB_ret_code_t rc = RIB_init(rtab);
  if (rc != RIB_NO_ERROR) {
    printf("Could not initialize RIB\n");
    return rc;
  }
  //Parse routing table
  char* routingTableFile = argv[1];
  if (parseRoutingTable(rtab, routingTableFile) != 0) {
    printf("Could not parse routing table!\n");
    RIB_free(rtab);
    return 1;
  }

  int quitCalled = 0;

  while (!quitCalled) {
    char inputLine[255];
    scanf("> %s", &inputLine);
    char* commandStr = strtok(inputLine, " ");
    route_cmd_t command = getCommand(commandStr); 
    switch (command) {
      case UNKNOWN:
      case HELP: {
        usage();
        break;
      }
      case QUIT: {
        quitCalled = 1;
        printf("Closing RIB...\n");
        break;
      }
      case ADD: {
        RIB_ret_code_t ret;
        if ((ret = addCommand(rtab, inputLine)) != RIB_NO_ERROR) {
          printf("COMMAND ERROR: %d\n", ret);
        } else {
          printf("OK\n");
        }
        break;
      }
      case DELETE: {
        break;
      }
      case UPDATE: {
        break;
      }
      case CLEAR: {
        break;
      }
      case SELECT: {
        break;
      }
      case ROUTE: {
        break;
      }
      case DUMP: {
        break;
      }
      case COMMIT: {
        int ret;
        if ((ret = commitRoutingTable(rtab, routingTableFile)) != 0 ) {
          printf("Could not commit changes to routing table (%d)\n", ret);
        }
        break;
      }
      case ROLLBACK: {
        RIB_free(rtab);
        rc = RIB_init(rtab);
        if (rc != RIB_NO_ERROR) {
          printf("Could not re-initialize RIB\n");
          return rc;
        }
        if (parseRoutingTable(rtab, routingTableFile) != 0) {
          printf("Could not parse routing table!\n");
          RIB_free(rtab);
          return 1;
        }
        printf("Routing table rollbacked!\n");
        break;
      }
    }

  }

  //Commit changes
  int ret;
  if ((ret = commitRoutingTable(rtab, routingTableFile)) != 0) {
    printf("Could not commit changes to routing table (%d)\n", ret);
  }
  //Free RIB table
  RIB_free(rtab);
  printf("RIB closed.\n");

  return 0;
}