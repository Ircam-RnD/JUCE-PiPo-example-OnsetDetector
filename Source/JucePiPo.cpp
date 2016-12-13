/**
 * @file JucePiPo.cpp
 * @author Riccardo.Borghesi@ircam.fr (Derived from ofxPiPo.cpp by Joseph Larralde)
 * 
 * @brief wrapper of Pipo, including PipoCollection
 *
 *
 * Copyright (C) 2016 by IMTR IRCAM – Centre Pompidou, Paris, France.
 * All rights reserved.
 */


#include "JucePiPo.h"
#include "PiPoCollection.h"
#include "PiPoHost.h"

JucePiPo::JucePiPo(std::string name) {
  this->pipo = PiPoCollection::create(name);
  
  if (this->pipo != NULL) {
    this->name = name;
  } else {
    this->name = "undefined";
  }
}

JucePiPo::~JucePiPo() {
  if (this->pipo != NULL) {
    delete pipo;
  }
}

void
JucePiPo::connect(PiPo *receiver) {
  this->pipo->setReceiver(receiver);
}

int
JucePiPo::streamAttributes(bool hasTimeTags, double rate, double offset,
                           unsigned int width, unsigned int height,
                           const char **labels, bool hasVarSize, double domain,
                           unsigned int maxFrames) {
  return this->pipo->streamAttributes(hasTimeTags, rate, offset, width, height, labels, hasVarSize, domain, maxFrames);
}

int
JucePiPo::frames(double time, double weight, PiPoValue *values, unsigned int size,
                 unsigned int num) {
  return this->pipo->frames(time, weight, values, size, num);
}

bool
JucePiPo::setAttr(const char *attrName, double value) {
  PiPo::Attr *attr = this->pipo->getAttr(attrName);
  
  if (attr != NULL) {
    int iAttr = attr->getIndex();
    return this->pipo->setAttr(iAttr, value);
  }
  return false;
}

bool
JucePiPo::setAttr(const char *attrName, std::vector<double> values) {
  PiPo::Attr *attr = this->pipo->getAttr(attrName);

  if (attr != NULL) {
    int iAttr = attr->getIndex();
    double vals[values.size()];
    return this->pipo->setAttr(iAttr, vals, values.size());
  }
  return false;
}

bool
JucePiPo::setAttr(const char *attrName, const char *value) {
  PiPo::Attr *attr = this->pipo->getAttr(attrName);
  
  if (attr != NULL) {
    int iAttr = attr->getIndex();
    PiPo::Type type = attr->getType();
  
    if (type == PiPo::Type::Enum) {
      std::vector<const char *> *list = attr->getEnumList();
      
      for (int i = 0; i < list->size(); i++) {
        if (strcmp(list->at(i), value) == 0) {
          attr->set(0, i);
          return true;
        }
      }
    }
  }
  return false;
}
