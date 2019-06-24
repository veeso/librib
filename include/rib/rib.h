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

// Data types

typedef struct RIB {
  Route** routes;
  size_t entries;
} RIB;

// Functions

// Table manipulation functions 
RIB* RIB_init();
int RIB_free(RIB* rtab);
int RIB_add(RIB* rtab, const char* destination, const char* netmask, const char* gateway, const char* iface, int metric);
int RIB_delete(RIB* rtab, const char* destination);
int RIB_update(RIB* rtab, const char* destination, const char* newDestination, const char* newNetmask, const char* newGateway, const char* newIface, int newMetric);
int RIB_clear(RIB* rtab);

// Table querying functions

Route* RIB_find(RIB* rtab, const char* networkAddr);
Route* RIB_match(RIB* rtab, const char* destination);
Route* RIB_match_ipv4(RIB* rtab, const char* destination);
Route* RIB_match_ipv6(RIB* rtab, const char* destination);

#endif
