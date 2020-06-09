AC_INIT(jlfltk.cpp)

AM_INIT_AUTOMAKE(loopcenter,0.1.2)

AC_PROG_CC

AC_PROG_CXX

AC_PROG_INSTALL

AC_CONFIG_SRCDIR(src/jlfltk.cpp)

# Check for FLTK...
AC_PATH_PROG(FLTKCONFIG,fltk-config)

if test "x$FLTKCONFIG" = x; then
	# Use old method for FLTK < 1.1.x
	# Search for static library 'libfltk.a':
	AC_PATH_PROG(LIBFLTKA,libfltk.a,-lfltk,/lib:/usr/lib:/usr/X11R6/lib:/usr/local/lib:/usr/local/lib/X11)
	# Add static FLTK lib if libfltk.a found otherwise dynamic FLTK:
	AC_CHECK_LIB(fltk, main,LIBS="$LIBFLTKA $LIBS",,$LIBS)
	# Replace `main' with a function in -lfltk:
	dnl AC_CHECK_LIB(fltk, main,,,$LIBS)
else
        CFLAGS="`$FLTKCONFIG --cflags` $CFLAGS"
        CXXFLAGS="`$FLTKCONFIG --cflags` $CXXFLAGS"
        LIBS="`$FLTKCONFIG --use-images --ldflags` $LIBS"

	AC_SUBST(FLTKCONFIG)

	jpeg=`$FLTKCONFIG --use-images --ldflags | grep jpeg`
	if test "x$jpeg" != x; then
        	AC_DEFINE(HAVE_LIBJPEG)
	fi

	png=`$FLTKCONFIG --use-images --ldflags | grep png`
	if test "x$png" != x; then
        	AC_DEFINE(HAVE_LIBPNG)
	fi
fi

AC_CHECK_LIB([pthread], [pthread_self], , [AC_MSG_ERROR(You need libpthread installed)])

# Check for JACK
PKG_CHECK_MODULES(JACK,jack)
AC_SUBST(JACK_CFLAGS)
AC_SUBST(JACK_LIBS)


AC_OUTPUT(Makefile src/Makefile)
