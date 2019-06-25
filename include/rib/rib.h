/**
 *   librib - rib.h
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

#ifndef RIB_H
#define RIB_H

#include <rib/route.h>

#include <stdlib.h>

// Data types

typedef struct RIB {
  Route** routes;
  size_t entries;
} RIB;

typedef enum RIB_ret_code_t {
  RIB_NO_ERROR,
  RIB_INVALID_ADDRESS,
  RIB_NO_MATCH,
  RIB_DUP_RECORD,
  RIB_NOT_EXISTS,
  RIB_UNINITIALIZED_RIB,
  RIB_BAD_ALLOC
} RIB_ret_code_t;

// Functions

// Table manipulation functions 
RIB_ret_code_t RIB_init(RIB** rtab);
RIB_ret_code_t RIB_free(RIB* rtab);
RIB_ret_code_t RIB_add(RIB* rtab, const char* destination, const char* netmask, const char* gateway, const char* iface, int metric);
RIB_ret_code_t RIB_delete(RIB* rtab, const char* destination, const char* netmask);
RIB_ret_code_t RIB_update(RIB* rtab, const char* destination, const char* netmask, const char* newNetmask, const char* newGateway, const char* newIface, int newMetric);
RIB_ret_code_t RIB_clear(RIB* rtab);

// Table query functions

RIB_ret_code_t RIB_find(RIB* rtab, const char* networkAddr, const char* netmask, Route** route);
RIB_ret_code_t RIB_match(RIB* rtab, const char* destination, Route** route);
RIB_ret_code_t RIB_match_ipv4(RIB* rtab, const char* destination, Route** route);
RIB_ret_code_t RIB_match_ipv6(RIB* rtab, const char* destination, Route** route);

#endif
