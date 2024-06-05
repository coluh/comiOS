#ifndef __BASIC_H__
#define __BASIC_H__

#include "basic_memlayout.h"
#include "basic_types.h"
#include "basic_riscv.h"
#include "basic_parameters.h"

#define NELEM(x) (sizeof(x)/sizeof(x[0]))
#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX(x, y) ((x) > (y) ? (x) : (y))

#endif
