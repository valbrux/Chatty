/*
	[file]
	@author:	REmaxer
	@last_mod:	05/02/13
	@lang:		C
	@desc:		chatty main source
	@vers:		0.1
*/
#include "../include/chatty.h"
#include <stdlib.h>

/* main function implementation  */
int main(int argc,char *argv[]){
	/* initialize chatty */
	if(!chatty_init())
		perror("Error");
	return 0;
}
