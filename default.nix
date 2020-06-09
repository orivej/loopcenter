{ lib, stdenv, fetchurl, autoreconfHook, pkgconfig, fltk, libjack2, libjpeg, rtaudio }:

stdenv.mkDerivation rec {
  name = "loopcenter-unstable-0";

  src = lib.cleanSource ./.;

  nativeBuildInputs = [ autoreconfHook pkgconfig ];

  buildInputs = [ fltk libjack2 libjpeg rtaudio ];

  enableParallelBuilding = true;

  NIX_CFLAGS_COMPILE = [ "-Wall" "-Werror" ];

  meta = with lib; {
    homepage = "https://github.com/orivej/loopcenter";
    description = "Easily record loops, play and overdub them, as with loop pedals";
    license = licenses.gpl2;
    maintainers = with maintainers; [ orivej ];
    platforms = platforms.linux;
  };
}
