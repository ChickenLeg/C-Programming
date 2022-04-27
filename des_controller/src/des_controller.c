#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <unistd.h>
#include <sys/neutrino.h>
#include <sys/netmgr.h>
#include <string.h>
#include <errno.h>
#include "../../des_display/src/des.h"

int skip;

void (*func)(Person, Display*);

void scan_state(Person, Display*);
void door_unlock(Person, Display*);
void door_open(Person, Display*);
void weight_scan(Person, Display*);
void door_close(Person, Display*);
void door_lock(Person, Display*);

int main(int argc, char* argv[]) {
	int coid;
	int rcvid;
	int chid;

	Display display;
	Person person;

	func = scan_state;

	if (argc != 2) {
		printf("Please input the pid of des_display\n");
		return EXIT_FAILURE;
	}

	pid_t serverpid = atoi(argv[1]);

	coid = ConnectAttach(ND_LOCAL_NODE, serverpid, 1, _NTO_SIDE_CHANNEL, 0);
	if (coid == -1) {
		printf("Could not ConnectAttach\n");
		return EXIT_FAILURE;
	}
	printf("The controller is running as PID: %d\nWaiting for a Person...\n",
			getpid());
	chid = ChannelCreate(0);
	if (chid == -1) {
		perror("Cannot create channel\n");
		return EXIT_FAILURE;
	}
	person.state = SCAN_STATE;
	while (1) {
		skip = 0;
		rcvid = MsgReceive(chid, &person, sizeof(person), NULL);
		if (person.state == EXIT_STATE) {
			printf("Exit from controller\n");
			strcpy(display.message, "exit");
			if (MsgSend(coid, &display, sizeof(display), NULL, 0) == -1L) {
				printf("Error during MsgSend\n");
				return EXIT_FAILURE;
			}
			MsgReply(rcvid, EOK, NULL, 0);
			break;
		}
		(*func)(person, &display);
		if (!skip) {
			if (MsgSend(coid, &display, sizeof(display), NULL, 0) == -1L) {
				printf("Error during MsgSend\n");
				return EXIT_FAILURE;
			}
		}
		MsgReply(rcvid, EOK, NULL, 0);
	}
	ChannelDestroy(chid);
	ConnectDetach(coid);
	return EXIT_SUCCESS;
}

void scan_state(Person person, Display* display) {
	strcpy(display->message, outMessage[SCAN_OUTPUT]);
	display->withNum = 1;
	display->num = person.id;

	func = door_unlock;
}
void door_unlock(Person person, Display* display) {
	display->withNum = 0;
	if ((!person.direction && !person.weight_scanned)
			|| (person.direction && person.weight_scanned)) {
		strcpy(display->message, outMessage[GUARD_L_UNLOCK_OUTPUT]);
	} else {
		strcpy(display->message, outMessage[GUARD_R_UNLOCK_OUTPUT]);
	}
	if (person.state == EXIT_OUTPUT) {
		strcpy(display->message, outMessage[EXIT_OUTPUT]);
	}

	func = door_open;
}
void door_open(Person person, Display* display) {
	display->withNum = 0;
	if (person.weight_scanned) {
		if (person.direction) {
			strcpy(display->message, outMessage[L_OPEN_OUTPUT]);
		} else {
			strcpy(display->message, outMessage[R_OPEN_OUTPUT]);
		}
		func = door_close;
	} else {
		if (!person.direction) {
			strcpy(display->message, outMessage[L_OPEN_OUTPUT]);
		} else {
			strcpy(display->message, outMessage[R_OPEN_OUTPUT]);
		}
		func = weight_scan;
	}
}
void weight_scan(Person person, Display* display) {
	strcpy(display->message, outMessage[WEIGHT_OUTPUT]);
	display->withNum = 1;
	display->num = person.weight;
	func = door_close;
}
void door_close(Person person, Display* display) {
	display->withNum = 0;
	if ((!person.direction && !person.weight_scanned)
			|| (person.direction && person.weight_scanned)) {
		strcpy(display->message, outMessage[L_CLOSE_OUTPUT]);
	} else if ((person.direction && !person.weight_scanned)
			|| (!person.direction && person.weight_scanned)) {
		strcpy(display->message, outMessage[R_CLOSE_OUTPUT]);
	}
	func = door_lock;
}
void door_lock(Person person, Display* display) {
	display->withNum = 0;
	if (person.state == START_STATE) {
		if (person.direction) {
			strcpy(display->message, outMessage[GUARD_L_LOCK_OUTPUT]);
		} else {
			strcpy(display->message, outMessage[GUARD_R_LOCK_OUTPUT]);
		}
		func = scan_state;
		return;
	}
	if (person.direction) {
		strcpy(display->message, outMessage[GUARD_R_LOCK_OUTPUT]);
	} else {
		strcpy(display->message, outMessage[GUARD_L_LOCK_OUTPUT]);
	}
	func = door_unlock;

}
