ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

AM_CXXFLAGS = $(IMAGEMAGICK_CFLAGS) $(EXIV_CFLAGS) -Wno-conversion-null -Wno-deprecated
AM_LDFLAGS  = $(IMAGEMAGICK_LIBS) $(EXIV_LIBS)

bin_PROGRAMS = diptych

diptych_CPPFLAGS = -DGEN_EXIF
diptych_CXXFLAGS = $(AM_CXXFLAGS)
#diptych_LDFLAGS = 
#diptych_LDADD =
diptych_SOURCES = \
	Frame.cc \
	diptych.cc

