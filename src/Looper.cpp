#include "Looper.h"
#include "timetools.h"
#include <iostream>
#include "RtAudio.h"
#include <cmath>
#include "Thread.h"
#include "main.h"
#include <sstream>

using namespace std;
int bufPos, measPos;
int totLen, totMeasLen;
double toneFreq = 440.0;
double lev;

double Looper::metroVol;
Params Looper::params;
Phrase Looper::currentPhrase;
int Looper::sampRate;
Mutex Looper::threadLock;
bool Looper::overdubWrap;

int playRec(char *buffer, int numSamp, void *) {  

  Locker locker(Looper::threadLock);

  if(!Looper::params.playing && !Looper::params.priorRecording) {
    // play silence:
    float *myBuf = (float *) buffer;
    lev = 0.0;
    int curSampPos = Looper::params.sampPos;
    int sampPerMeasure = Looper::params.sampPerMeasure;
    int bpMeas = Looper::params.bpMeasure;
    int beepSampLen = (int) (BEEP_LEN * Looper::sampRate);
    for(int i = 0; i < numSamp; i++) {
      if(fabs(*myBuf) > lev) lev = fabs(*myBuf);

      if(curSampPos >= sampPerMeasure) {
	curSampPos = 0;
      }

      double measTime = ((float) curSampPos) / sampPerMeasure;
      
      measTime -= floor(measTime);
      int beatNum = (int) floor(measTime * bpMeas);
      int beatStartSamp = (int) (curSampPos - measTime * sampPerMeasure + ((int) (beatNum * sampPerMeasure / ((float) bpMeas))));
      
      // if we should be making a metronome beep :
      float outVal = 0.0;
      float freq = BEEP_FREQ;
      if(beatNum == 0) freq *= 2.0;
      if((curSampPos - beatStartSamp) < beepSampLen) {
	outVal += Looper::params.metroVol * sin(2.0 * PI * freq * curSampPos / Looper::sampRate);
      }

      *myBuf = outVal;
      myBuf++;
      curSampPos++;
    }
    Looper::params.sampPos = curSampPos;
    Looper::params.recLevel = Looper::params.recVolume * lev;
    return 0;
  }

  // add in the metronome track:
  float *myBuf = (float *) buffer;
  float measTime, outVal;
  int curSampPos = Looper::params.sampPos;
  int bpMeas = Looper::params.bpMeasure;
  int sampPerMeasure = Looper::params.sampPerMeasure;
  int beepSampLen = (int) (BEEP_LEN * Looper::sampRate);
  lev = 0.0;
  for(int i = 0; i < numSamp; i++) {
    if(fabs(*myBuf) > lev) lev = fabs(*myBuf);

    // if we're not recording, and not prior recording, and the sample position is beyond the end
    // of the current and old overdub tracks, then reset the position to
    // the beginning:
    if(!Looper::params.recording && !Looper::params.priorRecording && (curSampPos >= (int) Looper::currentPhrase.currentOverdub.size()) &&
       (curSampPos >= (int) Looper::currentPhrase.oldOverdub.size())) {
      curSampPos = 0;
    }

    measTime = ((float) curSampPos) / sampPerMeasure;
      
    measTime -= floor(measTime);
    int beatNum = (int) floor(measTime * bpMeas);
    int beatStartSamp = (int) (curSampPos - measTime * sampPerMeasure + ((int) (beatNum * sampPerMeasure / ((float) bpMeas))));
      
    // if we should be making a metronome beep :
    outVal = 0.0;
    float freq = BEEP_FREQ;
    if(beatNum == 0) freq *= 2.0;
    if((curSampPos - beatStartSamp) < beepSampLen) {
      outVal += Looper::params.metroVol * sin(2.0 * PI * freq * curSampPos / Looper::sampRate);
    }

    // if we're prior recording, then check to see if it's time to switch to recording:
    if(Looper::params.priorRecording) {
      *myBuf = outVal;
      curSampPos++;
      myBuf++;
      if(curSampPos == sampPerMeasure) {
	Looper::params.recording = true;
	Looper::params.priorRecording = false;
	curSampPos = 0;
      }
      continue;
    }

    // mix in the appropriate sample of the looped phrase:
    if(Looper::currentPhrase.phraseMeasures != 0) {
      int phraseSamp = curSampPos % (Looper::currentPhrase.phraseMeasures * sampPerMeasure);
      outVal += Looper::currentPhrase.phrase[phraseSamp];
    }

    // mix in the appropriate sample of the old overdub:
    if(curSampPos < (int) Looper::currentPhrase.oldOverdub.size()) {
      outVal += Looper::currentPhrase.oldOverdub[curSampPos];
    }

    // mix in the appropriate sample of the current (latest) overdub:
    if(curSampPos < (int) Looper::currentPhrase.currentOverdub.size()) {
      outVal += Looper::currentPhrase.currentOverdub[curSampPos];
    }


    // if we're recording, then save the sample into the current overdub buffer:
    if(Looper::params.recording) {
      // if the current sample position is beyond the end of the current
      // overdub buffer, then extend the overdub buffer to include another
      // measure:
      if(curSampPos >= (int) Looper::currentPhrase.currentOverdub.size()) {
	// if we're recording an overdub track, and we starrted the track in the latter
	// half of the last measure, then instead of extending the overdub track,
	// jump to the beginning of the track and record from there:
	if(Looper::overdubWrap) {
	  Log(string("wrapping now.\n"));
	  Looper::overdubWrap = false;
	  curSampPos = 0;
	}
	// otherwise extend the overdub track by a measure:
	else {
	  Looper::currentPhrase.currentOverdubMeasures++;
	  Looper::currentPhrase.currentOverdub.resize(Looper::currentPhrase.currentOverdubMeasures * sampPerMeasure, 0.0);
	}
      }
      Looper::currentPhrase.currentOverdub[curSampPos] = Looper::params.recVolume * (*myBuf);
    }

    if(outVal > 1.0) outVal = 1.0;
    else if(outVal < -1.0) outVal = -1.0;
    (*myBuf) = outVal;
    myBuf++;
    curSampPos++;
  }
  Looper::params.recLevel = Looper::params.recVolume * lev;
  Looper::params.sampPos = curSampPos;

  return 0;
}            


