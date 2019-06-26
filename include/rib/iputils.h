/**
 *   librib - iputils.h
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

#ifndef IPUTILS_H
#define IPTUILS_H

int isValidIpAddress(const char* ipAddr, int* ipv);
int getCIDRnetmask(const char* netmask);
char* getIpv4NetworkAddress(const char* ipAddress, const char* netmask);
void formatIPv4Address(char** ipAddress);
int compareIPv4Addresses(const char* ipAddress, const char* cmpIpAddress);
char* getIpv6NetworkAddress(const char* ipAddress, int prefixLength);
void formatIPv6Address(char** ipAddress);
int compareIPv6Addresses(const char* ipAddress, const char* cmpIpAddress);

#endif
