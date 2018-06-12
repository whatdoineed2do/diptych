#include "Frame.h"

#include <iomanip>

namespace diptych
{

const std::string  ImgFrame::Exif::TAG_make      = "exif:Make";
const std::string  ImgFrame::Exif::TAG_model     = "exif:Model";
const std::string  ImgFrame::Exif::TAG_dateorig  = "exif:DateTimeOriginal";
const std::string  ImgFrame::Exif::TAG_artist    = "exif:Artist";
const std::string  ImgFrame::Exif::TAG_copyright = "exif:Copyright";
const std::string  ImgFrame::Exif::TAG_maxaperture = "exif:MaxApertureValue";
const std::string  ImgFrame::Exif::TAG_focallen  = "exif:FocalLength";


std::ostream&  operator<<(std::ostream& os_, const ImgFrame::Exif& obj_)
{
    return os_ << "make=" << obj_.make << " model=" << obj_.model << " date=" << obj_.dateorig << " focallen=" << obj_.focallen << " max f/=" << obj_.maxaperture;
}



ImgFrame::Exif::Exif(const Magick::Image& img_)
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

ImgFrame::Exif::Exif(const Exif& rhs_) : make(rhs_.make), model(rhs_.model), dateorig(rhs_.dateorig), artist(rhs_.artist), copyright(rhs_.copyright), maxaperture(rhs_.maxaperture), focallen(rhs_.focallen)
{ }

const ImgFrame::Exif& ImgFrame::Exif::operator=(const Exif& rhs_)
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

const bool ImgFrame::Exif::operator==(const Exif& rhs_) const
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

void  ImgFrame::Exif::copy(Magick::Image& img_) const
{
    if (!make.empty())         img_.attribute(TAG_make,        make);
    if (!model.empty())        img_.attribute(TAG_model,       model);
    if (!dateorig.empty())     img_.attribute(TAG_dateorig,    dateorig);
    if (!artist.empty())       img_.attribute(TAG_artist,      artist);
    if (!copyright.empty())    img_.attribute(TAG_copyright,   copyright);
    if (!maxaperture.empty())  img_.attribute(TAG_maxaperture, maxaperture);
    if (!focallen.empty())     img_.attribute(TAG_focallen,    focallen);
}


bool  ImgFrame::Exif::clean(const Exif& rhs_)
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


Magick::Image  ImgFrame::process(const unsigned  trgt_)  throw (std::underflow_error)
{
    if (_imgs.empty()) {
	throw std::underflow_error("ImgFrame: no internal imgs");
    }
    /* everything is now same width or height so need to create the final
     * img
     */
    Magick::Image  img = _process(trgt_);

#ifdef GEN_EXIF
    if (exif())
    {
	try
	{
	    // and attach the exif
	    Exiv2::Blob      evraw;
	    Exiv2::ExifData  evexif;

	    evexif["Exif.Image.Make"]     = exif().make;
	    evexif["Exif.Image.Model"]    = exif().model;
	    evexif["Exif.Photo.DateTimeOriginal"] = exif().dateorig;
	    evexif["Exif.Image.Artist"]   = exif().artist;
	    evexif["Exif.Image.Copyright"] = exif().copyright;
	    evexif["Exif.Photo.MaxApertureValue"] = exif().maxaperture;
	    evexif["Exif.Photo.FocalLength"] = exif().focallen;

	    Exiv2::ExifParser::encode(evraw, Exiv2::littleEndian, evexif);
	    unsigned char*  ebuf = new unsigned char[6+evraw.size()];
	    ebuf[0] = 'E';
	    ebuf[1] = 'x';
	    ebuf[2] = 'i';
	    ebuf[3] = 'f';
	    ebuf[4] = 0;
	    ebuf[5] = 0;
	    memcpy(ebuf+6, &evraw[0], evraw.size());

	    img.exifProfile(Magick::Blob(ebuf, 6+evraw.size()));
	    delete [] ebuf;

	    DIPTYCH_VERBOSE_LOG("encoded exif=" << ImgFrame::Exif(img));
	}
	catch (const std::exception& ex)
	{
	    std::cerr << "failed to attached generated exif - " << ex.what() << std::endl;
	}
    }
#endif
    return img;
}
ImgFrame::~ImgFrame()
{
    for (ImgFrame::Imgs::iterator i=_imgs.begin(); i!=_imgs.end(); ++i) {
	delete *i;
    }
    delete _exif;
}

void  ImgFrame::push_back(Magick::Image img_)
{
    _imgs.push_back(new _ImgFrame(img_));
    _ttly += _imgs.back()->rows();
    _ttlx += _imgs.back()->cols();

    DIPTYCH_DEBUG_LOG(this << " : adding M:  ttlx=" << _ttlx << " ttly=" << _ttly << ", x/y=" << _imgs.back()->cols() << "x" << _imgs.back()->rows());

    _tracksmallest(*_imgs.back());
}

void  ImgFrame::push_back(const char* img_)
{
    _imgs.push_back(new _ImgFrame(img_));
    _ttly += _imgs.back()->rows();

    DIPTYCH_DEBUG_LOG(this << " : adding c*:  ttly=" << _ttly << ", x/y cols/rows=" << _imgs.back()->cols() << "x" << _imgs.back()->rows() << "  " << img_);

    _tracksmallest(*_imgs.back());
}


const unsigned  ImgFrame::ttly()
{
    unsigned  y = 0;
    for (ImgFrame::Imgs::const_iterator i=_imgs.begin(); i!=_imgs.end(); ++i)
    {
	const _ImgFrame&  img = **i;

	// figure out the scaling ratio so that all X are the same -
	// affects the Y

	const double  sr = (double)_smallest->cols() / (double)img.cols();
	y += img.rows() * sr;

	DIPTYCH_DEBUG_LOG("IF=" << this << " : scaled Y: smallest x=" << _smallest->cols() << ", curr x=" << img.cols() << " ratio=" << sr << "  ttly=" << y << "  (y scaled from=" << img.rows() << " to=" << img.rows()*sr << ")");
    }
    DIPTYCH_DEBUG_LOG("IF=" << this << ": final ttly=" << y);
    _ttly = y;

    return _ttly;
}

bool  ImgFrame::_cmpExif(const Magick::Image& img_)
{
    bool  b = false;

    const ImgFrame::Exif  e(img_);
    if (_exif == NULL) {
	_exif = new ImgFrame::Exif(e);
    }
    else
    {
	if (*_exif == e) {
	}
	else
	{
	    DIPTYCH_DEBUG_LOG("   " << this << " exif mismatch - cleaning: orig=" << *_exif << "new=" << e);

	    if (_exif->clean(e)) {
	    }
	    else {
		b = true;
		delete _exif;
		_exif = NULL;
	    }
	}
    }
    return b;
}

void  ImgFrame::_tracksmallest(const _ImgFrame& img_)
{
    /* keep track of the min x (width) and the relevant scale ratio across
     * all imgs
     */
    if (_smallest == NULL) {
	_smallest = &img_;
    }
    else
    {
	if (img_.cols() < _smallest->cols()) {
	    _smallest = &img_;
	}
    }
}


Magick::Image  VImgFrame::_process(const unsigned  trgt_)
{
    ImgFrame::_MImgs  imgs;

    /* scale to the target height, taking care that resulting scaled 
     * internal
     * images reflect the req'd padding
     */
    const unsigned  ttlpad = _padding.intnl * (_imgs.size()-1);
    const unsigned  adjust = _imgs.size() == 1 ? 0 : ceil(ttlpad/_imgs.size());
    const double  sr     = _imgs.size() == 1 ? 1 : trgt_/(double)_ttly;

    unsigned  remain = trgt_;

    DIPTYCH_DEBUG_LOG("VF=" << this << " : target=" << trgt_ << " padding=" << _padding.intnl << " imgs=" << _imgs.size() << " ttl pad=" << ttlpad << " adjust=" << adjust << " scale ratio=" << sr);

    const double  smallestx = _smallest->cols();
    bool  skip = false;
    unsigned  j = 0;
    for (ImgFrame::Imgs::iterator i=_imgs.begin(); i!=_imgs.end(); ++i)
    {
	Magick::Image&  img = (*i)->_read();

	unsigned  trgt = 0;
	if (++j == _imgs.size())
	{
	    /* dealing with the last frame, need to make sure that we have
	     * filled met trgt_ size
	     */
	    trgt = remain - adjust;
	}
	else
	{
	    const double  wsr = smallestx / (double)img.columns();
	    trgt = (img.rows() * wsr * sr) - adjust;
	}
	remain -= trgt+adjust;

	DIPTYCH_DEBUG_LOG("VF=" << this << " " << j << '/' << _imgs.size() << " scale from=" << img.columns() << "x" << img.rows() << " trgt y=" << trgt << "  remain y=" << remain << "/" << trgt_);

	DIPTYCH_SCALE(img, Magick::Geometry(0, trgt), thegopts.scale.ratio);

	DIPTYCH_DEBUG_LOG("VF=" << this << " " << j << '/' << _imgs.size() << "         to=" << img.columns() << "x" << img.rows() << "  exif=" << ImgFrame::Exif(img));
	imgs.push_back(img);


	if (skip) {
	    continue;
	}
	skip = _cmpExif(img);
    }

    /* never has border but internal padding
     * stack imgs top-bottom
     */

    Magick::Image  dest(Magick::Geometry(imgs.front().columns(), trgt_), thegopts.border.colour);

    DIPTYCH_VERBOSE_LOG("vert frame dest cols=" << dest.columns() << " rows=" << dest.rows() << " seperator=" << _padding.intnl << " colour=" << thegopts.border.colour);

    unsigned  y = 0;
    for (ImgFrame::_MImgs::const_iterator i=imgs.begin(); i!=imgs.end(); ++i)
    {
	const Magick::Image&  img = *i;
	dest.composite(img, 0, y);

	DIPTYCH_VERBOSE_LOG("  y=" << std::setw(5) << y << " input cols=" << img.columns() << " rows=" << img.rows() << "  (" << i->fileName() << ")  { " << ImgFrame::Exif(img) << " }");

	DIPTYCH_DEBUG_LOG("VF=" << this << " stacking y pos=" << y << " img rows=" << img.rows());

	y += img.rows() + _padding.intnl;
    }

    return dest;
}

Magick::Image  HImgFrame::_process(const unsigned  trgt_)
{
    ImgFrame::_MImgs  imgs;

    bool  skip = false;
    for (ImgFrame::Imgs::iterator i=_imgs.begin(); i!=_imgs.end(); ++i)
    {
	Magick::Image&  img = (*i)->_read();
	imgs.push_back(img);

	if (skip) {
	    continue;
	}
	skip = _cmpExif(img);
    }


    /* stack left to right
    */

    unsigned  w = 0;
    for (ImgFrame::_MImgs::const_iterator i=imgs.begin(); i!=imgs.end(); ++i) {
	if (i != imgs.begin()) {
	    w += _padding.intnl;
	}
	w += i->columns();
    }


    Magick::Image  dest(Magick::Geometry(w+2*_padding.extnl, imgs.front().rows()+2*_padding.extnl), thegopts.frame.colour);
    DIPTYCH_DEBUG_LOG("HF=" << this << " target=" << dest.columns() << "x" << dest.rows() << " (w/o border=" << w << 'x' << imgs.front().rows() << ")");

    dest.resolutionUnits(Magick::PixelsPerInchResolution);
    dest.density(thegopts.resolution);

    DIPTYCH_VERBOSE_LOG("horz frame dest cols=" << dest.columns() << " rows=" << dest.rows() << " border=" << _padding.extnl << " colour=" << thegopts.frame.colour);

    unsigned  x = 0;
    unsigned  y = 0;

    /* requested an external border to the entire final composite?
    */
    if (_padding.extnl) {
	x = _padding.extnl;
	y = _padding.extnl;
    }

    for (ImgFrame::_MImgs::const_iterator i=imgs.begin(); i!=imgs.end(); ++i) {
	const Magick::Image&  img = *i;
	dest.composite(img, x, y);
	DIPTYCH_VERBOSE_LOG("  x=" << std::setw(5) << x << " input cols=" << img.columns() << " rows=" << img.rows());
	x += img.columns() + _padding.intnl;
    }

    return dest;
}

}
