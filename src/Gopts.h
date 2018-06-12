#ifndef DIP_GOPTS_H
#define DIP_GOPTS_H

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
	void (Magick::Image::*fptr)(const Magick::Geometry&);
	Magick::FilterTypes  filter;
	float  ratio;
    } scale;

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
	
	scale.fptr = &Magick::Image::resize;
	scale.filter = Magick::UndefinedFilter;
	scale.ratio =  1;

	verbose = false;
    };
};

extern _Gopts thegopts;

#endif
