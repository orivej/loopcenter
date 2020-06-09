#ifndef LOOPER_H
#define LOOPER_H

#include "Thread.h"
#include "Params.h"
#include <vector>

const double BEEP_FREQ = 440.0;
const double BEEP_LEN = 0.1;
const double PI = 3.1415926535;
const int MIN_BPMEASURE = 1;
const int MAX_BPMEASURE = 32;
const double MIN_TEMPO = 10;
const double MAX_TEMPO = 250;
const int NUM_PHRASES = 10;
const int DESIRED_SAMP_RATE = 44100;

class Phrase {

  double tempo;
  int framesPerMeasure;

 public:
  Phrase(void);
  vector<float> phrase, oldOverdub, currentOverdub;
  void erasePhrase(void);
  void mixdown(void);
  void deleteLastDub(void);
  void deleteAllDubs(void);
  void measurize(int sampPerMeas);
  void firstRecord(void);

  int phraseMeasures, oldOverdubMeasures, currentOverdubMeasures;
  bool erased;

};

class Looper : public Thread {

  int done;
  vector<double> tempoTimes;

 protected:
  void Execute(void);

 public:
  vector<Phrase> phrases;
  static int sampRate;
  static Phrase currentPhrase;
  static Params params;
  static double metroVol;
  static Mutex threadLock;
  static bool overdubWrap;
  Looper(void);
  void inChar(char inch);
  void getParams(Params &outParams);
  void metroVolume(double x);
  void metronomeQuieter(void);
  void metronomeLouder(void);
  void changeRecordVol(double volChange);
  void setRecordVol(double vol);
  void changeTempo(double newTempo);
  void changeTempoDelta(double tempoChange);
  void changeMeter(int meterChange);
  void setMeter(int newMeter);
  void erasePhrase(void);
  void savePhrase(void);
  void setPhrase(int newPhrase);
  void changePhrase(int phraseDelta);
  void deleteLastDub(void);
  void deleteAllDubs(void);
  void rightButton(void);
  void leftButton(void);
  void startPlaying(void);
  void stopPlaying(void);
  void startRecording(void);
  void stopRecording(void);
  void rewind(void);
  void tempoTap(void);


};

#endif // LOOPER_H
