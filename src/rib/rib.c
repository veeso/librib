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
 * @param RIB* rtab
 * @returns RIB_ret_code_t
 */

RIB_ret_code_t RIB_init(RIB** rtab) {
  *rtab = (RIB*) malloc(sizeof(RIB));
  if (rtab != NULL) {
    (*rtab)->entries = 0;
    (*rtab)->routes = NULL;
    return RIB_NO_ERROR;
  } else {
    return RIB_BAD_ALLOC;
  }
}

/**
 * @function RIB_free
 * @description free RIB data structure
 * @param RIB*
 * @returns RIB_ret_code_t: 0 if free succeeded
 */

RIB_ret_code_t RIB_free(RIB* rtab) {
  if (rtab == NULL) {
    return RIB_UNINITIALIZED_RIB;
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
  return RIB_NO_ERROR;
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
 * @returns RIB_ret_code_t: 0 if add operation succeeded
 */

RIB_ret_code_t RIB_add(RIB* rtab, const char* destination, const char* netmask, const char* gateway, const char* iface, int metric) {
  if (rtab == NULL) {
    return RIB_UNINITIALIZED_RIB;
  }
  //Check whether provided addresses are valid and get ip version
  int ipVersion;
  if (isValidIpAddress(destination, &ipVersion) != 0) {
    return RIB_INVALID_ADDRESS;
  }
  if (isValidIpAddress(netmask, NULL) != 0) {
    return RIB_INVALID_ADDRESS;
  }
  if (isValidIpAddress(gateway, NULL) != 0) {
    return RIB_INVALID_ADDRESS;
  }
  if (ipVersion != 4 && ipVersion != 6) {
    return RIB_INVALID_ADDRESS;
  }
  //check if an entry for provided destination already exists
  for (size_t i = 0; i < rtab->entries; i++) {
    Route* thisRoute = rtab->routes[i];
    if (compareIPv4Addresses(thisRoute->destination, destination) == 0 && compareIPv4Addresses(thisRoute->netmask, netmask) == 0) {
      return RIB_INVALID_ADDRESS;
    }
  }
  //Allocate new route struct
  Route* newRoute = (Route*) malloc(sizeof(Route));
  //Allocate space for the new record
  newRoute->destination = (char*) malloc(sizeof(char) * (strlen(destination) + 1));
  if (newRoute->destination == NULL) {
    free(newRoute);
    return RIB_BAD_ALLOC;
  }
  newRoute->netmask = (char*) malloc(sizeof(char) * (strlen(netmask) + 1));
  if (newRoute->netmask == NULL) {
    free(newRoute->destination);
    free(newRoute);
    return RIB_BAD_ALLOC;
  }
  newRoute->gateway = (char*) malloc(sizeof(char) * (strlen(gateway) + 1));
  if (newRoute->netmask == NULL) {
    free(newRoute->destination);
    free(newRoute->netmask);
    free(newRoute);
    return RIB_BAD_ALLOC;
  }
  newRoute->iface = (char*) malloc(sizeof(char) * (strlen(iface) + 1));
  if (newRoute->iface == NULL) {
    free(newRoute->destination);
    free(newRoute->netmask);
    free(newRoute->gateway);
    free(newRoute);
    return RIB_BAD_ALLOC;
  }
  //Copy to new route struct the attributes passed as arguments
  strcpy(newRoute->destination, destination);
  strcpy(newRoute->netmask, netmask);
  strcpy(newRoute->gateway, gateway);
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
  if (rtab->routes == NULL) {
    free(newRoute);
    return RIB_BAD_ALLOC;
  }
  rtab->routes[rtab->entries - 1] = newRoute;
  return RIB_NO_ERROR;
}

/**
 * @function RIB_delete
 * @description delete an entry from the routing table
 * @param RIB*
 * @param const char* destination to remove
 * @returns RIB_ret_code_t: 0 if succeeded
 */

RIB_ret_code_t RIB_delete(RIB* rtab, const char* destination, const char* netmask) {
  if (rtab == NULL) {
    return RIB_UNINITIALIZED_RIB;
  }
  if (rtab->routes == NULL) {
    return RIB_NOT_EXISTS;
  }
  //Iterate over routing table to find the destination to remove
  for (size_t i = 0; i < rtab->entries; i++) {
    Route* thisRoute = rtab->routes[i];
    if (compareIPv4Addresses(thisRoute->destination, destination) == 0 && (compareIPv4Addresses(thisRoute->netmask, netmask) == 0 || strcmp(netmask, "*") == 0)) {
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
      if (rtab->routes == NULL && rtab->entries > 0) {
        return RIB_BAD_ALLOC;
      }
      return RIB_NO_ERROR;
    }
  }
  //Destination not found :(
  return RIB_NOT_EXISTS;
}

/**
 * @function RIB_update
 * @description update a routing table entry
 * @param RIB* rtab
 * @param const char*
 * @param const char*
 * @param const char*
 * @param const char*
 * @param const char*
 * @param int
 * @returns RIB_ret_code_t
 */

RIB_ret_code_t RIB_update(RIB* rtab, const char* destination, const char* netmask, const char* newNetmask, const char* newGateway, const char* newIface, int newMetric) {
  if (rtab == NULL) {
    return RIB_UNINITIALIZED_RIB;
  }
  if (rtab->routes == NULL) {
    return RIB_NOT_EXISTS;
  }
  //Check if passed arguments are valid
  int ipVersion;
  if (isValidIpAddress(newNetmask, NULL) != 0) {
    return RIB_INVALID_ADDRESS;
  }
  if (isValidIpAddress(newGateway, &ipVersion) != 0) {
    return RIB_INVALID_ADDRESS;
  }
  //Iterate over routing table to find the destination to update
  for (size_t i = 0; i < rtab->entries; i++) {
    Route* thisRoute = rtab->routes[i];
    if (compareIPv4Addresses(thisRoute->destination, destination) == 0 && compareIPv4Addresses(thisRoute->netmask, netmask) == 0) {
      //We found it
      //Check if ip versions match
      if (thisRoute->ipv != ipVersion) {
        return RIB_INVALID_ADDRESS;
      }
      //Allocate space for the new record
      thisRoute->netmask = (char*) realloc(thisRoute->netmask, sizeof(char) * (strlen(newNetmask) + 1));
      if (thisRoute->netmask == NULL) {
        free(thisRoute->destination);
        free(thisRoute->gateway);
        free(thisRoute->iface);
        free(thisRoute);
        return RIB_BAD_ALLOC;
      }
      thisRoute->gateway = (char*) realloc(thisRoute->gateway, sizeof(char) * (strlen(newGateway) + 1));
      if (thisRoute->netmask == NULL) {
        free(thisRoute->destination);
        free(thisRoute->netmask);
        free(thisRoute->iface);
        free(thisRoute);
        return RIB_BAD_ALLOC;
      }
      thisRoute->iface = (char*) realloc(thisRoute->iface, sizeof(char) * (strlen(newIface) + 1));
      if (thisRoute->iface == NULL) {
        free(thisRoute->destination);
        free(thisRoute->netmask);
        free(thisRoute->gateway);
        free(thisRoute);
        return RIB_BAD_ALLOC;
      }
      thisRoute->metric = newMetric;
      thisRoute->ipv = ipVersion;
      //Copy to new route struct the attributes passed as arguments
      strcpy(thisRoute->netmask, newNetmask);
      strcpy(thisRoute->gateway, newGateway);
      strcpy(thisRoute->iface, newIface);
      //Format addresses
      if (thisRoute->ipv == 4) {
        formatIPv4Address(&thisRoute->netmask);
        formatIPv4Address(&thisRoute->gateway);
      }
      return RIB_NO_ERROR;
    }
  }
  return RIB_NOT_EXISTS;
}

/**
 * @function RIB_clear
 * @description clear Routing table entries
 * @param RIB*
 * @returns RIB_ret_code_t
 */

RIB_ret_code_t RIB_clear(RIB* rtab) {
  if (rtab == NULL) {
    return RIB_UNINITIALIZED_RIB;
  }
  if (rtab->routes == NULL) {
    return RIB_NO_ERROR;
  }
  //Delete each entry of the routing table
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
  rtab->routes = NULL;
  rtab->entries = 0;
  return RIB_NO_ERROR;
}

/**
 * @function RIB_find
 * @description find a Route with provided network address in provided route table
 * @param RIB*
 * @param const char*
 * @param Route* found route
 * @returns RIB_ret_code_t
 */

RIB_ret_code_t RIB_find(RIB* rtab, const char* networkAddr, const char* netmask, Route** route) {
  if (rtab == NULL) {
    return RIB_UNINITIALIZED_RIB;
  }
  if (rtab->routes == NULL) {
    return RIB_NO_MATCH;
  }
  for (size_t i = 0; i < rtab->entries; i++) {
    Route* thisRoute = rtab->routes[i];
    if (compareIPv4Addresses(thisRoute->destination, networkAddr) == 0 && (compareIPv4Addresses(thisRoute->netmask, netmask) == 0 || strcmp(netmask, "*") == 0)) {
      *route = thisRoute;
      return RIB_NO_ERROR;
    }
  }
  return RIB_NO_MATCH;
}

/**
 * @function RIB_match
 * @description find a matching route for the provided destination address; Longest prefix match is used in case of ambiguity; supports both ipv4 and ipv6
 * @param RIB*
 * @param const char* destination
 * @param Route* matched route; NULL if not found
 * @returns RIB_ret_code_t
 */

RIB_ret_code_t RIB_match(RIB* rtab, const char* destination, Route** route) {
  int ipVersion;
  //Check whether destination is valid
  if (isValidIpAddress(destination, &ipVersion) != 0) {
    return RIB_INVALID_ADDRESS;
  }
  //Find match based on ip version
  if (ipVersion == 4) {
    return RIB_match_ipv4(rtab, destination, route);
  } else if (ipVersion == 6) {
    return RIB_match_ipv6(rtab, destination, route);
  } else {
    return RIB_INVALID_ADDRESS;
  }
}

/**
 * @function RIB_match_ipv4
 * @description find a matching route for the provided ipv4 destination address; Longest prefix match is used in case of ambiguity
 * @param RIB*
 * @param const char* destination
 * @param Route* matched route; NULL if not found
 * @returns RIB_ret_code_t
 */

RIB_ret_code_t RIB_match_ipv4(RIB* rtab, const char* destination, Route** route) {
  //Iterate over routing table in order to find a suitable route
  if (rtab == NULL) {
    return RIB_UNINITIALIZED_RIB;
  }
  if (rtab->routes == NULL) {
    return RIB_NO_MATCH;
  }
  *route = NULL;
  //Iterate over routing table
  for (size_t i = 0; i < rtab->entries; i++) {
    Route* thisRoute = rtab->routes[i];
    //Find the network address with the provided address and check if they match
    char* thisNetaddr = getIpv4NetworkAddress(destination, thisRoute->netmask);
    //Check if they match
    if (compareIPv4Addresses(thisNetaddr, thisRoute->destination) == 0) {
      //They match! 
      if (*route == NULL) {
        //Set immediately this route as next hop
        *route = thisRoute;
      } else {
        //Else check check Longest prefix match
        if (getCIDRnetmask((*route)->netmask) < getCIDRnetmask(thisRoute->netmask)) {
          *route = thisRoute;
        }
      }
    }
  }
  //check also 0.0.0.0 as destination if route is NULL
  if (*route == NULL) {
    const char default_address[8] = "0.0.0.0";
    RIB_find(rtab, default_address, default_address, route);
  }

  if (*route == NULL) {
    return RIB_NO_MATCH;
  }

  return RIB_NO_ERROR;
}

/**
 * @function RIB_match_ipv6
 * @description find a matching route for the provided ipv6 destination address; Longest prefix match is used in case of ambiguity
 * @param RIB*
 * @param const char* destination
 * @param Route* matched route; NULL if not found
 * @returns RIB_ret_code_t
 */

RIB_ret_code_t RIB_match_ipv6(RIB* rtab, const char* destination, Route** route) {
  return RIB_NO_MATCH;
}