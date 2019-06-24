/**
 *   librib - iputils.c
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

#include <netdb.h>
#include <string.h>

/**
 * @function isValidIpAddress
 * @description returns whether the provided ip address is valid and its ip version
 * @param char* ipAddr
 * @param int* ipversion
 * @returns int: 0 if valid
 */

int isValidIpAddress(const char *ipAddr, int* ipv) {
  if (ipAddr == NULL) {
    return 1;
  }
  int ipVersion = 0;
  struct addrinfo hint, *res = NULL;
  memset(&hint, '\0', sizeof hint);
  hint.ai_family = PF_UNSPEC;
  hint.ai_flags = AI_NUMERICHOST;
  int ret = getaddrinfo(ipAddr, NULL, &hint, &res);
  if (ret) {
    return 1;
  }
  if(res->ai_family == AF_INET) {
    ipVersion = 4;
  } else if (res->ai_family == AF_INET6) {
    ipVersion = 6;
  } else {
    return 1;
  }
  freeaddrinfo(res);
  if (ipv != NULL) {
    *ipv = ipVersion;
  }
  return 0;
}

/**
 * @function getCIDRnetmask
 * @description returns the netmask CIDR notation from netmask address (e.g. 255.255.255.0 => 24)
 * @param char*
 * @returns int
 */

int getCIDRnetmask(const char *netmask) {
  //Copy tmp copy for tokenization
  size_t netmaskLen = strlen(netmask);
  char* tmpNetmask = (char*) malloc(sizeof(char) * (netmaskLen + 1));
  memcpy(tmpNetmask, netmask, netmaskLen);
  tmpNetmask[netmaskLen] = 0x00;
  char* netmaskToken = strtok(tmpNetmask, ".");
  int ipbytes[4];
  int cidrNetmask = 0;
  size_t i = 0;
  while (netmaskToken) {
    ipbytes[i++] = atoi(netmaskToken);
    netmaskToken = strtok(NULL, ".");
  }
  free(tmpNetmask);
  for (int i = 0; i < 4; i++) {
    switch (ipbytes[i]) {
    case 0x80:
      cidrNetmask += 1;
      break;

    case 0xC0:
      cidrNetmask += 2;
      break;

    case 0xE0:
      cidrNetmask += 3;
      break;

    case 0xF0:
      cidrNetmask += 4;
      break;

    case 0xF8:
      cidrNetmask += 5;
      break;

    case 0xFC:
      cidrNetmask += 6;
      break;

    case 0xFE:
      cidrNetmask += 7;
      break;

    case 0xFF:
      cidrNetmask += 8;
      break;

    default:
      return cidrNetmask;
      break;
    }
  }
  return cidrNetmask;
}

/**
 * @function getIpv4NetworkAddress
 * @description returns the network address from a provided ip address and a netmask
 * @param char*
 * @param char*
 * @returns char*
 */

char* getIpv4NetworkAddress(const char* ipAddress, const char* netmask) {
  char* networkAddress = NULL;
  //Create tmp copy for tokenization
  size_t ipLen = strlen(ipAddress);
  char* tmpAddr = (char*) malloc(sizeof(char) * (ipLen + 1));
  memcpy(tmpAddr, ipAddress, ipLen);
  tmpAddr[ipLen] = 0x00;
  size_t netmaskLen = strlen(netmask);
  char* tmpNetmask = (char*) malloc(sizeof(char) * (netmaskLen + 1));
  memcpy(tmpNetmask, netmask, netmaskLen);
  tmpNetmask[netmaskLen] = 0x00;
  //Get ip address tokens
  int ipBytes[4];
  char* ipToken = strtok(tmpAddr, ".");
  size_t i = 0;
  while (ipToken) {
    ipBytes[i++] = atoi(ipToken);
    ipToken = strtok(NULL, ".");
  }
  //Get netmask tokens
  int netmaskBytes[4];
  char* netmaskToken = strtok(tmpNetmask, ".");
  i = 0;
  while (ipToken) {
    netmaskBytes[i++] = atoi(netmaskToken);
    netmaskToken = strtok(NULL, ".");
  }
  //Free tokens
  free(tmpAddr);
  free(tmpNetmask);
  //Calc network address (ipAddr AND netmask)
  int networkAddrBytes[4];
  for (size_t i = 0; i < 4; i++) {
    networkAddrBytes[i] = ipBytes[i] & netmaskBytes[i];
  }
  const int maxAddrLength = 17;
  networkAddress = (char*) malloc(sizeof(char) * maxAddrLength);
  memset(networkAddress, 0x00, maxAddrLength);
  sprintf(networkAddress, "%d.%d.%d.%d", networkAddrBytes[0], networkAddrBytes[1], networkAddrBytes[2], networkAddrBytes[3]);
  return networkAddress;
}

/**
 * @function formatIPv4Address
 * @description format an address to a correct ipv4 format (e.g. 010.008.000.001 => 10.8.0.1)
 * @param char**
 */

void formatIPv4Address(char** ipAddress) {
  size_t newAddrSize = 3; //3 dots
  int ipBytes[4];
  char* ipToken = strtok(*ipAddress, ".");
  for (int i = 0; ipToken && i < 4; i++) {
    ipBytes[i] = atoi(ipToken);
    if (ipBytes[i] < 10) {
      newAddrSize++;
    } else if (ipBytes[i] < 100) {
      newAddrSize += 2;
    } else {
      newAddrSize += 3;
    }
    ipToken = strtok(NULL, ".");
  }
  *ipAddress = (char*) realloc(*ipAddress, sizeof(char) * (newAddrSize + 1));
  sprintf(*ipAddress, "%d.%d.%d.%d", ipBytes[0], ipBytes[1], ipBytes[2], ipBytes[3]);
}

/**
 * @function compareIPv4Addresses
 * @description compare two ipv4 addresses and returns if they're equal or not
 * @param char*
 * @param char*
 * @returns int: 0 if they're equal
 */

int compareIPv4Addresses(const char* ipAddress, const char* cmpIpAddress) {
  //Allocate temporary ip address to compare them
  char* ipAddr1 = (char*) malloc(sizeof(char) * (strlen(ipAddress) + 1));
  strcpy(ipAddr1, ipAddress);
  char* ipAddr2 = (char*) malloc(sizeof(char) * (strlen(cmpIpAddress) + 1));
  strcpy(ipAddr2, cmpIpAddress);
  //Format them
  formatIPv4Address(&ipAddr1);
  formatIPv4Address(&ipAddr2);
  //Check if they're equal
  int ret = strcmp(ipAddr1, ipAddr2);
  //Free previously allocated temporary addresses
  free(ipAddr1);
  free(ipAddr2);
  return ret;
}
