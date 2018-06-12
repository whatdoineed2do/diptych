#ifndef DIP_LOG_H
#define DIP_LOG_H

#include <iostream>

#include "Gopts.h"

#define DIPTYCH_VERBOSE_LOG(x) if (thegopts.verbose) { std::cout << x << std::endl; }

#ifdef DEBUG_LOG
#define DIPTYCH_DEBUG_LOG(x) { std::cout << "DEBUG:  " << x << std::endl; }
#else
#define DIPTYCH_DEBUG_LOG(x)
#endif

#endif
