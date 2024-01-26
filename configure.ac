AC_PREREQ([2.69])
AC_INIT([diptych],[0.4.0],[https://github.com/whatdoineed2do/diptych/issues],[https://github.com/whatdoineed2do/diptych],[whatdoineed2do@nospam-github.com])

AM_INIT_AUTOMAKE([dist-bzip2 no-dist-gzip])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])
AC_CONFIG_SRCDIR([src])
AC_CONFIG_HEADERS([config.h])

AC_PROG_CXX
AC_PROG_INSTALL
PKG_PROG_PKG_CONFIG

AX_CXX_COMPILE_STDCXX([17], [noext], [mandatory])

PKG_CHECK_MODULES(IMAGEMAGICK, [Magick++ >= 6.9.11])
PKG_CHECK_EXISTS([Magick++ < 7.0.0],
                 [AC_DEFINE([HAVE_IM_RESIZE_FILTERTYPES], 1,
                            [Defined if IM resize filter enum type change])])


AC_ARG_ENABLE(exiv2,
        AS_HELP_STRING([--disable-exiv2],[do not generate Exif]),
        [ use_exiv2="${enableval}" ], [ use_exiv2=yes ] )
if test "x$use_exiv2" != xno ; then
        PKG_CHECK_MODULES(EXIV2, [exiv2 >= 0.25], [ use_exiv2=yes ], [ use_exiv2=no ])
        AH_TEMPLATE([HAVE_EXIV2], [generate Exif where available])
        AS_IF([test "x$use_exiv2" = "xyes"], [
                AC_DEFINE([HAVE_EXIV2])
                AC_SUBST(EXIV2_CFLAGS)
                AC_SUBST(EXIV2_LIBS)
           ])
fi


AC_ARG_ENABLE(debug,
        AS_HELP_STRING([--enable-debug],[enable debug code (default is no)]),
        [ debug="${enableval}" ], [ debug=no ] )
if test "x$debug" = xno; then
        AC_DEFINE(NDEBUG,1,[debug code])
fi

AC_CONFIG_FILES([
Makefile
src/Makefile
])
AC_OUTPUT
