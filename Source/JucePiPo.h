/**
 * @file JucePiPo.h
 * @author Riccardo.Borghesi@ircam.fr (Derived from ofxPiPo.h by Joseph Larralde)
 * 
 * @brief wrapper of Pipo, including PipoCollection
 *
 *
 * Copyright (C) 2016 by IMTR IRCAM – Centre Pompidou, Paris, France.
 * All rights reserved.
 */

#ifndef _JUCE_PIPO_H_
#define _JUCE_PIPO_H_

#include "PiPoHost.h"

class JucePiPo {
public:
  std::string name;
  PiPo *pipo;

  JucePiPo(std::string name);
  ~JucePiPo();

  void connect(PiPo *receiver);
  int streamAttributes(bool hasTimeTags, double rate, double offset,
                       unsigned int width, unsigned int height,
                       const char **labels, bool hasVarSize, double domain,
                       unsigned int maxFrames);
  int frames(double time, double weight, PiPoValue *values, unsigned int size,
             unsigned int num);
  bool setAttr(const char *attrName, double value);
  bool setAttr(const char *attrName, std::vector<double> values);
  bool setAttr(const char *attrName, const char *value);
};

#endif /* _JUCE_PIPO_H_ */