Looper::Looper(void) {
  done = 0;

  params.bpMin = 90;
  params.recVolume = 0.9;
  params.recLevel = 0.0;
  params.metroVol = 0.5;
  params.sampPos = 0;
  params.bpMeasure = 4;
  params.phraseNum = 0;
  params.priorRecording = false;
  params.recording = false;
  params.playing = false;
  params.leftButton = "start recording";
  params.rightButton = "---";
  params.status = "waiting...";

  phrases.resize(NUM_PHRASES);
  overdubWrap = false;
}

void Looper::Execute(void) {

  int channels = 1;
  sampRate = -1;
  int bufferSize = 2048;
  int nBuffers = 4;
  int device = 0;
  RtAudio *audio = 0;

  // determine the device parameters, and try to set appropraitely:
  try {
    audio = new RtAudio();
    int devices = audio->getDeviceCount();
    if(devices == 0) {
      //      cout << "No audio devices found!\n";
      exit(1);
    }

    RtAudioDeviceInfo info;
    for(int i = 1; i <= devices; i++) {
      info = audio->getDeviceInfo(i);
      //      cout << "i: " << i << "\nprobed? " << info.probed << "\nduplex channels: " << info.duplexChannels << "\ndefault? " << info.isDefault << endl;
      /*      if(info.probed && info.isDefault) {
	device = i;
	break;
	}*/
    }
    device = 1;
    info = audio->getDeviceInfo(device);

    if(info.duplexChannels == 0) {
      //      cout << "No duplex channels found!\n";
      exit(1);
    }

    for(int i = 0; i < (int) info.sampleRates.size(); i++) {
      //      cout << "sample rate: " << info.sampleRates[i] << endl;
      if(sampRate < 0) sampRate = info.sampleRates[i];
      if(abs(info.sampleRates[i] - DESIRED_SAMP_RATE) < abs(sampRate - DESIRED_SAMP_RATE)) {
	sampRate = info.sampleRates[i];
      }
    }
  }
  catch (RtError &error) {
    error.printMessage();
    exit(EXIT_FAILURE);
  }

  totLen = sampRate;
  totMeasLen = sampRate * 4;
  bufPos = 0;
  params.sampPerMeasure = (int) (sampRate / (params.bpMin / 60.0) * params.bpMeasure);


  // Open a stream during RtAudio instantiation
  try {
    audio = new RtAudio(device, channels, device, channels, RTAUDIO_FLOAT32,
                        sampRate, &bufferSize, nBuffers);
  }
  catch (RtError &error) {
    error.printMessage();
    exit(EXIT_FAILURE);
  }

  try {
    // Set the stream callback function
    audio->setStreamCallback(&playRec, NULL);
    AccuSleep(1.0);
    // Start the stream
    audio->startStream();
  }
  catch (RtError &error) {
    error.printMessage();
    goto cleanup;
  }

  //  std::cout << "\nRunning duplex ... press <enter> to quit.\n";
  while(!done) {
    AccuSleep(0.05);
  }
  //  cout << "done!\n";

  try {
    // Stop and close the stream
    audio->stopStream();
    audio->closeStream();
  }
  catch (RtError &error) {
    error.printMessage();
  }

 cleanup:
  delete audio;

}

