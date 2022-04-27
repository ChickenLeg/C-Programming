#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <unistd.h>
#include <sys/neutrino.h>
#include <sys/netmgr.h>
#include <string.h>
#include <errno.h>
#include "../../des_display/src/des.h"

int main(int argc, char* argv[]) {
	char inputBuffer[50];
	int coid;

	Person person;

	person.state = START_STATE;
	int preState = START_STATE;
	int proceed = 0;

	if (argc != 2) {
		printf("Please enter your PID!");
		return EXIT_FAILURE;
	}

	pid_t serverpid = atoi(argv[1]);

	coid = ConnectAttach(ND_LOCAL_NODE, serverpid, 2, _NTO_SIDE_CHANNEL, 0);
	if (coid == -1) {
		printf("Could not ConnectAttach\n");
		return EXIT_FAILURE;
	}
	while (1) {
		proceed = 0;
		printf("%s\n", outMessage[USAGE_OUTPUT]);
		if (scanf("%s", inputBuffer) == -1) {
			printf("Error in des_inputs scanning\n");
			return EXIT_FAILURE;
		}
		if (!strcmp(inputBuffer, inMessage[EXIT_INPUT])) {
			person.state = EXIT_STATE;
			proceed = 1;
		} else if (!strcmp(inputBuffer, inMessage[L_SCAN])) {
			if (preState == START_STATE) {
				printf("Input person's id: \n");
				scanf("%d", &person.id);
				person.state = SCAN_STATE;
				person.direction = 0;
				person.weight_scanned = 0;
				proceed = 1;
			}
		} else if (!strcmp(inputBuffer, inMessage[R_SCAN])) {
			if (preState == START_STATE) {
				printf("Input person's id: \n");
				scanf("%d", &person.id);
				person.state = SCAN_STATE;
				person.direction = 1;
				person.weight_scanned = 0;
				proceed = 1;
			}
		} else if (!strcmp(inputBuffer, inMessage[WEIGHT_SCAN])) {
			if (preState == DOOR_OPEN_STATE) {
				printf("Input person's weight: \n");
				scanf("%d", &person.weight);
				person.state = WEIGHT_SCAN_STATE;
				proceed = 1;
			}
		} else if (!strcmp(inputBuffer, inMessage[GUARD_L_UNLOCK])) {
			if ((preState == SCAN_STATE && person.direction == 0)
					|| (preState == DOOR_LOCK_STATE && person.direction == 1)) {
				person.state = DOOR_UNLOCK_STATE;
				proceed = 1;
			}
		} else if (!strcmp(inputBuffer, inMessage[L_OPEN])) {
			if ((preState == DOOR_UNLOCK_STATE && person.direction == 0
					&& person.weight_scanned == 0)
					|| (preState == DOOR_UNLOCK_STATE && person.direction == 1
							&& person.weight_scanned == 1)) {
				person.state = DOOR_OPEN_STATE;
				proceed = 1;
			}
		} else if (!strcmp(inputBuffer, inMessage[L_CLOSE])) {
			if ((preState == WEIGHT_SCAN_STATE && person.direction == 0)
					|| (preState == DOOR_OPEN_STATE && person.direction == 1
							&& person.weight_scanned == 1)) {
				person.state = DOOR_CLOSE_STATE;
				proceed = 1;
			}
		} else if (!strcmp(inputBuffer, inMessage[GUARD_L_LOCK])) {
			if (preState == DOOR_CLOSE_STATE) {
				if (person.direction == 0 && person.weight_scanned == 0) {
					person.state = DOOR_LOCK_STATE;
					person.weight_scanned = 1;
					proceed = 1;
				} else if (person.direction == 1
						&& person.weight_scanned == 1) {
					person.state = START_STATE;
					person.weight_scanned = 0;
					person.weight = 0;
					person.id = 0;
					proceed = 1;
				}
			}
		} else if (!strcmp(inputBuffer, inMessage[GUARD_R_UNLOCK])) {
			if ((preState == SCAN_STATE && person.direction == 1)
					|| (preState == DOOR_LOCK_STATE && person.direction == 0)) {
				person.state = DOOR_UNLOCK_STATE;
				proceed = 1;
			}
		} else if (!strcmp(inputBuffer, inMessage[R_OPEN])) {
			if ((preState == DOOR_UNLOCK_STATE && person.direction == 0
					&& person.weight_scanned == 1)
					|| (preState == DOOR_UNLOCK_STATE && person.direction == 1
							&& person.weight_scanned == 0)) {
				person.state = DOOR_OPEN_STATE;
				proceed = 1;
			}
		} else if (!strcmp(inputBuffer, inMessage[R_CLOSE])) {
			if ((preState == WEIGHT_SCAN_STATE && person.direction == 1)
					|| (preState == DOOR_OPEN_STATE && person.direction == 0
							&& person.weight_scanned == 1)) {
				person.state = DOOR_CLOSE_STATE;
				proceed = 1;
			}
		} else if (!strcmp(inputBuffer, inMessage[GUARD_R_LOCK])) {
			if (preState == DOOR_CLOSE_STATE) {
				if (person.direction == 1 && person.weight_scanned == 0) {
					person.state = DOOR_LOCK_STATE;
					person.weight_scanned = 1;
					proceed = 1;
				} else if (person.direction == 0
						&& person.weight_scanned == 1) {
					person.state = START_STATE;
					person.weight_scanned = 0;
					person.weight = 0;
					person.id = 0;
					proceed = 1;
				}
			}
		}
		printf("\n");
		if (proceed) {
			preState = person.state;
			if (MsgSend(coid, &person, sizeof(person), NULL, 0) == -1L) {
				printf("Error during MsgSend\n");
				return EXIT_FAILURE;
			}
			if (preState == EXIT_STATE){
				break;
			}
		}
	}
	printf("Exit from inputs\n");
	ConnectDetach(coid);
	return EXIT_SUCCESS;
}
