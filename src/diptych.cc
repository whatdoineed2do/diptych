#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <strings.h>
#include <math.h>
#include <limits.h>
#include <getopt.h>
#include <libgen.h>

#include <memory>
#include <array>
#include <string>
#include <stdexcept>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <cassert>
#include <list>


#include <Magick++.h>

#include "diptych.h"

namespace diptych
{
void  scale(Magick::Image& img_, const Magick::Geometry& g_, const float ratio_)
{
    if (thegopts.scale.filter != Magick::UndefinedFilter) {
	img_.filterType(thegopts.scale.filter);
    }

    if (ratio_ <= 1) {
	DIPTYCH_DEBUG_LOG(&img_ << "  scaling: direct to=" << g_.height() << "x" << g_.width());
	thegopts.scale.scaler(img_, g_);
	return;
    }


    using namespace diptych;
    Magick::Geometry  g(img_.columns(), img_.rows());

    DIPTYCH_DEBUG_LOG(&img_ << "  scaling: from=" << g.height() << "x" << g.width() << " to=" << g_.height() << "x" << g_.width());

    /* scale the img by steps to avoid posterisation effects */
    while (diptych::operator!=(g, g_) )
    {
	g /= ratio_;
	DIPTYCH_DEBUG_LOG(&img_ << "  scaling: " << g.height() << "x" << g.width());
	if (diptych::operator<(g, g_)) {
	    g = g_;
	    DIPTYCH_DEBUG_LOG(&img_ << "  scaling: " << g.height() << "x" << g.width() << " - final");
	}
	thegopts.scale.scaler(img_, g_);
	//img_.resize(g);
    }
}
}


_Gopts thegopts;

