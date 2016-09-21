/*
 * init.h
 *
 *  Created on: Sep 21, 2016
 *      Author: Jay
 */

#ifndef INIT_H_
#define INIT_H_

// initialize hardware subsystems - called once before main loop
void InitializeHardware(void);

// initialize global variables - called once before main loop
void InitializeVariables(void);

#endif /* INIT_H_ */
