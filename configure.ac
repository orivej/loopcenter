AC_INIT(jlfltk.cpp)

AM_INIT_AUTOMAKE(loopcenter,0.1)

AC_PROG_CC

AC_PROG_CXX

AC_PROG_INSTALL

AC_CONFIG_SRCDIR(src/jlfltk.cpp)

dnl Checks for libraries.
AC_CHECK_LIB(fltk,numericsort,,
  AC_CHECK_LIB(fltk,fl_numericsort,,
  AC_MSG_ERROR("missing fltk"))
)

AC_CHECK_LIB([pthread], [pthread_self], , [AC_MSG_ERROR(You need libpthread installed)])

# Check for JACK
PKG_CHECK_MODULES(JACK,jack)
AC_SUBST(JACK_CFLAGS)
AC_SUBST(JACK_LIBS)


AC_OUTPUT(Makefile src/Makefile)
