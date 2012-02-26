/*
 * sig.h
 *
 *  Created on: 2010-3-9
 *      Author: jivin
 */

#ifndef HAL_SIG_H_
#define HAL_SIG_H_
#include <signal.h>
#define _NSIG		64
#define _NSIG_BPW	32
#define _NSIG_WORDS	(_NSIG / _NSIG_BPW)
struct sigframe
{
	char *pretcode;
	int sig;
	struct sigcontext sc;
	struct _fpstate fpstate;
	unsigned long extramask[_NSIG_WORDS-1];
	char retcode[8];
};
#endif /* SIG_H_ */
