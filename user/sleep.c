#include "kernel/types.h"
#include "user/user.h"
#include "kernel/stat.h"

int main(int argc, char* argv[]) {
	if (argc < 2) {
		fprintf(2, "usage: sleep ticks\n");
		exit(1);
	}
	int ticks = atoi(argv[1]);
	if(ticks < 0) {
		fprintf(2, "sleep: ticks should be positive integers\n");
		exit(1);
	}
	pause(ticks);
	exit(0);
}
