/**
 *
 * @file JucePiPoHost.h
 * @author Riccardo.Borghesi@ircam.fr
 * 
 * @brief Juce PiPo utilities
 * Copyright (C) 2012-2014 by IRCAM – Centre Pompidou, Paris, France.
 * All rights reserved.
 * 
 */
#include "JucePiPoHost.h"
#include "PiPoCollection.h"

#include <string.h>
#include <vector>

using namespace std;

/*********************************************************
 *
 *  Juce PiPo Host Utils
 *
 */
static const unsigned int maxWordLen = 256;

static bool
getPiPoInstanceAndAttrName(const char *attrName, char *instanceName, char *pipoAttrName) {
  const char *dot = strrchr(attrName, '.');
  
  if (dot != NULL) {
    unsigned int pipoAttrNameLen = dot - attrName;
    
    strcpy(pipoAttrName, dot + 1);
    
    if (pipoAttrNameLen > maxWordLen) {
      pipoAttrNameLen = maxWordLen;
    }
    
    strncpy(instanceName, attrName, pipoAttrNameLen);
    instanceName[pipoAttrNameLen] = '\0';
    
    return true;
  }
  
  return false;
}

/*********************************************************
 *
 *  Juce PiPo Host Class
 *
 */
JucePiPoHost::JucePiPoHost(MainContentComponent *mcc) :
owner(mcc), inputStreamAttrs(), outputStreamAttrs() {
  PiPoCollection::init();
  outputter = new PiPoOutputter(this);
  chain = nullptr;
}

JucePiPoHost::~JucePiPoHost(void) {
  if (chain != nullptr) {
    delete chain;
  }
  PiPoCollection::deinit();
}

MainContentComponent *
JucePiPoHost::getOwner() {
  return owner;
}

void
JucePiPoHost::onNewFrame(std::function<void(std::vector<PiPoValue>,
                                            MainContentComponent *mcc)> cb) {
  outputter->setFrameCallback(cb);
}

std::vector<PiPoValue>
JucePiPoHost::getLastFrame() {
  return outputter->getLastFrame();
}

JucePiPo *
JucePiPoHost::setPiPoChain(std::string name) {
  if (chain != nullptr) {
    delete chain;
  }
  
  chain = new JucePiPo(name);
  chain->connect((PiPo *)outputter);
  return chain;
}

void
JucePiPoHost::clearPiPoChain() {
  delete chain;
  chain = nullptr;
}

void
JucePiPoHost::propagateInputAttributes(void) {
  if (chain != NULL) {
    const char *colNameStr[PIPO_MAX_LABELS];
    const char **labels = NULL;
    unsigned int numCols = this->inputStreamAttrs.dims[0];
    unsigned int numLabels = this->inputStreamAttrs.numLabels;
    
    if (numLabels > PIPO_MAX_LABELS) {
      numLabels = PIPO_MAX_LABELS;
    }
    
    if (numLabels > numCols) {
      numLabels = numCols;
    }
    
    if (numLabels > 0) {
      for (unsigned int i = 0; i < numLabels; i++) {
        colNameStr[i] = this->inputStreamAttrs.labels[i];
      }
      
      for (unsigned int i = numLabels; i < numCols; i++) {
        colNameStr[i] = "unnamed";
      }
      
      labels = colNameStr;
    }
    
    chain->streamAttributes(this->inputStreamAttrs.hasTimeTags,
                           this->inputStreamAttrs.rate,
                           this->inputStreamAttrs.offset,
                           this->inputStreamAttrs.dims[0],
                           this->inputStreamAttrs.dims[1],
                           labels,
                           this->inputStreamAttrs.hasVarSize,
                           this->inputStreamAttrs.domain,
                           this->inputStreamAttrs.maxFrames);
  }
}

void
JucePiPoHost::setOutputAttributes(bool hasTimeTags, double rate, double offset,
                                  unsigned int width, unsigned int size,
                                  const char **labels, bool hasVarSize,
                                  double domain, unsigned int maxFrames) {
  
  if (labels != NULL) {
    int numLabels = width;
    
    if (numLabels > PIPO_MAX_LABELS) {
      numLabels = PIPO_MAX_LABELS;
    }
    
    for (int i = 0; i < numLabels; i++) {
      this->outputStreamAttrs.labels[i] = labels[i];
    }
    
    this->outputStreamAttrs.numLabels = width;
  } else {
    this->outputStreamAttrs.numLabels = 0;
  }
  
  this->outputStreamAttrs.hasTimeTags = hasTimeTags;
  this->outputStreamAttrs.rate = rate;
  this->outputStreamAttrs.offset = offset;
  this->outputStreamAttrs.dims[0] = width;
  this->outputStreamAttrs.dims[1] = size;
  this->outputStreamAttrs.hasVarSize = hasVarSize;
  this->outputStreamAttrs.domain = domain;
  this->outputStreamAttrs.maxFrames = maxFrames; 
}