void Looper::inChar(char inch) {

  if(inch == 'x') 
    done = 1;

  //  cout << inch << endl;
}

void Looper::getParams(Params &outParams) {

  params.measurePos = (double) params.sampPos / params.sampPerMeasure;
  params.measurePos -= floor(params.measurePos);
  params.beatPos = params.measurePos * params.bpMeasure;
  params.beatPos -= floor(params.beatPos);
  if(params.recording) {
    params.status = "recording";
    params.leftButton = "stop recording";
  }
  else if(params.priorRecording) {
    params.status = "prior recording";
    params.leftButton = "stop recording";
  }
  else if(params.playing) {
    params.status = "playing";
    params.leftButton = "start recording";
  }
  else {
    params.status = "waiting...";
    params.leftButton = "start recording";
  }
  if(params.playing && !params.recording && !params.priorRecording) {
    params.rightButton = "stop playing";
  }
  else if(!params.playing && !currentPhrase.erased) {
    params.rightButton = "start playing";
  }
  else {
    params.rightButton = "---";
  }
  outParams = params;

}

void Looper::metroVolume(double x) {
  
  if((x >= 0.0) && (x <= 1.0))
    params.metroVol = x;
}

void Looper::metronomeQuieter(void) {


  if(params.metroVol >= 0.1) {
    Log(string("making metronome quieter.\n"));
    params.metroVol -= 0.1;
  }

}

void Looper::metronomeLouder(void) {
  
  if(params.metroVol <= 0.9) {
    Log(string("making metronome quieter.\n"));
    params.metroVol += 0.1;
  }

}

void Looper::setMeter(int newMeter) {

  if((params.priorRecording == false) && (params.recording == false) && (params.playing == false) &&
     (phrases[params.phraseNum].erased == true)) {
    if((newMeter >= MIN_BPMEASURE) && (newMeter <= MAX_BPMEASURE)) {
      stringstream ostr;
      ostr << "changing beats per measure to " << newMeter << "\n";
      Log(ostr.str());
      params.bpMeasure = newMeter;
      //      phrases[params.phraseNum].bpMeasure = newMeter;
      params.sampPerMeasure = (int) (sampRate / (params.bpMin / 60.0) * params.bpMeasure);
    }
  }

}

void Looper::changeMeter(int meterChange) {

  int newMeter = params.bpMeasure + meterChange;
  setMeter(newMeter);

}

void Looper::changeTempoDelta(double tempoChange) {

  changeTempo(params.bpMin + tempoChange);
}

