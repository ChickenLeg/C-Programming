#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <unistd.h>
#include <sys/neutrino.h>
#include <string.h>
#include <errno.h>
#include "des.h"

int main(void) {
	int rcvid;
	int chid;
	Display display;

	chid = ChannelCreate(0);
	if (chid == -1) {
		perror("Cannot create channel\n");
		return EXIT_FAILURE;
	}
	printf("DES display running, pid = %d\n", getpid());
	while (1) {
		rcvid = MsgReceive(chid, &display, sizeof(display), NULL);
		if (!strcmp(display.message, "exit")) {
			MsgReply(rcvid, EOK, NULL, 0);
			printf("Exit from display\n");
			break;
		}
		if (display.withNum) {
			printf("%s%d\n", display.message, display.num);
		} else {
			printf("%s", display.message);
		}

		MsgReply(rcvid, EOK, NULL, 0);
	}
	ChannelDestroy(chid);
	return EXIT_SUCCESS;
}
