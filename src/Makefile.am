ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

AM_CXXFLAGS = $(MAGICK_CFLAGS) $(EXIV_CFLAGS) -Wno-conversion-null -Wno-deprecated
AM_LDFLAGS  = $(MAGICK_LIBS) ${EXIV_LIBS)

bin_PROGRAMS = diptych

diptych_CPPFLAGS = -DGEN_EXIF -DNEED_UCHAR_UINT_T
diptych_CXXFLAGS = $(AM_CXXFLAGS)
#diptych_LDFLAGS = 
#diptych_LDADD =
diptych_SOURCES = \
	diptych.cc

