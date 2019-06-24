/**
 *   librib - rib.c
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

#include <rib/iputils.h>
#include <rib/rib.h>

#include <stdlib.h>
#include <string.h>

/**
 * @function RIB_init
 * @description initialize a RIB data structure; returns NULL if it fails
 * @returns RIB*
 */

RIB* RIB_init() {
  RIB* rtab = (RIB*) malloc(sizeof(RIB));
  if (rtab != NULL) {
    rtab->entries = 0;
    rtab->routes = NULL;
  }
  return rtab;
}

/**
 * @function RIB_free
 * @description free RIB data structure
 * @param RIB*
 * @returns int: 0 if free succeeded
 */

int RIB_free(RIB* rtab) {
  if (rtab == NULL) {
    return 1;
  }
  //If routing table exists delete each entry and for each entry free char pointers
  if (rtab->routes != NULL) {
    for(size_t i = 0; i < rtab->entries; i++) {
      Route* thisRoute = rtab->routes[i];
      if (thisRoute != NULL) {
        if (thisRoute->destination != NULL) {
          free(thisRoute->destination);
        }
        if (thisRoute->netmask != NULL) {
          free(thisRoute->netmask);
        }
        if (thisRoute->gateway != NULL) {
          free(thisRoute->gateway);
        }
        if (thisRoute->iface != NULL) {
          free(thisRoute->iface);
        }
        free(thisRoute);
      }
    }
    free(rtab->routes);
  }
  free(rtab);
  return 0;
}

/**
 * @function RIB_add
 * @description add new entry to the routing table
 * @param RIB* routing table
 * @param const char* destination
 * @param const char* netmask
 * @param const char* gateway
 * @param const char* iface
 * @param int metric
 * @returns int: 0 if add operation succeeded
 */

int RIB_add(RIB* rtab, const char* destination, const char* netmask, const char* gateway, const char* iface, int metric) {
  if (rtab == NULL) {
    return 1;
  }
  //Check whether provided addresses are valid and get ip version
  int ipVersion;
  if (isValidIpAddress(destination, &ipVersion) != 0) {
    return 1;
  }
  if (isValidIpAddress(netmask, NULL) != 0) {
    return 1;
  }
  if (isValidIpAddress(gateway, NULL) != 0) {
    return 1;
  }
  if (ipVersion != 4 && ipVersion != 6) {
    return 1;
  }
  //check if an entry for provided destination already exists
  for (size_t i = 0; i < rtab->entries; i++) {
    Route* thisRoute = rtab->routes[i];
    if (compareIPv4Addresses(thisRoute->destination, destination) == 0) {
      return 1;
    }
  }
  //Allocate new route struct
  Route* newRoute = (Route*) malloc(sizeof(Route));
  //Copy to new route struct the attributes passed as arguments
  newRoute->destination = (char*) malloc(sizeof(char) * (strlen(destination) + 1));
  strcpy(newRoute->destination, destination);
  newRoute->netmask = (char*) malloc(sizeof(char) * (strlen(netmask) + 1));
  strcpy(newRoute->netmask, netmask);
  newRoute->gateway = (char*) malloc(sizeof(char) * (strlen(gateway) + 1));
  strcpy(newRoute->gateway, gateway);
  newRoute->iface = (char*) malloc(sizeof(char) * (strlen(iface) + 1));
  strcpy(newRoute->iface, iface);
  newRoute->metric = metric;
  newRoute->ipv = ipVersion;
  //Format addresses
  if (newRoute->ipv == 4) {
    formatIPv4Address(&newRoute->destination);
    formatIPv4Address(&newRoute->netmask);
    formatIPv4Address(&newRoute->gateway);
  }
  //Increment entries
  rtab->entries++;
  //Allocate new route and store it into routing table
  rtab->routes = (Route**) realloc(rtab->routes, sizeof(Route*) * rtab->entries);
  rtab->routes[rtab->entries - 1] = newRoute;
  return 0;
}

/**
 * @function RIB_delete
 * @description delete an entry from the routing table
 * @param RIB*
 * @param const char* destination to remove
 * @returns int: 0 if succeeded
 */

