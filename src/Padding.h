#ifndef DIP_PADDING_H
#define DIP_PADDING_H

#include <sys/types.h>

namespace diptych {

struct Padding
{
    Padding(const unsigned intnl_, const unsigned extnl_) : intnl(intnl_), extnl(extnl_) { }

    const unsigned  intnl;
    const unsigned  extnl;
};

}

#endif
