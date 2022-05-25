// Copyright 2022 Andrei Maruntis

#ifndef UTILS_H_
#define UTILS_H_

#include <errno.h>
#include <stdio.h>

// Macro pentru programare defensiva (scris de echipa de SD)
#define DIE(assertion, call_description)				\
	do {								\
		if (assertion) {					\
			fprintf(stderr, "(%s, %d): ", __FILE__,		\
				__LINE__);				\
			perror(call_description);			\
			exit(errno);					\
		}							\
	} while (0)

#endif  // UTILS_H_
