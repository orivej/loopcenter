// generated by Fast Light User Interface Designer (fluid) version 1.0108
#include "Looper.h"
#include "jlfltk.h"
#include <iostream>
#include "Thread.h"
#include <fstream>
#include "timetools.h"
#include "Params.h"

Mutex logMutex;
using namespace std;
Looper *looper;
LooperUI *looperUI;

void Log(const string& inStr) {
  Locker locker(logMutex);

  /*  ofstream file;
  file.open("logfile.txt", ios::app);
  file << inStr;
  file.close();*/

}

void Updater::Execute(void) {

  while(true) {
    AccuSleep(0.05);
    looperUI->updateStuff();
  }

}

void LooperUI::updateStuff(void) {

  Fl::lock();
  Params par;
  looperUI->theLooper->getParams(par);
  looperUI->recordLevel->value(par.recLevel * 100.0);
  looperUI->measurePosition->value(par.measurePos * 100.0);
  looperUI->measureNum->value(par.sampPos / par.sampPerMeasure + 1);
  string tmpstr = "Status: ";
  tmpstr += par.status;
  looperUI->status->copy_label(tmpstr.c_str());
  Fl::unlock();
  Fl::awake(this->LoopCenter);
}

Key_Window::Key_Window(int x, int y, const char *lab) : Fl_Double_Window(x, y, lab) {}

int Key_Window::handle(int event) {

  char ch;
  switch(event) {
  case FL_FOCUS:
    //    cout << "focus\n";
    return 1;
  case FL_UNFOCUS:
    //    cout << "unfocus\n";
    return 1;
  case FL_KEYDOWN:
    ch = Fl::event_key();
    Log(string("recieved key '") + ch + "'\n");
    if(Fl::event_key() == '[') {
      theLooper->metronomeQuieter();
      metroVol->value(theLooper->params.metroVol * 100.0);
    }
    else if(Fl::event_key() == ']') {
      theLooper->metronomeLouder();
      metroVol->value(theLooper->params.metroVol * 100.0);
    }
    else if(Fl::event_key() == FL_Up) {
      theLooper->changeTempoDelta(1.0);
      metroTempo->value((int) theLooper->params.bpMin);
    }
    else if(Fl::event_key() == FL_Down) {
      theLooper->changeTempoDelta(-1.0);
      metroTempo->value((int) theLooper->params.bpMin);
    }
    else if(Fl::event_key() == '-') {
      theLooper->changeRecordVol(-0.05);
      recordVol->value(theLooper->params.recVolume * 100.0);
    }
    else if(Fl::event_key() == '=') {
      theLooper->changeRecordVol(0.05);
      recordVol->value(theLooper->params.recVolume * 100.0);
    }
    else if(Fl::event_key() == 'a') {
      theLooper->changeMeter(1);
      bpMeasure->value(theLooper->params.bpMeasure);
    }
    else if(Fl::event_key() == 'z') {
      theLooper->changeMeter(-1);
      bpMeasure->value(theLooper->params.bpMeasure);
    }
    else if(Fl::event_key() == 'e') {
      theLooper->erasePhrase();
    }
    else if(Fl::event_key() == 's') {
      theLooper->savePhrase();
    }
    else if(Fl::event_key() == 'd') {
      theLooper->deleteAllDubs();
    }
    else if(Fl::event_key() == 'k') {
      theLooper->deleteLastDub();
    }
    else if(Fl::event_key() == 't') {
      theLooper->tempoTap();
    }
    else if(Fl::event_key() == FL_Left) {
      theLooper->changePhrase(-1);
    }
    else if(Fl::event_key() == FL_Right) {
      theLooper->changePhrase(1);
    }
    else if(Fl::event_key() == 32) {
      theLooper->leftButton();
    }
    else if(Fl::event_key() == FL_Enter) {
      theLooper->rightButton();
    }
    this->setButtons();
    return 1;
  default:
    return Fl_Double_Window::handle(event);
  }
}

void LooperUI::cb_metroTempo_i(Fl_Spinner*, void*) {
  theLooper->changeTempo(metroTempo->value());
  LoopCenter->setButtons();
}
void LooperUI::cb_metroTempo(Fl_Spinner* o, void* v) {
  ((LooperUI*)(o->parent()->user_data()))->cb_metroTempo_i(o,v);
}

void LooperUI::cb_metroVol_i(Fl_Slider*, void*) {
  theLooper->metroVolume(metroVol->value() / 100.0);
  LoopCenter->setButtons();
}
void LooperUI::cb_metroVol(Fl_Slider* o, void* v) {
  ((LooperUI*)(o->parent()->user_data()))->cb_metroVol_i(o,v);
}

void LooperUI::cb_bpMeasure_i(Fl_Spinner*, void*) {
  theLooper->setMeter((int) bpMeasure->value());
  LoopCenter->setButtons();
}
void LooperUI::cb_bpMeasure(Fl_Spinner* o, void* v) {
  ((LooperUI*)(o->parent()->user_data()))->cb_bpMeasure_i(o,v);
}

