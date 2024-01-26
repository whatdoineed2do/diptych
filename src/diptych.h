#ifndef DIP_DIPTYCH_H
#define DIP_DIPTYCH_H

#ifdef HAVE_CONFIG
#include <config.h>
#endif

#include <sys/types.h>
#include <iostream>
#include <Magick++.h>

#include "log.h"
#include "Padding.h"
#include "Frame.h"


inline
std::ostream&  operator<<(std::ostream& os_, const Magick::Geometry& obj_)
{
    return os_ << (std::string)obj_;
}

namespace diptych
{
void scale(Magick::Image& img_, const Magick::Geometry& g_, const float ratio_);

inline
bool  operator==(const Magick::Geometry& lhs_, const Magick::Geometry& rhs_)
{
    bool  w = true;
    bool  h = true;

    if (lhs_.width()  && rhs_.width())    w = lhs_.width()  == rhs_.width();
    if (lhs_.height() && rhs_.height())   h = lhs_.height() == rhs_.height();

    return w && h;
}

inline
bool  operator!=(const Magick::Geometry& lhs_, const Magick::Geometry& rhs_)
{ return !diptych::operator==(lhs_, rhs_); }

inline
bool  operator<(const Magick::Geometry& lhs_, const Magick::Geometry& rhs_)
{
    bool  w = true;
    bool  h = true;

    if (lhs_.width()  && rhs_.width())    w = lhs_.width()  < rhs_.width();
    if (lhs_.height() && rhs_.height())   h = lhs_.height() < rhs_.height();

    return w && h;
}

inline
Magick::Geometry&  operator/=(Magick::Geometry& obj_, const float n_)
{
    if (obj_.width()) {
	obj_.width(obj_.width()/n_);
    }

    if (obj_.height()) {
	obj_.height(obj_.height()/n_);
    }

    return obj_;
}
}


#endif
