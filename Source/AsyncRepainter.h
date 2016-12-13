//
//  AsyncRepainter.h
//  PiPoGettingStarted
//
//  Created by Joseph Larralde on 09/12/16.
//
//

#ifndef AsyncRepainter_h
#define AsyncRepainter_h

#include "../JuceLibraryCode/JuceHeader.h"

class AsyncRepainter : public AsyncUpdater {
private:
  AudioAppComponent *owner;
  
public:
  AsyncRepainter(AudioAppComponent *mcc) : owner(mcc) { }
  
  void handleAsyncUpdate() {
    owner->repaint();
  }
};


#endif /* AsyncRepainter_h */