void LooperUI::cb_phraseSetter_i(Fl_Spinner*, void*) {
  theLooper->setPhrase((int) phraseSetter->value() - 1);
  LoopCenter->setButtons();
}
void LooperUI::cb_phraseSetter(Fl_Spinner* o, void* v) {
  ((LooperUI*)(o->parent()->user_data()))->cb_phraseSetter_i(o,v);
}

void LooperUI::cb_phraseSaver_i(Fl_Button*, void*) {
  theLooper->savePhrase();
  LoopCenter->setButtons();
}
void LooperUI::cb_phraseSaver(Fl_Button* o, void* v) {
  ((LooperUI*)(o->parent()->user_data()))->cb_phraseSaver_i(o,v);
}

void Key_Window::setButtons(void) {

  Fl::lock();
  Params par;
  theLooper->getParams(par);
  string tmpstr = par.leftButton + " (spacebar)";
  this->leftButton->copy_label(tmpstr.c_str());
  this->leftButton->value((int) (par.recording || par.priorRecording));
  tmpstr = par.rightButton + " (enter)";
  this->rightButton->copy_label(tmpstr.c_str());
  this->rightButton->value((int) par.playing);
  this->metroTempo->value((int) par.bpMin);
  this->phraseSetter->value(par.phraseNum + 1);
  if(!par.priorRecording && (par.recording == false) && (par.playing == false) &&
     (theLooper->phrases[par.phraseNum].erased == true)) {
    this->tempoTap->activate();
    this->metroTempo->activate();
    this->bpMeasure->activate();
  }
  else {
    this->tempoTap->deactivate();
    this->metroTempo->deactivate();
    this->bpMeasure->deactivate();
  }
  Fl::unlock();
}

void LooperUI::cb_leftButton_i(Fl_Light_Button*, void*) {
  theLooper->leftButton();
  LoopCenter->setButtons();

}
void LooperUI::cb_leftButton(Fl_Light_Button* o, void* v) {
  ((LooperUI*)(o->parent()->user_data()))->cb_leftButton_i(o,v);
}

void LooperUI::cb_rightButton_i(Fl_Light_Button*, void*) {
  theLooper->rightButton();
  LoopCenter->setButtons();
}
void LooperUI::cb_rightButton(Fl_Light_Button* o, void* v) {
  ((LooperUI*)(o->parent()->user_data()))->cb_rightButton_i(o,v);
}

void LooperUI::cb_recordVol_i(Fl_Slider*, void*) {
  theLooper->setRecordVol(recordVol->value() / 100.0);
  LoopCenter->setButtons();
}
void LooperUI::cb_recordVol(Fl_Slider* o, void* v) {
  ((LooperUI*)(o->parent()->user_data()))->cb_recordVol_i(o,v);
}

void LooperUI::cb_deleteLastDub_i(Fl_Button*, void*) {
  theLooper->deleteLastDub();
  LoopCenter->setButtons();
}
void LooperUI::cb_deleteLastDub(Fl_Button* o, void* v) {
  ((LooperUI*)(o->parent()->user_data()))->cb_deleteLastDub_i(o,v);
}

void LooperUI::cb_deleteAllDubs_i(Fl_Button*, void*) {
  theLooper->deleteAllDubs();
  LoopCenter->setButtons();
}
void LooperUI::cb_deleteAllDubs(Fl_Button* o, void* v) {
  ((LooperUI*)(o->parent()->user_data()))->cb_deleteAllDubs_i(o,v);
}

void LooperUI::cb_tempoTapper_i(Fl_Button*, void*) {
  theLooper->tempoTap();
  LoopCenter->setButtons();
}
void LooperUI::cb_tempoTapper(Fl_Button* o, void* v) {
  ((LooperUI*)(o->parent()->user_data()))->cb_tempoTapper_i(o,v);
}

void LooperUI::cb_phraseEraser_i(Fl_Button*, void*) {
  theLooper->erasePhrase();
  LoopCenter->setButtons();
}
void LooperUI::cb_phraseEraser(Fl_Button* o, void* v) {
  ((LooperUI*)(o->parent()->user_data()))->cb_phraseEraser_i(o,v);
}

