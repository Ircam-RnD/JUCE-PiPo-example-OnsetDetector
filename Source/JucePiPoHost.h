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
#ifndef _JUCE_PIPO_HOST_
#define _JUCE_PIPO_HOST_

// define this to build with the default ircam pipos
#define DEFAULT_PIPOS

#include "../JuceLibraryCode/JuceHeader.h"
#include "PiPoHost.h"
#include "PiPoOutputter.h"
#include "JucePiPo.h"

class MainContentComponent;

class JucePiPoParam {
  String name;
  String pipoName;
  String paramName;
  StringArray values;
  
public:
  JucePiPoParam() {
    name = String::empty;
    pipoName = String::empty;
    paramName = String::empty;
  }
  
  JucePiPoParam(String name, String pipoName, String paramName, StringArray values) {
    this->name = name;
    this->pipoName = String::empty;
    this->paramName = String::empty;
    this->values = StringArray(values);
  }
  
  const char *getName() {
    return name.toRawUTF8();
  }
  
  const char *getParamName() {
    return paramName.toRawUTF8();
  }
  
  const char *getPiPoName() {
    return pipoName.toRawUTF8();
  }
  
  int getNumValues() {
    return values.size();
  }
  
  bool isValueInt(int i) {
    const char *txt = values[i].toRawUTF8();
    bool isInt = false;
    
    if (txt != NULL) {
      int ival = atoi(txt);
      
      if (ival != 0) {
        isInt = true;
      } else {
        if (strlen(txt) == 1 && txt[0] == '0') {
          isInt = true;
        }
      }
    }
    return isInt;
  }
  
  bool isValueFloat(int i) {
    const char *txt = values[i].toRawUTF8();
    bool isFloat = false;
    
    if (txt != NULL) {
      double fval = atof(txt);
      
      if (fval != 0 && fval != HUGE_VAL) {
        isFloat = true;
      } else {
        if ((strlen(txt) == 1 && txt[0] == '0') ||
            (strlen(txt) == 2 && txt[0] == '0' && txt[1] == '.') ||
            (strlen(txt) == 3 && txt[0] == '0' && txt[1] == '.' && txt[2] == '0')) {
          isFloat = true;
        }
      }
    }
    return isFloat;
  }

  bool isValueNum(int i) {
    return (isValueInt(i) || isValueFloat(i));
  }
  
  float getValueFloat(int i) {
    return values[i].getFloatValue();
  }
  
  int getValueInt(int i) {
    return values[i].getIntValue();
  }
  
  const char *getValueString(int i) {
    return values[i].toRawUTF8();
  }
  
  const String getValuesAsString() {
    return values.joinIntoString(" ");
  }
};

//============================== JucePiPoHost ================================//

class JucePiPoHost : public PiPo::Parent {
  
  class JucePiPoModuleFactory : public PiPoModuleFactory {
    
    class JucePiPoModule : public PiPoModule {
      JucePiPo *jucePiPo;
      
    public:
      JucePiPoModule(JucePiPo *jucePiPo) {
        this->jucePiPo = jucePiPo;
      };
      
      ~JucePiPoModule(void) {
        if (this->jucePiPo != NULL) {
          delete this->jucePiPo;
        }
      };
    };

  public:
    JucePiPoModuleFactory() { };
    
    ~JucePiPoModuleFactory(void) { };
    
    PiPo *create(unsigned int index, const std::string &pipoName,
                 const std::string &instanceName, PiPoModule *&module) {
      std::string pipoClassNameStr = "pipo." + pipoName;
      JucePiPo *jucePiPo = new JucePiPo(pipoName.c_str());
      
      if(jucePiPo->pipo != NULL) {
        module = new JucePiPoModule(jucePiPo);
        return jucePiPo->pipo;
      } else {
        delete jucePiPo;
      }
      return NULL;
    }
  };
  
  class PiPoStreamAttributes {
  public:
    int hasTimeTags;
    double rate;
    double offset;
    unsigned int dims[2];
    const char *labels[PIPO_MAX_LABELS];
    unsigned int numLabels;
    bool hasVarSize;
    double domain;
    unsigned int maxFrames;
    
    PiPoStreamAttributes(void) : labels() {
      this->hasTimeTags = false;
      this->rate = 1000.0;
      this->offset = 0.0;
      this->dims[0] = 1;
      this->dims[1] = 1;
      this->numLabels = 0;
      this->hasVarSize = false;
      this->domain = 0.0;
      this->maxFrames = 1;
    };
  };
  
  JucePiPo *chain;
  PiPoOutputter *outputter;
  MainContentComponent *owner;
  PiPoStreamAttributes inputStreamAttrs;
  PiPoStreamAttributes outputStreamAttrs;
  
public:
  JucePiPoHost(MainContentComponent *owner);
  ~JucePiPoHost();
  
  MainContentComponent *getOwner();
  void onNewFrame(std::function<void(std::vector<PiPoValue>, MainContentComponent *mcc)> cb);
  std::vector<PiPoValue> getLastFrame();
  JucePiPo *setPiPoChain(std::string name);
  void clearPiPoChain();
  
  void propagateInputAttributes(void);
  void setOutputAttributes(bool hasTimeTags, double rate, double offset,
                           unsigned int width, unsigned int size, const
                           char **labels, bool hasVarSize,
                           double domain, unsigned int maxFrames);

  void streamAttributesChanged(PiPo *pipo, PiPo::Attr *attr);
  void signalError(PiPo *pipo, std::string errorMsg);
  void signalWarning(PiPo *pipo, std::string errorMsg);
  
  void setInputDims(int width, int size, bool propagate = true);
  void setInputHasTimeTags(int hasTimeTags, bool propagate = true);
  void setInputFrameRate(double sampleRate, bool propagate = true);
  void setInputFrameOffset(double sampleOffset, bool propagate = true);
  void setInputMaxFrames(int maxFrames, bool propagate = true);
  
  void getInputDims(int &width, int &size);
  bool getInputHasTimeTags(void);
  double getInputFrameRate(void);
  double getInputFrameOffset(void);

  void getOutputDims(int &width, int &size);
  bool getOutputHasTimeTags(void);
  double getOutputFrameRate(void);
  double getOutputFrameOffset(void);
  int getOutputMaxFrames();
  
  // TODO : adapt this one (not functionnal yet)
  void setJucePiPoParam(JucePiPoParam *param);
};

#endif
