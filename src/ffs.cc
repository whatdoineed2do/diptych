#include <iostream>
#include <string>

#include "Exif.h"
#include "Gopts.h"


_Gopts  thegopts;

int main(int argc, char* argv[])
{
    int c = 1;
    while (c < argc) {
        const char*  file = argv[c++];

	try {
	    Magick::Image  img (file);
	    diptych::Exif  exif = img;

	    std::cout << file << ": BEFORE  " << diptych::Exif(img) << "\n";

	    {
		const std::string  tag("Exif.Image.Make");
		Exiv2::ExifKey  key(tag);
		Exiv2::ExifMetadata::value_type  data(key);
		auto  p = std::find(exif.meta.begin(), exif.meta.end(), [&data](const Exiv2::ExifMetadata::value_type& rhs_) {
		    return rhs_.key() == std::string(data.key());
		});
		if (p == exif.meta.end()) {
		    data = "Nikon";
		    exif.meta.push_back( Exiv2::ExifMetadata::value_type(data));
		}
		else {
		    *p = "Nikon";
		}
	    }
	    {
		const std::string  tag("Exif.Image.Model");
		Exiv2::ExifKey  key(tag);
		Exiv2::ExifMetadata::value_type  data(key);
		data = "D300";
		exif.meta.push_back( Exiv2::ExifMetadata::value_type(data));
	    }
	    exif.merge(img);  // tmp Exif obj created??
	    std::cout << file << ": MERGE   " << exif << "\n";

	    exif.copy(img);
	    std::cout << file << ": AFTER   " << diptych::Exif(img) << "\n";

	}
	catch (const std::exception& ex)
	{
	    std::cout << file << ": " << ex.what() << "\n";
	}
    }

    return 0;
}
