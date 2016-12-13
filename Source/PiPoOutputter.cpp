//
//  PiPoOutputter.cpp
//  PiPoGettingStarted
//
//  Created by Joseph Larralde on 06/12/16.
//
//

#include "PiPoOutputter.h"
#include "JucePiPoHost.h"

PiPoOutputter::PiPoOutputter(JucePiPoHost *host) : PiPo((PiPo::Parent *)host) {
  this->host = host;
  writeIndex = 0;
  readIndex = 0;
  ringBuffer.resize(PIPO_OUTPUTTER_RING_SIZE);
}

void
PiPoOutputter::setFrameCallback(std::function<void(std::vector<PiPoValue>, MainContentComponent *mcc)> f) {
  frameCallback = f;
}

int
PiPoOutputter::streamAttributes(bool hasTimeTags, double rate, double offset,
                     unsigned int width, unsigned int height,
                     const char **labels, bool hasVarSize, double domain,
                     unsigned int maxFrames) {
  
  this->host->setOutputAttributes(hasTimeTags, rate, offset, width, height,
                                  labels, hasVarSize, domain, maxFrames);
  
  for (int i = 0; i < PIPO_OUTPUTTER_RING_SIZE; ++i) {
    ringBuffer[i].resize(width * height);
  }
  return 0;
}

int
PiPoOutputter::frames(double time, double weight, float *values,
           unsigned int size, unsigned int num) {
  
  if (num > 0) {
    for (int i = 0; i < num; ++i) {
      
      for (int j = 0; j < size; ++j) {
        ringBuffer[writeIndex][j] = values[j + i*size];
      }
      
      // atomic swap ?
      writeIndex = 1 - writeIndex;
      readIndex = 1 - writeIndex;
      
      this->frameCallback(ringBuffer[readIndex], this->host->getOwner());
      std::cout << "callback !" << std::endl;
      
      if (writeIndex + 1 == PIPO_OUTPUTTER_RING_SIZE) {
        writeIndex = 0;
      } else {
        writeIndex++;
      }
    }
  }
  return 0;
}

std::vector<PiPoValue>
PiPoOutputter::getLastFrame() {
  std::vector<PiPoValue> f;
  
  if (readIndex > -1) {
    f = ringBuffer[readIndex];
  }
  return f;
}
