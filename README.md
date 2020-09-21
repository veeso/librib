# LibRIB

[![License: MIT](https://img.shields.io/badge/License-MIT-teal.svg)](https://opensource.org/licenses/MIT) [![Stars](https://img.shields.io/github/stars/ChristianVisintin/librib.svg)](https://github.com/ChristianVisintin/librib) [![Issues](https://img.shields.io/github/issues/ChristianVisintin/librib.svg)](https://github.com/ChristianVisintin/librib/issues) [![Build](https://api.travis-ci.org/ChristianVisintin/librib.svg?branch=master)](https://travis-ci.org/ChristianVisintin/librib)

Developed by *Christian Visintin*

Current Version 1.0.1 (21/09/2020)

- [LibRIB](#librib)
  - [Build](#build)
    - [CMake](#cmake)
    - [Autotools](#autotools)
  - [Documentation](#documentation)
    - [RIB](#rib)
      - [RIB struct](#rib-struct)
      - [Route struct](#route-struct)
      - [Return codes](#return-codes)
      - [RIB_init](#rib_init)
      - [RIB_free](#rib_free)
      - [RIB_add](#rib_add)
      - [RIB_delete](#rib_delete)
      - [RIB_update](#rib_update)
      - [RIB_clear](#rib_clear)
      - [RIB_find](#rib_find)
      - [RIB_match](#rib_match)
  - [Known Issues](#known-issues)
  - [Changelog](#changelog)
  - [License](#license)

---

## Build

### CMake

```sh
mkdir build/
cmake -DWITH_ROUTER=yes ..
make
make install
```

### Autotools

```sh
./autogen.sh
./configure
make
make install
```

## Documentation

LibRIB is a C library which can be used to implement a routing table. It supports both IPv4 and IPv6.
This project provides both the library and a program to store and manage routing table (bin/router).

### RIB

RIB header defines all the functions to implement a routing information base.

#### RIB struct

```C
typedef struct RIB {
  Route** routes;
  size_t entries;
} RIB;
```

The RIB struct represents a routing table object, which is a wrapper for all the routes.

#### Route struct

```C
typedef struct Route {
  char* destination;
  char* netmask;    //IPv4 specific
  int prefixLength; //IPv6 specific
  char* gateway;
  char* iface;
  int metric;
  int ipv;
} Route;
```

The Route struct represents a Route object, which describes a single record for the routing table.

#### Return codes

All the functions implemented by the librib API return a RIB_ret_code_t, which is an enum with these error codes; each one describes the issue in satisfying the request.

```C
typedef enum RIB_ret_code_t {
  RIB_NO_ERROR,
  RIB_INVALID_ADDRESS,
  RIB_NO_MATCH,
  RIB_DUP_RECORD,
  RIB_NOT_EXISTS,
  RIB_UNINITIALIZED_RIB,
  RIB_BAD_ALLOC
} RIB_ret_code_t;
```

* NO_ERROR: the operation outcome was successful
* INVALID_ADDRESS: one of the addresses provided to the function is nor ipv4 nor ipv6.
* NO_MATCH: No route could satisfy the query.
* DUP_RECORD: A route with the provided addresses already exists in the routing table.
* NOT_EXISTS: A route with the provided addresses doesn't exist in the routing table.
* UNINITIALIZED_RIB: The RIB object is NULL or not correctly initialized.
* BAD_ALLOC: It was not possible to allocate memory for a RIB/Route object.

---

#### RIB_init

```C
/**
 * @function RIB_init
 * @description initialize a RIB data structure; returns NULL if it fails
 * @param RIB* rtab
 * @returns RIB_ret_code_t
 */

RIB_ret_code_t RIB_init(RIB** rtab);
```

This is the first function to call when an instance of a RIB is needed. It returns a pointer to a RIB struct with all the needed attributes initialized.

#### RIB_free

```C
/**
 * @function RIB_free
 * @description free RIB data structure
 * @param RIB*
 * @returns RIB_ret_code_t: 0 if free succeeded
 */

RIB_ret_code_t RIB_free(RIB* rtab);
```

This function has to be called to free a RIB structure when it is no more necessary.

#### RIB_add

```C
/**
 * @function RIB_add
 * @description add new entry to the routing table
 * @param RIB* routing table
 * @param const char* destination
 * @param const char* netmask (ipv4)/prefix (ipv6) char representation
 * @param const char* gateway
 * @param const char* iface
 * @param int metric
 * @returns RIB_ret_code_t: 0 if add operation succeeded
 */

RIB_ret_code_t RIB_add(RIB* rtab, const char* destination, const char* netmask, const char* gateway, const char* iface, int metric);
```

RIB_add is used to add a new route to the RIB.
It supports both IPv4 and IPv6; the netmask is in 32 bits address format for IPv4 and is prefix length in case of IPv6 (e.g. 64).

#### RIB_delete

```C
/**
 * @function RIB_delete
 * @description delete an entry from the routing table
 * @param RIB*
 * @param const char* destination to remove
 * @returns RIB_ret_code_t: 0 if succeeded
 */

RIB_ret_code_t RIB_delete(RIB* rtab, const char* destination, const char* netmask);
```

RIB_delete deletes the route in the RIB with the specified network address and netmask.

#### RIB_update

```C
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

RIB_ret_code_t RIB_update(RIB* rtab, const char* destination, const char* netmask, const char* newNetmask, const char* newGateway, const char* newIface, int newMetric);
```

RIB_update updates the record in the RIB with the same address and netmask as the provided two with the newer provided parameters.

#### RIB_clear

```C
/**
 * @function RIB_clear
 * @description clear Routing table entries
 * @param RIB*
 * @returns RIB_ret_code_t
 */

RIB_ret_code_t RIB_clear(RIB* rtab);
```

RIB_clear clears all the routes from the RIB

#### RIB_find

```C
/**
 * @function RIB_find
 * @description find a Route with provided network address in provided route table
 * @param RIB*
 * @param const char*
 * @param Route* found route
 * @returns RIB_ret_code_t
 */

RIB_ret_code_t RIB_find(RIB* rtab, const char* networkAddr, const char* netmask, Route** route);
```

RIB_find finds a route in the RIB with the network address and the netmask equal to the provided as arguments.
It returns the route in a Route* pointer.

#### RIB_match

```C
/**
 * @function RIB_match
 * @description find a matching route for the provided destination address; Longest prefix match is used in case of ambiguity; supports both ipv4 and ipv6
 * @param RIB*
 * @param const char* destination
 * @param Route* matched route; NULL if not found
 * @returns RIB_ret_code_t
 */

RIB_ret_code_t RIB_match(RIB* rtab, const char* destination, Route** route);
```

RIB_match returns the route to use to communicate with the provided ip address
The route to use  is returned as a Route* pointer.

---

## Known Issues

None, as far as I know

## Changelog

## License

View [LICENSE](LICENSE)
