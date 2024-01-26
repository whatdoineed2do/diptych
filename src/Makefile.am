ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

AM_CXXFLAGS = $(IMAGEMAGICK_CFLAGS) $(EXIV2_CFLAGS) -Wno-conversion-null -Wno-deprecated
AM_LDFLAGS  = $(IMAGEMAGICK_LIBS) $(EXIV2_LIBS)

bin_PROGRAMS = diptych

diptych_CPPFLAGS = 
diptych_CXXFLAGS = $(AM_CXXFLAGS)
diptych_LDFLAGS = 
diptych_LDADD = $(AM_LDFLAGS)
diptych_SOURCES = \
	Frame.cc \
	diptych.cc

