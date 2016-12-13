//
//  PiPoOutputter.hpp
//  PiPoGettingStarted
//
//  Created by Joseph Larralde on 06/12/16.
//
//

#ifndef PiPoOutputter_hpp
#define PiPoOutputter_hpp

#include "PiPo.h"

#define PIPO_OUTPUTTER_RING_SIZE 2

class JucePiPoHost;
class MainContentComponent;

class PiPoOutputter : public PiPo {
private:
  JucePiPoHost *host;
  std::atomic<int> writeIndex, readIndex;
  std::vector<std::vector<PiPoValue>> ringBuffer;
  std::function<void(std::vector<PiPoValue>, MainContentComponent *mcc)> frameCallback;
  
public:
  PiPoOutputter(JucePiPoHost *host);
  
  void setFrameCallback(std::function<void(std::vector<PiPoValue>, MainContentComponent *mc)> f);
  
  int streamAttributes(bool hasTimeTags, double rate, double offset,
                       unsigned int width, unsigned int height,
                       const char **labels, bool hasVarSize, double domain,
                       unsigned int maxFrames);
  
  int frames(double time, double weight, float *values,
             unsigned int size, unsigned int num);

  std::vector<PiPoValue> getLastFrame();
};

#endif /* PiPoOutputter_hpp */
