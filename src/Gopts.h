#ifndef DIP_GOPTS_H
#define DIP_GOPTS_H

#include <functional>
#include <Magick++.h>


struct _Gopts {
    struct Brdr {
	const char*  colour;
	unsigned  width;
    } border, frame;

    struct Output {
	unsigned  quality;
	const char*  output;
	const char*  size;
    } output;

    const char*  resolution;

    struct {
	std::function< void (Magick::Image&, const Magick::Geometry&)>  scaler;
#ifdef HAVE_IM_RESIZE_FILTERTYPES
	Magick::FilterTypes  filter;
#else
	Magick::FilterType  filter;
#endif
	float  ratio;
    } scale;

    bool  excludeMeta;
    bool  verbose;

    _Gopts()
    {
	border.colour = "white";
	border.width = 10;
	frame.colour = "white";
	frame.width = 10;

	output.quality = 100;
	output.output = NULL;
	output.size = NULL;

	resolution = "300x300";
	
	scale.scaler = &Magick::Image::resize;
	scale.filter = Magick::UndefinedFilter;
	scale.ratio =  1;

	excludeMeta = false;
	verbose = false;
    };
};

extern _Gopts thegopts;

#endif
