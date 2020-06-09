INCLUDES = -D__LINUX_JACK__ 
AM_CFLAGS = -g -Wall $(JACK_CFLAGS) 
AM_LDFLAGS = $(JACK_LIBS)

bin_PROGRAMS = loopcenter
loopcenter_SOURCES = jlfltk.cpp RtAudio.cpp  Thread.cpp Looper.cpp timetools.cpp

noinst_HEADERS = Looper.h timetools.h jlfltk.h main.h RtAudio.h Thread.h Params.h  RtError.h

