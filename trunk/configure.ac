#----------------------------------------------------------------------------
# Autoconf input script. Start the ./autgen.sh script to create
# configure script from this.
#----------------------------------------------------------------------------
AC_PREREQ([2.54])

AC_INIT([windstille], [0.5], [matze@braunis.de])
AC_CONFIG_SRCDIR([mk/jam/build.jam])
AC_CONFIG_AUX_DIR([mk/autoconf])

AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CANONICAL_TARGET

#----------------------------------------------------------------------------
# Configuration header
#----------------------------------------------------------------------------
AC_CONFIG_HEADERS(config.h)

# stupid autoconf is adding default -g -O2 flags when we don't want this :-/
test ".$CXXFLAGS" = "." && CXXFLAGS=" "
test ".$CFLAGS" = "." && CFLAGS=" "

#----------------------------------------------------------------------------
# Check for build variant (debug, profile, optimize)
#----------------------------------------------------------------------------
VARIANT=optimize
AC_ARG_ENABLE([optimize], [AC_HELP_STRING([--enable-optimize],
    [build with optimizations enabled (default YES)])],
    [test "$enableval" = "yes" && VARIANT=optimize])
                                                                                
AC_ARG_ENABLE([debug], [AC_HELP_STRING([--enable-debug],
    [build with debugging information (default NO)])],
    [test "$enableval" = "yes" && VARIANT=debug])
                                                                                
AC_ARG_ENABLE([profile], [AC_HELP_STRING([--enable-profile],
    [build with profiling information (default NO)])],
    [test "$enableval" = "yes" && VARIANT=profile])
AC_SUBST([VARIANT])

#----------------------------------------------------------------------------
# find applications
#----------------------------------------------------------------------------
AC_PROG_CXX
AC_PROG_INSTALL
AC_CHECK_PROGS([XGETTEXT], [xgettext xgettext.exe])

# bison + flex
AC_PROG_LEX
LEX_LIBS=$LEXLIB
AC_SUBST([LEX_LIBS])
AC_CHECK_PROGS([BISON], [bison bison.exe])
AC_SUBST([BISON])

# check if our c++ compiler is gcc3.2 or newer
AC_MSG_CHECKING([whether g++ is new enough])
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
AC_TRY_COMPILE([], [
#ifndef __GNUC__
#error no gcc compiler found (__GNUC__ not defined)
#endif
#if __GNUC__ < 3 || (__GNUC__ == 3 && __GNUC__ < 2)
#error gcc compiler too old. Need at least version 3.2.0
#endif
], [AC_MSG_RESULT([yes])], [
	AC_MSG_RESULT([no])
	AC_MSG_ERROR([
Error: Your gcc version is too old. Windstille needs at least gcc 3.2.0.
])])
AC_LANG_RESTORE

#----------------------------------------------------------------------------
# check headers
#----------------------------------------------------------------------------
AC_HEADER_DIRENT
AC_HEADER_TIME
AC_HEADER_STDC

AM_ICONV
AC_SUBST([ICONV_LIBS], [$LIBICONV])

#----------------------------------------------------------------------------
# find libraries
#----------------------------------------------------------------------------
AM_BINRELOC

AX_CHECK_GL
if test "$no_gl" = "yes"; then
    AC_MSG_ERROR([Couldn't find opengl libraries or headers])
fi

PKG_CHECK_MODULES([CLANLIB], [clanApp-0.7 clanVorbis-0.7 clanCore-0.7 clanDisplay-0.7 clanGL-0.7 clanSignals-0.7 clanGUI-0.7 clanSound-0.7])

XIPH_PATH_VORBIS(, [AC_MSG_ERROR([Please install libvorbis])])
XIPH_PATH_OGG(, [AC_MSG_ERROR([Please install libogg])])

NP_FINDLIB([OPENAL], [OpenAL], [OpenAL],
         NP_LANG_PROGRAM([#include <AL/al.h>],
           [alcOpenDevice(0);]),
         [], [-lopenal],
         [],
         [AC_MSG_ERROR([Please intall OpenAL])],
         [], [])

NP_FINDLIB([PHYSFS], [physfs], [physfs >= 1.0.0],
        NP_LANG_PROGRAM([#include <physfs.h>
#if PHYSFS_VER_MAJOR < 1
# error PHYSFS is too old
#endif]),
        [], [-lphysfs],
        [],
        [AC_MSG_ERROR([Please install physfs >= 1.0])])

AC_INIT_JAM
AC_CONFIG_FILES([Jamconfig])

# We have a config.h file (binreloc needs this define)
CPPFLAGS="$CPPFLAGS -DHAVE_CONFIG_H"

AC_OUTPUT

AC_MSG_NOTICE([

Notice: This project uses jam (and not make) as build tool.
])
