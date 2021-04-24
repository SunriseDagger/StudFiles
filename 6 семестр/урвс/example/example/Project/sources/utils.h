#ifndef _UTILS_H
#define _UTILS_H

#include <fcntl.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdio.h>
#include "constants.h"

int saveToFile(const char *filename, char *msg);
void readDataFromFile(const char *filename, int *params, int totalParams);
double factorial(int k);
void error(const char *msg, int code);


#endif /* _UTILS_H */
