/*
 * des.h
 *
 *  Created on: Feb 14, 2021
 *      Author: hurdleg
 */

#ifndef DOOR_ENTRY_SYSTEM_H_
#define DOOR_ENTRY_SYSTEM_H_

#define NUM_STATES 8	// where n = number of states in your finite state machine (FSM)
typedef enum {
	START_STATE = 0,
	SCAN_STATE = 1,
	DOOR_UNLOCK_STATE = 2,
	DOOR_OPEN_STATE = 3,
	WEIGHT_SCAN_STATE = 4,
	DOOR_CLOSE_STATE = 5,
	DOOR_LOCK_STATE = 6,
	EXIT_STATE = 7
} State;

#define NUM_INPUTS 12	// where n equals the number of input commands that drive the FSM.
// From the assignment specification, notice the prompt. Each command, ls, rs, etc.
// Count those...
typedef enum {
	L_SCAN = 0,
	GUARD_L_UNLOCK = 1,
	L_OPEN = 2,
	WEIGHT_SCAN = 3,
	L_CLOSE = 4,
	GUARD_L_LOCK = 5,
	GUARD_R_UNLOCK = 6,

	R_SCAN = 7,
	R_OPEN = 8,
	R_CLOSE = 9,
	GUARD_R_LOCK = 10,

	EXIT_INPUT = 11
} Input;

const char *inMessage[NUM_INPUTS] = { "ls", "glu", "lo", "ws", "lc", "gll",
		"gru",

		"rs", "ro", "rc", "grl", "exit" };

#define NUM_OUTPUTS 11	// where n equals the number of output messages from the FSM.
typedef enum {
	USAGE_OUTPUT = 0,
	SCAN_OUTPUT = 1,
	GUARD_L_UNLOCK_OUTPUT = 2,
	L_OPEN_OUTPUT = 3,
	WEIGHT_OUTPUT = 4,
	L_CLOSE_OUTPUT = 5,
	GUARD_L_LOCK_OUTPUT = 6,
	GUARD_R_UNLOCK_OUTPUT = 7,
	R_OPEN_OUTPUT = 8,
	R_CLOSE_OUTPUT = 9,
	GUARD_R_LOCK_OUTPUT = 10,

	EXIT_OUTPUT = 11
} Output;

const char *outMessage[NUM_OUTPUTS] =
		{
				"Enter the event type (ls = left scan, rs = right scan, ws = weight scan, lo = left open, ro = right open, lc = left closed, rc = right closed, gru = guard right unlock, grl = guard right lock, gll = guard left lock, glu = guard left unlock, exit = exit programs)\n",
				"Person scanned ID, ID = ",
				"Left door unlocked by guard\n",
				"Person opened left door\n",
				"Person weighed, weight = ",
				"Left door closed (automatically)\n",
				"Left door locked by guard\n",
				"Right door unlocked by guard\n",
				"Person opened right door\n",
				"Right door closed automatically\n",
				"Right door locked by guard\n"
				//"Exit\n"

		};

// inputs client sends a Person struct to its server, the controller
typedef struct {
	int id;
	int weight;
	int direction;
	int state;
	int weight_scanned;
} Person;

// controller client sends a Display struct to its server, the display
typedef struct {
//TODO :: fields for the output message and the Person. For the output message, I used an int, which
//	is the index into the outMessages array. That way I don't pass strings in my messages.
//	The Person field is needed, as some output message require information from the Person.
//	Specifically, those messages that display the Person's ID and weight.
	char message[255];
	int num;
	int withNum;
} Display;

#endif /* DOOR_ENTRY_SYSTEM_H_ */
