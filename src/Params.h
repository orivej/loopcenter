#ifndef PARAMS_H
#define PARAMS_H

#include <string>

using namespace std;

class Params {

 public:
  double bpMin, recVolume, recLevel, beatPos, measurePos, metroVol;
  int sampPos, bpMeasure, sampPerMeasure, phraseNum;
  bool priorRecording, recording, playing;
  string leftButton, rightButton, status;

};

#endif // PARAMS_H
