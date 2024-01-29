#ifndef DIP_EXIF_H
#define DIP_EXIF_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string>
#include <iosfwd>

#include <Magick++.h>

#ifdef HAVE_EXIV2
#include <exiv2/exiv2.hpp>
#endif


namespace diptych  {
 
class Exif
{
  public:
    static const std::string  TAG_make;
    static const std::string  TAG_model;
    static const std::string  TAG_dateorig;
    static const std::string  TAG_artist;
    static const std::string  TAG_copyright;
    static const std::string  TAG_maxaperture;
    static const std::string  TAG_focallen;

    Exif() = default;
    ~Exif() = default;

    Exif(const Magick::Image& img_);
    Exif(const Exif& rhs_);

    const Exif& operator=(const Exif& rhs_);
    const bool operator==(const Exif& rhs_) const;

    const bool operator!=(const Exif& rhs_) const
    { return !operator==(rhs_); }

    operator bool() const 
    { return make.empty() ? false : true; }

    void  copy(Magick::Image& img_) const;

    bool  clean(const Exif& rhs_);


    std::string  make;
    std::string  model;
    std::string  dateorig;

    std::string  artist;
    std::string  copyright;
    std::string  maxaperture;
    std::string  focallen;

#ifdef HAVE_EXIV2
    Exiv2::ExifData  exif;
#endif

  private:
};

std::ostream&  operator<<(std::ostream& os_, const Exif& obj_);
} 

#endif
