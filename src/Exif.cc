#include "Exif.h"
#include "log.h"

#include <algorithm>


#ifdef HAVE_EXIV2
namespace Exiv2 {
bool operator<(const Exiv2::Exifdatum& l_, const Exiv2::Exifdatum& r_)
{
    return l_.key() < r_.key() || ((l_.key() == r_.key() && l_.toString() < r_.toString()) );
}
}
#endif


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


Exif::Exif(Magick::Image& img_)
{
    make     = img_.attribute(TAG_make);
    model    = img_.attribute(TAG_model);

    dateorig = img_.attribute(TAG_dateorig);
    artist    = img_.attribute(TAG_artist);
    copyright = img_.attribute(TAG_copyright);
    maxaperture = img_.attribute(TAG_maxaperture);
    focallen  = img_.attribute(TAG_focallen);

    _copyExif(img_);
}

Exif::Exif(const Exif& rhs_)
    : make(rhs_.make), model(rhs_.model), dateorig(rhs_.dateorig), artist(rhs_.artist), copyright(rhs_.copyright), maxaperture(rhs_.maxaperture), focallen(rhs_.focallen), exif(rhs_.exif)
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

	exif = rhs_.exif;
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

Exif&  Exif::operator=(Magick::Image& img_)
{
    _copyExif(img_);
    return *this;
}

void  Exif::_copyExif(Magick::Image& img_)
{
#ifdef HAVE_EXIV2
    try
    {
	Magick::Blob  raw;
	img_.write(&raw);
	const Magick::Blob  orig(raw.data(), raw.length());

	const auto  exiv = Exiv2::ImageFactory::open((const Exiv2::byte*)raw.data(), raw.length());
	exiv->readMetadata();
	exif = exiv->exifData();

	img_.read(orig);
    }
    catch (const std::exception& ex)
    {
	DIPTYCH_VERBOSE_LOG("failed to copy exif on " << img_.fileName() << " - " << ex.what());
    }
#endif
}

void  Exif::copy(Magick::Image& img_)
{
    if (!make.empty())         img_.attribute(TAG_make,        make);
    if (!model.empty())        img_.attribute(TAG_model,       model);
    if (!dateorig.empty())     img_.attribute(TAG_dateorig,    dateorig);
    if (!artist.empty())       img_.attribute(TAG_artist,      artist);
    if (!copyright.empty())    img_.attribute(TAG_copyright,   copyright);
    if (!maxaperture.empty())  img_.attribute(TAG_maxaperture, maxaperture);
    if (!focallen.empty())     img_.attribute(TAG_focallen,    focallen);

    assign(img_);
}

void  Exif::assign(Magick::Image& img_) 
{
#ifdef HAVE_EXIV2
    unsigned char*  ebuf = nullptr;
    try
    {
	// and attach the exif
	Exiv2::Blob  evraw;
	Exiv2::ExifParser::encode(evraw, Exiv2::littleEndian, (Exiv2::ExifData&)exif);
	ebuf = new unsigned char[6+evraw.size()];

	ebuf[0] = 'E';
	ebuf[1] = 'x';
	ebuf[2] = 'i';
	ebuf[3] = 'f';
	ebuf[4] = 0;
	ebuf[5] = 0;
	memcpy(ebuf+6, &evraw[0], evraw.size());

	img_.exifProfile(Magick::Blob(ebuf, 6+evraw.size()));

	DIPTYCH_VERBOSE_LOG("encoded exif=" << diptych::Exif(img_));
    }
    catch (const std::exception& ex)
    {
	std::cerr << "failed to attached generated exif - " << ex.what() << std::endl;
    }
    delete []  ebuf;
#endif
}

void  Exif::merge(const Exif& rhs_)
{
#ifdef HAVE_EXIV2
    try
    {
	if (meta.empty()) {
	    std::copy(exif.begin(), exif.end(), std::back_inserter(meta));
	    meta.sort();
	}

	auto  e = rhs_.exif;
	e.sortByKey();

	Exiv2::ExifMetadata  out;
	std::set_intersection(meta.begin(), meta.end(), e.begin(), e.end(),
			      std::back_inserter(out));

	meta = std::ref(out);
	meta.sort();
    }
    catch (const std::exception& e)
    {
    }
#endif
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
