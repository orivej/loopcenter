INCLUDES = -D__LINUX_JACK__ 
AM_CXXFLAGS = -g -Wall $(JACK_CFLAGS) $(RTAUDIO_CFLAGS)
AM_LDFLAGS = $(JACK_LIBS) $(RTAUDIO_LIBS)

bin_PROGRAMS = loopcenter
loopcenter_SOURCES = jlfltk.cpp Thread.cpp Looper.cpp timetools.cpp

noinst_HEADERS = Looper.h timetools.h jlfltk.h main.h Thread.h Params.h