void Looper::changeTempo(double newTempo) {

  if(!params.priorRecording && (params.recording == false) && (params.playing == false) &&
     (phrases[params.phraseNum].erased == true)) {
    if((newTempo >= MIN_TEMPO) && (newTempo <= MAX_TEMPO)) {
      stringstream ostr;
      ostr << "changing beats per minute to " << newTempo << "\n";
      Log(ostr.str());
      params.bpMin = newTempo;
      params.sampPerMeasure = (int) (sampRate / (params.bpMin / 60.0) * params.bpMeasure);
      rewind();
    }
  }
}

void Looper::setRecordVol(double vol) {

  params.recVolume = vol;
}

void Looper::changeRecordVol(double volChange) {

  double newVol = params.recVolume + volChange;
  stringstream ostr2;
  ostr2 << "want to change record volume to " << newVol << "\n";
  Log(ostr2.str());
  if((newVol >= 0.0) && (newVol <= 2.0)) {
    stringstream ostr;
    ostr << "changing record volume to " << newVol << "\n";
    Log(ostr.str());
    params.recVolume = newVol;
  }

}


void Looper::erasePhrase(void) {

  stopRecording();
  stopPlaying();

  phrases[params.phraseNum].erasePhrase();
  currentPhrase = phrases[params.phraseNum];
}

void Looper::savePhrase(void) {
  
  if(params.recording) {
    stopRecording();
  }
  currentPhrase.mixdown();
  phrases[params.phraseNum] = currentPhrase;
}

void Looper::changePhrase(int phraseDelta) {

  stopRecording();
  stopPlaying();

  params.phraseNum = (params.phraseNum + phraseDelta) % NUM_PHRASES;
  if(params.phraseNum < 0) params.phraseNum += NUM_PHRASES;
  stringstream ostr;
  ostr << "Setting current phrase to phrase #" << params.phraseNum << "...\n";
  Log(ostr.str());
  currentPhrase = phrases[params.phraseNum];

  if(params.playing == false) {
    if(phrases[params.phraseNum].erased == false) {
      params.rightButton = "start playing";
    }
    else {
      params.rightButton = "---";
    }
  }
}

void Looper::deleteLastDub(void) {
  
  Locker locker(threadLock);
  if(params.recording) {
    stopRecording();
  }

  rewind();
  currentPhrase.deleteLastDub();

}

void Looper::deleteAllDubs(void) {

  Locker locker(threadLock);  
  if(params.recording) {
    stopRecording();
  }

  rewind();
  currentPhrase.deleteAllDubs();

}


void Looper::rightButton(void) {

  Log(string("right button pushed.\n"));
  if(currentPhrase.erased || params.recording)
    return;

  if(params.playing) {
    stopPlaying();
    params.rightButton = "start playing";
  }
  else {
    rewind();
    startPlaying();
    params.rightButton = "stop playing";
  }
}

void Looper::leftButton(void) {

  Log(string("left button pushed\n"));
  if(currentPhrase.erased && !params.recording && !params.priorRecording) {
    Log(string("in first\n"));
    params.leftButton = "stop recording";
    rewind();
    startRecording();
    return;
  }
   
  if(params.recording || params.priorRecording) {
    Log(string("in second\n"));
    params.leftButton = "start recording";
    params.rightButton = "stop playing";
    stopRecording();
    return;
  }

  Log(string("in thrid\n"));
  params.leftButton = "stop recording";
  params.rightButton = "---";
  startRecording();
}

void Looper::startPlaying(void) {

  Locker locker(threadLock);
  Log(string("starting playing\n"));
  params.playing = true;
}

void Looper::stopPlaying(void) {

  Locker locker(threadLock);
  Log(string("stopping playing\n"));
  params.playing = false;
  rewind();
}

void Looper::startRecording(void) {

  Locker locker(threadLock);
  Log(string("starting recording\n"));
  currentPhrase.mixdown();
  if(!params.playing)
    params.priorRecording = true;
  else {
    if(!currentPhrase.erased && ((currentPhrase.currentOverdubMeasures * params.sampPerMeasure - params.sampPos) / (params.sampPerMeasure + 0.0001) < 0.5)) {
      Log(string("setting overdubWrap to true\n"));
      overdubWrap = true;
    }
    params.recording = true;
  }
  startPlaying();
}