void
JucePiPoHost::streamAttributesChanged(PiPo *pipo, PiPo::Attr *attr) {
  this->propagateInputAttributes();
}

void
JucePiPoHost::signalError(PiPo *pipo, std::string errorMsg) {
  //object_error(this->ext, errorMsg.c_str());
}

void
JucePiPoHost::signalWarning(PiPo *pipo, std::string errorMsg) {
  //object_warn(this->ext, errorMsg.c_str());
}

void
JucePiPoHost::setInputDims(int width, int size, bool propagate) {
  this->inputStreamAttrs.dims[0] = width;
  this->inputStreamAttrs.dims[1] = size;
  
  if (propagate) {
    this->propagateInputAttributes();
  }
}

void
JucePiPoHost::setInputHasTimeTags(int hasTimeTags, bool propagate) {
  this->inputStreamAttrs.hasTimeTags = hasTimeTags;
  
  if (propagate) {
    this->propagateInputAttributes();
  }
}

#define MIN_SAMPLERATE (1.0 / 31536000000.0) /* once a year */
#define MAX_SAMPLERATE (96000000000.0)

void
JucePiPoHost::setInputFrameRate(double sampleRate, bool propagate) {
  if(sampleRate <= MIN_SAMPLERATE) {
    this->inputStreamAttrs.rate = MIN_SAMPLERATE;
  } else if (sampleRate >= MAX_SAMPLERATE) {
    this->inputStreamAttrs.rate = MAX_SAMPLERATE;
  } else {
    this->inputStreamAttrs.rate = sampleRate;
  }
  
  if(propagate) {
    this->propagateInputAttributes();
  }
}

void
JucePiPoHost::setInputFrameOffset(double sampleOffset, bool propagate) {
  this->inputStreamAttrs.offset = sampleOffset;

  if (propagate) {
    this->propagateInputAttributes();
  }
}

void
JucePiPoHost::setInputMaxFrames(int maxFrames, bool propagate) {
  this->inputStreamAttrs.maxFrames = maxFrames;

  if(propagate) {
    this->propagateInputAttributes();
  }
}

void
JucePiPoHost::getInputDims(int &width, int &size) {
  width = this->inputStreamAttrs.dims[0];
  size = this->inputStreamAttrs.dims[1];
}

bool
JucePiPoHost::getInputHasTimeTags() {
  return this->inputStreamAttrs.hasTimeTags;
}

double
JucePiPoHost::getInputFrameRate(void) {
  return this->inputStreamAttrs.rate;
}

double
JucePiPoHost::getInputFrameOffset(void) {
  return this->inputStreamAttrs.offset;
}

void
JucePiPoHost::getOutputDims(int &width, int &size) {
  width = this->outputStreamAttrs.dims[0];
  size = this->outputStreamAttrs.dims[1];
}

bool
JucePiPoHost::getOutputHasTimeTags() {
  return this->outputStreamAttrs.hasTimeTags;
}

double
JucePiPoHost::getOutputFrameRate(void) {
  return this->outputStreamAttrs.rate;
}

double
JucePiPoHost::getOutputFrameOffset(void) {
  return this->outputStreamAttrs.offset;
}

int
JucePiPoHost::getOutputMaxFrames() {
  return this->outputStreamAttrs.maxFrames;
}

void JucePiPoHost::setJucePiPoParam(JucePiPoParam *param) {
  char instanceName[maxWordLen];
  char pipoAttrName[maxWordLen];
  
  if(getPiPoInstanceAndAttrName(param->getName(), instanceName, pipoAttrName)) {
    /*
    PiPo *pipo = getChain()->getPiPo(instanceName);
    
    if (pipo != NULL) {
      PiPo::Attr *attr = pipo->getAttr(param->getName());
      
      if (attr != NULL) {
        int ac = param->getNumValues();
       
        if (ac > 0) {
          PiPo::Type type = attr->getType();
          
          attr->setSize(ac);
          
          for (int i = 0; i < ac; i++) {
            switch(type) {
              case PiPo::Int: {
                attr->set(i, (int)param->getValueInt(i), true);
                break;
              }
       
              case PiPo::Double:
              case PiPo::Float: {
                attr->set(i, (double)param->getValueFloat(i), true);
                break;
              }
       
              case PiPo::String: {
                attr->set(i, param->getValueString(i), true);
                break;
              }
       
              case PiPo::Bool: {
                attr->set(i, (bool)param->getValueInt(i), true);
                break;
              }
       
              case PiPo::Enum: {
                int enumIndex = (dynamic_cast<PiPo::EnumAttr *>(attr))->getEnumIndex(param->getValueString(i));
                attr->set(i, enumIndex, true);
                break;
              }
       
              case PiPo::Undefined: {
                break;
              }
            }
          }
          
          attr->changed(false);
        }
      }
    }
    //*/
  }
}
