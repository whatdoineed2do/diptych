#ifndef DIP_FRAME_H
#define DIP_FRAME_H

#include <sys/types.h>

#include <string>
#include <stdexcept>
#include <iostream>
#include <cassert>
#include <list>


#include <Magick++.h>


#include "diptych.h"
#include "Padding.h"


namespace diptych  {

class Frame
{
  public:
    virtual ~Frame() = default;

    Frame(const Frame&) = delete;
    void operator=(const Frame&) = delete;

   virtual Magick::Image  process(const unsigned) = 0;

  protected:
    Frame() = default;
};

class ImgFrame;

class _ImgFrame
{
  public:
    friend class ImgFrame;

    _ImgFrame(Magick::Image& img_) : _img(img_), _full(true) { }

    _ImgFrame(const char* img_) : _img(img_), _full(false)
    { _img.ping(img_); }

    ~_ImgFrame() = default;

    unsigned  rows() const { return _img.rows(); }
    unsigned  cols() const { return _img.columns(); }

    Magick::Image&  _read()
    {
	if (_full) { return _img; }

	_img.read(_img.fileName());
	_full = true;
	return _img;
    }

  private:
    Magick::Image  _img;
    bool  _full;
};


class ImgFrame : public Frame
{
  public:
    typedef std::list<_ImgFrame*>  Imgs;
    typedef std::list<Magick::Image>  _MImgs;

    struct Exif
    {
	static const std::string  TAG_make;
	static const std::string  TAG_model;
	static const std::string  TAG_dateorig;
	static const std::string  TAG_artist;
	static const std::string  TAG_copyright;
	static const std::string  TAG_maxaperture;
	static const std::string  TAG_focallen;

	Exif() { }

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
    };

    virtual ~ImgFrame();

    virtual void  push_back(Magick::Image img_);
    virtual void  push_back(const char* img_);

    const _ImgFrame&  back() const
    { return *_imgs.back(); }

    Magick::Image  process(const unsigned);

    /* consolidated exif across all the push_back'd img frames
     */
    const ImgFrame::Exif&  exif() const
    { 
	static ImgFrame::Exif  tmp;
	return _exif ? *_exif : tmp;
    }

    const unsigned  size() const
    { return _imgs.size(); }

    const unsigned  ttly();

  protected:
    ImgFrame(Padding&  padding_) : _exif(NULL), _ttlx(0), _ttly(0), _padding(padding_), _smallest(NULL) { }

    ImgFrame::Exif*  _exif;

    ImgFrame::Imgs  _imgs;

    virtual Magick::Image  _process(const unsigned  trgt_) = 0;

    bool  _cmpExif(const Magick::Image& img_);

    /*mutable*/ Padding&  _padding;

    unsigned  _ttlx;
    unsigned  _ttly;

    const _ImgFrame*  _smallest;  // represents the smallest img along X

  private:
    void  _tracksmallest(const _ImgFrame& img_);
};

std::ostream&  operator<<(std::ostream& os_, const ImgFrame::Exif& obj_);
std::ostream&  operator<<(std::ostream& os_, const ImgFrame& obj_);



class VImgFrame : public ImgFrame
{
  public:
    VImgFrame(Padding& p_) : ImgFrame(p_)
    { }

  private:
    Magick::Image  _process(const unsigned  trgt_);
};

class HImgFrame : public ImgFrame
{
  public:
    HImgFrame(Padding& p_) : ImgFrame(p_)
    { }

  private:
    Magick::Image  _process(const unsigned  trgt_);
};

}

#endif