int main(int argc, char* const argv[])
{
    const char*  argv0 = basename(argv[0]);

    std::list<unsigned short>  verts;
    unsigned  reqfiles = 1;

    Magick::InitializeMagick(nullptr);

    int  c;
    while ( (c = getopt(argc, argv, "b:B:c:C:s:O:ho:q:vr:R:S:f:x")) != EOF) {
	switch (c)
	{
	    case 'b':
	    {
		thegopts.border.width = (unsigned)atoi(optarg);
	    } break;

	    case 'B':
	    {
		thegopts.frame.width = (unsigned)atoi(optarg);
	    } break;

	    case 'c':
	    {
	        try
		{
		    Magick::Color  c(optarg);
		    thegopts.border.colour = optarg;
		}
		catch (const std::exception& ex)
		{ }
	    } break;

	    case 'C':
	    {
	        try
		{
		    Magick::Color  c(optarg);
		    thegopts.frame.colour = optarg;
		}
		catch (const std::exception& ex)
		{ }
	    } break;

	    case 's':
	    {
		char*  q = optarg;
		char*  p = q;

		reqfiles = 0;

		while ( (p = strchr(q, ':')) )
		{
		    *p = (char)NULL;
		    verts.push_back((unsigned short)atoi(q));
		    reqfiles += verts.back();

		    q = p+1;
		}
		verts.push_back((unsigned short)atoi(q));
		reqfiles += verts.back();
	    } break;

	    case 'q':
	    {
		thegopts.output.quality = (unsigned short)atoi(optarg);
		if (thegopts.output.quality > 100) {
		    thegopts.output.quality = 100;
		}
	    } break;

	    case 'r':
	    {
		try {
		    const Magick::Geometry  g(optarg);
		    thegopts.resolution = optarg;
		}
		catch (...)
		{ }
	    } break;

	    case 'o':
	    {
	        thegopts.output.output = optarg;
	    } break;

	    case 'O':
	    {
		try {
		    const Magick::Geometry  g(optarg);
		    static const std::string  s = g;
		    thegopts.output.size = s.c_str();
		}
		catch (...)
		{ }
	    } break;

	    case 'R':
	    {
		thegopts.scale.scaler = *optarg == 'p' ?
			&Magick::Image::sample :
			&Magick::Image::scale;
	    } break;

	    case 'f':
	    {
		thegopts.scale.scaler = &Magick::Image::resize;
		struct IMfltrs {
		    const char*          name;
#ifdef HAVE_IM_RESIZE_FILTERTYPES
		    Magick::FilterTypes  fltr;
#else
		    Magick::FilterType  fltr;
#endif
		} imfltrs[] = 
		{
		    "Point", Magick::PointFilter,
		    "Box", Magick::BoxFilter,
		    "Triangle", Magick::TriangleFilter,
		    "Hermite", Magick::HermiteFilter,
		    "Hanning", Magick::HanningFilter,
		    "Hamming", Magick::HammingFilter,
		    "Blackman", Magick::BlackmanFilter,
		    "Gaussian", Magick::GaussianFilter,
		    "Quadratic", Magick::QuadraticFilter,
		    "Cubic", Magick::CubicFilter,
		    "Catrom", Magick::CatromFilter,
		    "Mitchell", Magick::MitchellFilter,
		    "Jinc", Magick::JincFilter,
		    "Sinc", Magick::SincFilter,
		    "SincFast", Magick::SincFastFilter,
		    "Kaiser", Magick::KaiserFilter,
		    "Welsh", Magick::WelshFilter,
		    "Parzen", Magick::ParzenFilter,
		    "Bohman", Magick::BohmanFilter,
		    "Bartlett", Magick::BartlettFilter,
		    "Lagrange", Magick::LagrangeFilter,
		    "Lanczos", Magick::LanczosFilter,
		    "LanczosSharp", Magick::LanczosSharpFilter,
		    "Lanczos2", Magick::Lanczos2Filter,
		    "Lanczos2Sharp", Magick::Lanczos2SharpFilter,
		    "Robidoux", Magick::RobidouxFilter,
		    "RobidouxSharpFilter", Magick::RobidouxSharpFilter,
		    "CosineFilter", Magick::CosineFilter,
		    "SplineFilter", Magick::SplineFilter,
		    "LanczosRadiusFilter", Magick::LanczosRadiusFilter,
		    // "CubicSplineFilter", Magick::CubicSplineFilter,
		    NULL, Magick::UndefinedFilter
		};
		const IMfltrs*  p = imfltrs;
		while (p->name != NULL) {
		    if (strcasecmp(p->name, optarg) == 0) {
			thegopts.scale.filter = p->fltr;
			break;
		    }
		    ++p;
		}

		if (p->name == NULL) {
		    std::cerr << "unknown IM resize filter '" << optarg << "' valid options: ";
		    p = imfltrs;
		    while (p->name != NULL) {
			std::cerr << p->name << " ";
			++p;
		    }
		    std::cerr << std::endl;
		    goto usage;
		}
	    } break;


	    case 'S':
	    {
		thegopts.scale.ratio = atof(optarg);
	    } break;

	    case 'x':
	    {
	        thegopts.excludeMeta = true;
	    } break;

	    case 'v':
	    {
	        thegopts.verbose = true;
	    } break;

usage:
	    case 'h':
	    default:
	        std::cout << argv0 << " " << PACKAGE_VERSION << " (with"
#ifndef HAVE_EXIV2
		     << "out"
#endif
		     << " exif support)\n"
		     << "usage:  " << argv0 << " [ -b <border width> -c <border colour> ] [ -B <external border width> -C <external border colour> ] [ -q <output quality%> ] [ -r <resolution> ] -s <components, ie 2:1> -o <output> [ -O <output geometry> ]  [-f <resize filter=lanczos|...] [-x]  <files>" << std::endl
		     << "        -B  = " << thegopts.frame.width << "pxls -C = " << thegopts.frame.colour << std::endl
		     << "        -b  = " << thegopts.border.width << "pxls -c = " << thegopts.border.colour << std::endl
		     << "        -q  = " << thegopts.output.quality << std::endl
		     << "        -r  = " << thegopts.resolution << std::endl
		     << "        -x    exclude metadata (default=no)\n";

		return 1;
	}
    }

    if (optind == argc) {
	std::cerr << argv0 << ": no files" << std::endl;
	goto usage;
    }

    if (thegopts.output.output == NULL) {
	goto usage;
    }

    if (verts.empty())
    {
	// no -s given!  figure out what we should do lets 1:1:1...
	reqfiles = argc - optind;
	int  o = optind;
	while (o < argc) {
	    verts.push_back(1);
	    ++o;
	}
    }

    if (reqfiles != argc - optind) {
	std::cerr << argv0 << ": insufficient files, req=" << reqfiles << " provided=" << argc-optind << std::endl;
	goto usage;
    }


    std::list<std::unique_ptr<diptych::VImgFrame>>  verticals;
    std::list<diptych::Padding>  paddings;

    /* this is the target height of the final HImgFrame, based on the smallest
     * ttl Y of all the vertical frames
     */
    unsigned  trgty = 0;

    try
    {
	std::for_each (verts.begin(), verts.end(), [&](const auto& i) {
	    // internal (ie verticals) don't have an outter border
	    paddings.push_back(diptych::Padding(thegopts.border.width, 0));
	    verticals.push_back(std::make_unique<diptych::VImgFrame>(paddings.back()));

	    unsigned short  N = i;
	    while (N-- > 0 && optind < argc)
	    {
		const char*  file = argv[optind++];

		if (access(file, R_OK) != 0) {
		    std::cerr << argv0 << ": " << file << " - " << strerror(errno) << std::endl;
		    throw std::runtime_error("EPERM");
		}

		try
		{
		    verticals.back()->push_back(file);
		}
		catch (const std::exception& ex)
		{
		    std::cerr << argv0 << ": unable to read file for composing verticals: " << file << " - " << ex.what() << std::endl;
		    throw;
		}
	    }

	    const unsigned  y = verticals.back()->ttly();
	    if (trgty == 0 || y < trgty) {
		DIPTYCH_DEBUG_LOG("re-eval for Y final target from=" << trgty << " to=" << y << "  based on " << verticals.back().get() << " " << *verticals.back().get());
		// doing this can scale up imgs!!
		//trgty = y + (verticals.back()->size()-1) * thegopts.border.width;
		trgty = y;
	    }
	});
    }
    catch (...)
    {
        return 1;
    }

    int  retcode = 0;
    try
    {
	diptych::Padding  hpad(thegopts.border.width, thegopts.frame.width);
	diptych::HImgFrame  horizontals(hpad);
	DIPTYCH_DEBUG_LOG("hframe=" << &horizontals << " - generating verticals/scalings");

	std::for_each (verticals.begin(), verticals.end(), [&horizontals, &trgty, &argv0](auto& i) {
	    try
	    {
		/* ensure that all vertical frames are the same (trgty) height
		 */
		horizontals.push_back( i->process(trgty) );
	    }
	    catch (const std::exception& ex)
	    {
		std::cerr << argv0 << ": failed to generate vertical component - " << ex.what() << std::endl;
		throw;
	    }
	});
	Magick::Image  final = horizontals.process(trgty);

	if (thegopts.output.size) {
	    const Magick::Geometry  a = final.size();
	    const Magick::Geometry  b = thegopts.output.size;

	    const diptych::ImgFrame::Exif  e(final);
	    diptych::scale(final, b, thegopts.scale.ratio);
	    e.copy(final);
	}
	final.quality(thegopts.output.quality);

	try
	{
	    if (thegopts.excludeMeta) {
		constexpr std::array  profileids {
		     "EXIF", "ICM", "IPTC"
		};
		for (const auto& id : profileids) {
		    final.profile(id, Magick::Blob());
		}
	    }
	    final.write(thegopts.output.output);
	}
	catch (const std::exception& ex)
	{
	    std::ostringstream  out;
	    out << "/tmp/" << argv0 << "." << time(NULL) << "." << getpid() << ".jpg";
	    std::cerr << argv0 << ": failed to write final image - " << ex.what() << ", will use file=" << out.str() << std::endl;
	    final.write(out.str());

	    retcode = 1;
	}
    }
    catch (const std::exception& ex)
    {
	std::cerr << argv0 << ": failed to generate final image - " << ex.what() << std::endl;
	retcode = 1;
    }
    verticals.clear();

    Magick::TerminateMagick();

    return retcode;
}
