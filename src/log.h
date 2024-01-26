#ifndef DIP_LOG_H
#define DIP_LOG_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>

#include "Gopts.h"

#define DIPTYCH_VERBOSE_LOG(x) if (thegopts.verbose) { std::cout << x << std::endl; }

#ifndef NDEBUG
#define DIPTYCH_DEBUG_LOG(x) { std::cout << "DEBUG:  " << x << std::endl; }
#else
#define DIPTYCH_DEBUG_LOG(x)
#endif

#endif