int RIB_delete(RIB* rtab, const char* destination) {
  if (rtab == NULL) {
    return 1;
  }
  if (rtab->routes == NULL) {
    return 1;
  }
  //Iterate over routing table to find the destination to remove
  for (size_t i = 0; i < rtab->entries; i++) {
    Route* thisRoute = rtab->routes[i];
    if (compareIPv4Addresses(thisRoute->destination, destination) == 0) {
      //We found it!
      size_t currIndex = i;
      //Delete element
      if (thisRoute != NULL) {
        if (thisRoute->destination != NULL) {
          free(thisRoute->destination);
        }
        if (thisRoute->netmask != NULL) {
          free(thisRoute->netmask);
        }
        if (thisRoute->gateway != NULL) {
          free(thisRoute->gateway);
        }
        if (thisRoute->iface != NULL) {
          free(thisRoute->iface);
        }
        free(thisRoute);
      }
      //Decrement entries
      rtab->entries--;
      //Now we need to shift all elements after the current one by one position back
      for (size_t j = currIndex; j < rtab->entries; j++) {
        rtab->routes[j] = rtab->routes[j + 1];
      }
      //Reallocate routes
      rtab->routes = (Route**) realloc(rtab->routes, sizeof(Route*) * rtab->entries);
    }
  }
  //Destination not found :(
  return 1;
}

/**
 * @function RIB_find
 * @description find a Route with provided network address in provided route table
 * @param RIB*
 * @param const char*
 * @returns Route*
 */

Route* RIB_find(RIB* rtab, const char* networkAddr) {
  if (rtab == NULL) {
    return NULL;
  }
  if (rtab->routes == NULL) {
    return NULL;
  }
  for (size_t i = 0; i < rtab->entries; i++) {
    Route* thisRoute = rtab->routes[i];
    if (compareIPv4Addresses(thisRoute->destination, networkAddr) == 0) {
      return thisRoute;
    }
  }
  return NULL;
}

/**
 * @function RIB_match
 * @description find a matching route for the provided destination address; Longest prefix match is used in case of ambiguity; supports both ipv4 and ipv6
 * @param RIB*
 * @param const char* destination
 * @returns Route*, NULL if none is found
 */

Route* RIB_match(RIB* rtab, const char* destination) {
  int ipVersion;
  //Check whether destination is valid
  if (isValidIpAddress(destination, &ipVersion) != 0) {
    return NULL;
  }
  //Find match based on ip version
  if (ipVersion == 4) {
    return RIB_match_ipv4(rtab, destination);
  } else if (ipVersion == 6) {
    return RIB_match_ipv6(rtab, destination);
  } else {
    return NULL;
  }
}

/**
 * @function RIB_match_ipv4
 * @description find a matching route for the provided ipv4 destination address; Longest prefix match is used in case of ambiguity
 * @param RIB*
 * @param const char* destination
 * @returns Route*, NULL if none is found
 */

Route* RIB_match_ipv4(RIB* rtab, const char* destination) {
  //Iterate over routing table in order to find a suitable route
  if (rtab == NULL) {
    return NULL;
  }
  if (rtab->routes == NULL) {
    return NULL;
  }
  Route* nextHop = NULL;
  //Iterate over routing table
  for (size_t i = 0; i < rtab->entries; i++) {
    Route* thisRoute = rtab->routes[i];
    //Find the network address with the provided address and check if they match
    char* thisNetaddr = getIpv4NetworkAddress(destination, thisRoute->netmask);
    //Check if they match
    if (compareIPv4Addresses(thisNetaddr, thisRoute->destination) == 0) {
      //They match! 
      if (nextHop == NULL) {
        //Set immediately this route as next hop
        nextHop = thisRoute;
      } else {
        //Else check check Longest prefix match
        if (getCIDRnetmask(nextHop->netmask) < getCIDRnetmask(thisRoute->netmask)) {
          nextHop = thisRoute;
        }
      }
    }
  }
  //check also 0.0.0.0 as destination if nextHop is NULL
  if (nextHop == NULL) {
    const char default_address[8] = "0.0.0.0";
    nextHop = RIB_find(rtab, &default_address);
  }

  return nextHop;
}