LooperUI::LooperUI(Looper *inLooper) {

  theLooper = inLooper;
  { LoopCenter = new Key_Window(680, 350, "LoopCenter");
  LoopCenter->theLooper = theLooper;
    LoopCenter->user_data((void*)(this));
    LoopCenter->align(FL_ALIGN_LEFT);
    { metroTempo = new Fl_Spinner(330, 10, 55, 25, "Metronome tempo (bpm) (up arr./down arr.):");
      metroTempo->minimum(10);
      metroTempo->maximum(250);
      metroTempo->value(90);
      metroTempo->callback((Fl_Callback*)cb_metroTempo);
    } // Fl_Spinner* metroTempo
    { metroVol = new Fl_Slider(500, 10, 150, 25, "volume ( [ / ] ):");
      metroVol->type(1);
      metroVol->maximum(100);
      metroVol->value(50);
      metroVol->callback((Fl_Callback*)cb_metroVol);
      metroVol->align(FL_ALIGN_LEFT);
    } // Fl_Slider* metroVol
    { bpMeasure = new Fl_Spinner(330, 43, 55, 25, "Beats per measure ( a / z ):");
      bpMeasure->maximum(32);
      bpMeasure->value(4);
      bpMeasure->callback((Fl_Callback*)cb_bpMeasure);
    } // Fl_Spinner* bpMeasure
    { phraseSetter = new Fl_Spinner(250, 87, 50, 25, "select phrase (left arr./right arr.):");
      phraseSetter->maximum(10);
      phraseSetter->value(1);
      phraseSetter->callback((Fl_Callback*)cb_phraseSetter);
    } // Fl_Spinner* phraseSetter
    { phraseSaver = new Fl_Button(330, 87, 140, 25, "save current ('s')");
      phraseSaver->callback((Fl_Callback*)cb_phraseSaver);
    } // Fl_Button* phraseSaver
    { leftButton = new Fl_Light_Button(180, 123, 220, 50, "Record");
      leftButton->callback((Fl_Callback*)cb_leftButton);
      leftButton->align(FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    } // Fl_Light_Button* leftButton
    { rightButton = new Fl_Light_Button(430, 123, 220, 50, "Play");
      rightButton->callback((Fl_Callback*)cb_rightButton);
      rightButton->align(FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
    } // Fl_Light_Button* rightButton
    { measureNum = new Fl_Value_Output(180, 320, 45, 20, "Measure num:");
    } // Fl_Value_Output* measureNum
    { status = new Fl_Box(300, 320, 180, 20, "Status:");
    status->align(FL_ALIGN_INSIDE | FL_ALIGN_LEFT);
    }
    { recordVol = new Fl_Slider(180, 228, 220, 20, "Record volume ('-'/'='):");
      recordVol->type(1);
      recordVol->maximum(200);
      recordVol->step(1);
      recordVol->value(90);
      recordVol->callback((Fl_Callback*)cb_recordVol);
      recordVol->align(FL_ALIGN_LEFT);
    } // Fl_Slider* recordVol
    { recordLevel = new Fl_Progress(180, 263, 470, 20, "Record level:       ");
      recordLevel->selection_color(FL_GREEN);
      recordLevel->align(FL_ALIGN_LEFT);
    } // Fl_Progress* recordLevel
    { measurePosition = new Fl_Progress(180, 288, 470, 20, "Measure position:");
      measurePosition->selection_color((Fl_Color)176);
      measurePosition->align(FL_ALIGN_LEFT);
    } // Fl_Progress* measurePosition
    { deleteLastDub = new Fl_Button(180, 185, 220, 25, "delete last dub ('k')");
      deleteLastDub->callback((Fl_Callback*)cb_deleteLastDub);
    } // Fl_Button* deleteLastDub
    { deleteAllDubs = new Fl_Button(430, 185, 220, 25, "delete all dubs ('d')");
      deleteAllDubs->callback((Fl_Callback*)cb_deleteAllDubs);
    } // Fl_Button* deleteAllDubs
    { tempoTapper = new Fl_Button(480, 43, 170, 25, "tap to set tempo ('t')");
      tempoTapper->callback((Fl_Callback*)cb_tempoTapper);
    } // Fl_Button* tempoTapper
    { phraseEraser = new Fl_Button(510, 87, 140, 25, "erase current ('e')");
      phraseEraser->callback((Fl_Callback*)cb_phraseEraser);
    } // Fl_Button* phraseEraser
    LoopCenter->end();
  } // Fl_Double_Window* LoopCenter
  tempoTapper->clear_visible_focus();
  phraseSaver->clear_visible_focus();
  leftButton->clear_visible_focus();
  rightButton->clear_visible_focus();
  deleteLastDub->clear_visible_focus();
  deleteAllDubs->clear_visible_focus();
  phraseEraser->clear_visible_focus();
  metroVol->clear_visible_focus();
  recordVol->clear_visible_focus();
  for(int i = 0; i < phraseSetter->children(); i++) {
    phraseSetter->child(i)->clear_visible_focus();
  }
  for(int i = 0; i < bpMeasure->children(); i++) {
    bpMeasure->child(i)->clear_visible_focus();
  }
  for(int i = 0; i < metroTempo->children(); i++) {
    metroTempo->child(i)->clear_visible_focus();
  }

  LoopCenter->metroTempo = metroTempo;
  LoopCenter->tempoTap = tempoTapper;
  LoopCenter->metroVol = metroVol;
  LoopCenter->recordVol = recordVol;
  LoopCenter->bpMeasure = bpMeasure;
  LoopCenter->phraseSetter = phraseSetter;
  LoopCenter->leftButton = leftButton;
  LoopCenter->rightButton = rightButton;
}

int main(int argc, char *argv[]) {
 
  looper = new Looper();
  looperUI = new LooperUI(looper);
  looperUI->LoopCenter->show();
  Fl::lock();
  Updater *updater = new Updater();
  updater->looperUI = looperUI;
  updater->Start();
  looper->Start();
  looperUI->LoopCenter->setButtons();
  return Fl::run();

}
