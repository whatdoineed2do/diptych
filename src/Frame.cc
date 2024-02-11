#include "Frame.h"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iomanip>
#include "Exif.h"

namespace diptych
{

std::ostream&  operator<<(std::ostream& os_, const ImgFrame& obj_)
{
    return os_ << "[" << obj_.size() << "] {back rows="  << obj_.back().rows() << " cols=" << obj_.back().cols() << "}";
}

Magick::Image  ImgFrame::process(const unsigned  trgt_)
{
    if (_imgs.empty()) {
	throw std::underflow_error("ImgFrame: no internal imgs");
    }
    /* everything is now same width or height so need to create the final
     * img
     */
    auto  img = _process(trgt_);
std::cout << "ImgFrame::process  " << diptych::Exif(img) << "\n";  // TODO
    return img;
}

ImgFrame::~ImgFrame()
{
    for (auto& i : _imgs) {
	delete i;
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
    for (const auto& i : _imgs)
    {
	const _ImgFrame&  img = *i;

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
    diptych::Exif  exif;
    diptych::Exif*  ep = nullptr;

    unsigned  j = 0;
    for (auto&  i : _imgs)
    {
	Magick::Image&  img = i->_read();
	if (!ep) {
	    exif = img;
	}

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

	diptych::scale(img, Magick::Geometry(0, trgt), thegopts.scale.ratio);

	DIPTYCH_DEBUG_LOG("VF=" << this << " " << j << '/' << _imgs.size() << "         to=" << img.columns() << "x" << img.rows() << "  exif=" << ImgFrame::Exif(img));
	imgs.push_back(img);

	exif.merge(img);
	ep = &exif;
    }

    /* never has border but internal padding
     * stack imgs top-bottom
     */

    Magick::Image  dest(Magick::Geometry(imgs.front().columns(), trgt_), thegopts.border.colour);
    dest.magick("TIFF");

    DIPTYCH_VERBOSE_LOG("vert frame dest cols=" << dest.columns() << " rows=" << dest.rows() << " seperator=" << _padding.intnl << " colour=" << thegopts.border.colour);

    unsigned  y = 0;
    for (const auto& img : imgs)
    {
	dest.composite(img, 0, y);

	DIPTYCH_VERBOSE_LOG("  y=" << std::setw(5) << y << " input cols=" << img.columns() << " rows=" << img.rows() << "  (" << img.fileName() << ")  { " << diptych::Exif((Magick::Image&)img) << " }");

	DIPTYCH_DEBUG_LOG("VF=" << this << " stacking y pos=" << y << " img rows=" << img.rows());

	y += img.rows() + _padding.intnl;
    }

    // replace the common exiv metadata to the final image
    exif.copy(dest);
std::cout << "v0: " << exif << "\n";  // TODO
std::cout << "v1: " << diptych::Exif(dest) << "\n";  // TODO

    return dest;
}

Magick::Image  HImgFrame::_process(const unsigned  trgt_)
{
    ImgFrame::_MImgs  imgs;

    diptych::Exif  exif;
    diptych::Exif*  ep = nullptr;

    for (auto& i : _imgs)
    {
	Magick::Image&  img = i->_read();
	imgs.push_back(img);

	if (!ep) {
	    exif = img;
	}
	else {
	    exif.merge(img);
	    ep = &exif;
	}
std::cout << "h:  " << exif << "\n";  // TODO
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
    dest.magick("TIFF");
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

    for (const auto& img : imgs) {
	dest.composite(img, x, y);
	DIPTYCH_VERBOSE_LOG("  x=" << std::setw(5) << x << " input cols=" << img.columns() << " rows=" << img.rows());
	x += img.columns() + _padding.intnl;
    }

    exif.copy(dest);

    return dest;
}

}
