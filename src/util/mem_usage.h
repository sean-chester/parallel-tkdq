/*
 * mem_usage.h
 *
 *  Created on: Dec 18, 2013
 *      Author: darius
 */

#ifndef MEM_USAGE_H_
#define MEM_USAGE_H_

#include <unistd.h>
#include <ios>
#include <iostream>
#include <fstream>
#include <string>

//////////////////////////////////////////////////////////////////////////////
//
// process_mem_usage(double &, double &) - takes two doubles by reference,
// attempts to read the system-dependent data for a process' virtual memory
// size and resident set size, and return the results in KB.
//
// On failure, returns 0.0, 0.0
// Example of usage:
//  int main() {
//    double vm, rss;
//    process_mem_usage(vm, rss);
//    cout << "VM: " << vm << "; RSS: " << rss << endl;
//  }

void process_mem_usage(double& vm_usage, double& resident_set);

#endif /* MEM_USAGE_H_ */
