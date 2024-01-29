#include "Exif.h"
#include "log.h"

namespace diptych
{

const std::string  Exif::TAG_make      = "exif:Make";
const std::string  Exif::TAG_model     = "exif:Model";
const std::string  Exif::TAG_dateorig  = "exif:DateTimeOriginal";
const std::string  Exif::TAG_artist    = "exif:Artist";
const std::string  Exif::TAG_copyright = "exif:Copyright";
const std::string  Exif::TAG_maxaperture = "exif:MaxApertureValue";
const std::string  Exif::TAG_focallen  = "exif:FocalLength";


std::ostream&  operator<<(std::ostream& os_, const Exif& obj_)
{
    return os_ << "make=" << obj_.make << " model=" << obj_.model << " date=" << obj_.dateorig << " focallen=" << obj_.focallen << " max f/=" << obj_.maxaperture;
}


Exif::Exif(const Magick::Image& img_)
{
    Magick::Image& img = (Magick::Image&)img_;
    make     = img.attribute(TAG_make);
    model    = img.attribute(TAG_model);

    dateorig = img.attribute(TAG_dateorig);
    artist    = img.attribute(TAG_artist);
    copyright = img.attribute(TAG_copyright);
    maxaperture = img.attribute(TAG_maxaperture);
    focallen  = img.attribute(TAG_focallen);
}

Exif::Exif(const Exif& rhs_) : make(rhs_.make), model(rhs_.model), dateorig(rhs_.dateorig), artist(rhs_.artist), copyright(rhs_.copyright), maxaperture(rhs_.maxaperture), focallen(rhs_.focallen)
{ }

const Exif& Exif::operator=(const Exif& rhs_)
{
    if (&rhs_ != this) {
	make      = rhs_.make;
	model     = rhs_.model;
	dateorig  = rhs_.dateorig;
	artist    = rhs_.artist;
	copyright = rhs_.copyright;
	maxaperture = rhs_.maxaperture;
	focallen  = rhs_.focallen;
    }
    return *this;
}

const bool Exif::operator==(const Exif& rhs_) const
{
    if (&rhs_ == this) {
	return true;
    }

    std::string  a = dateorig;
    std::string  b = rhs_.dateorig;
    std::string::size_type  p = a.find(" ");
    if (p != std::string::npos) {
	a.erase(p);
    }
    p = b.find(" ");
    if (p != std::string::npos) {
	b.erase(p);
    }

    return make == rhs_.make && model == rhs_.model && a == b;
}

void  Exif::copy(Magick::Image& img_) const
{
    if (!make.empty())         img_.attribute(TAG_make,        make);
    if (!model.empty())        img_.attribute(TAG_model,       model);
    if (!dateorig.empty())     img_.attribute(TAG_dateorig,    dateorig);
    if (!artist.empty())       img_.attribute(TAG_artist,      artist);
    if (!copyright.empty())    img_.attribute(TAG_copyright,   copyright);
    if (!maxaperture.empty())  img_.attribute(TAG_maxaperture, maxaperture);
    if (!focallen.empty())     img_.attribute(TAG_focallen,    focallen);
}


bool  Exif::clean(const Exif& rhs_)
{
    if (make != rhs_.make) {
	return false;
    }

    if (model       != rhs_.model)        model.clear();
    if (dateorig    != rhs_.dateorig)     dateorig.clear();
    if (artist      != rhs_.artist)       artist.clear();
    if (copyright   != rhs_.copyright)    copyright.clear();
    if (maxaperture != rhs_.maxaperture)  maxaperture.clear();
    if (focallen    != rhs_.focallen)     focallen.clear();

    return true;
}

}