void Looper::stopRecording(void) {

  Locker locker(threadLock);
  Log(string("stopping recording\n"));

  if(params.recording) {
    // round the current overdub track to the nearest measure length:
    currentPhrase.measurize(params.sampPerMeasure);
    if(currentPhrase.erased)
      currentPhrase.firstRecord();
    else Log(string("somehow not erased!"));
  }
  else 
    stopPlaying();

  params.recording = false;
  params.priorRecording = false;

}

void Looper::rewind(void) {

  Locker locker(threadLock);
  params.sampPos = 0;
}

void Looper::tempoTap(void) {

  tempoTimes.push_back(AccuTime());
  if(tempoTimes.size() > 4) {
    tempoTimes.erase(tempoTimes.begin(), tempoTimes.begin() + 1);
  }
  if(tempoTimes.size() < 4)
    return;
  if((AccuTime() - tempoTimes[0]) > 30.0)
    return;

  double newTempo = (int) (60.0 / ((tempoTimes[3] - tempoTimes[0]) / 3.0));
  changeTempo(newTempo);
}

void Phrase::mixdown(void) {

  oldOverdub.resize(currentOverdub.size(), 0.0);
  for(int i = 0; i < (int) oldOverdub.size(); i++) {
    oldOverdub[i] += currentOverdub[i];
    if(oldOverdub[i] > 1.0) oldOverdub[i] = 1.0;
    else if(oldOverdub[i] < -1.0) oldOverdub[i] = -1.0;
    currentOverdub[i] = 0.0;
  }
}

void Phrase::deleteLastDub(void) {

  currentOverdub.resize(0);
  currentOverdub.resize(oldOverdub.size(), 0.0);
}

void Phrase::deleteAllDubs(void) {

  currentOverdub.resize(0);
  currentOverdub.resize(phrase.size(), 0.0);
  oldOverdub.resize(0);
  oldOverdub.resize(phrase.size(), 0.0);
}

void Phrase::measurize(int sampPerMeas) {

  int numMeas = (int) floor(((double) Looper::params.sampPos) / sampPerMeas + 0.5);
  int numSamp = numMeas * sampPerMeas;
  if(numSamp > (int) currentOverdub.size()) {
    currentOverdub.resize(numSamp, 0.0);
  }
  else {
    Log(string("chopping down the overdub a little.\n"));
    stringstream logstr;
    logstr << "sampPos was: " << Looper::params.sampPos << "\n";
    if(Looper::params.sampPos >= numSamp)
      Looper::params.sampPos = Looper::params.sampPos % sampPerMeas;
    logstr << "sampPos now: " << Looper::params.sampPos << ", sampPerMeas: " << sampPerMeas << ", length of curoverdub: " << currentOverdub.size() << ", numsamp: " << numSamp << "\n";
    Log(logstr.str());
    currentOverdub.resize(numSamp);
  }
  currentOverdubMeasures = numMeas;
  stringstream ostr;
  ostr << "measurizing, new measure length = " << numMeas << "\n";
  Log(ostr.str());
  
}

Phrase::Phrase(void) {

  phraseMeasures = oldOverdubMeasures = currentOverdubMeasures = 0;
  erased = true;
}

void Phrase::erasePhrase(void) {

  Log(string("erasing phrase\n"));
  erased = true;
  phrase.resize(0);
  oldOverdub.resize(0);
  currentOverdub.resize(0); 
  phraseMeasures = oldOverdubMeasures = currentOverdubMeasures = 0;
}

void Phrase::firstRecord(void) {

  Log(string("Doing first record.\n"));  
  phraseMeasures = currentOverdubMeasures;
  phrase = currentOverdub;
  currentOverdub.resize(0);
  currentOverdub.resize(phrase.size(), 0.0);
  if(phrase.size() != 0)
    erased = false;
}

