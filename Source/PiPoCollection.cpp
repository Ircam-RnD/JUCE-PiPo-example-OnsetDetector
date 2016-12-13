/**
 *
 * @file PiPoCollection.h
 * @author Norbert.Schnell@ircam.fr
 * 
 * @brief PiPo Module Collection
 * 
 * Copyright (C) 2013 by IMTR IRCAM – Centre Pompidou, Paris, France.
 * All rights reserved.
 * 
 */
//#include <iostream>

#include "PiPoCollection.h"
#include "PiPoHost.h"

#include "../JuceLibraryCode/JuceHeader.h"

static Array<DynamicLibrary *> loadedPipoLibs;

class PiPoPool : public PiPoModuleFactory {
  
  class PiPoPoolModule : public PiPoModule {
  private:
    PiPo *pipo;
    std::string instanceName;
      
  public:
    PiPoPoolModule(PiPo *pipo) {
      this->pipo = pipo;
    }
      
    PiPoPoolModule() {
      if(this->pipo != NULL) {
        delete this->pipo;
        this->pipo = NULL;
      }
    }
  };
    
public:
  PiPoPool() {
    // nothing to do here
  }
  
  ~PiPoPool() {
    for (pipoMap::iterator it = map.begin(); it != map.end(); ++it) {
      if (it->second != NULL) {
        delete it->second;
        it->second = NULL;
      }
    }
    
    if (loadedPipoLibs.size() > 0) {
      DynamicLibrary *dlyb;
      int numLibs = loadedPipoLibs.size();
      
      for (int i = numLibs-1; i >= 0; i--) {
        dlyb = loadedPipoLibs[i];
        loadedPipoLibs.remove(i);
        dlyb->close();
        delete dlyb;
      }
    }
  }
  
  void include(std::string name, PiPoCreatorBase *creator) {
    // no need to check if the place is empty, the pointer will be replaced
    // dynamic libs will have priority over built-in pipos when they have the same name
    map[name] = creator;
  }
    
  PiPo *create(unsigned int index, const std::string &pipoName,
               const std::string &instanceName, PiPoModule *&module) {
    pipoMap::iterator it = map.find(pipoName);
    if(it == map.end()) return NULL;
    PiPo *ret = it->second->create();
    // change this to a unique_ptr, should be automatically deleted
    // for now the consumer still has to handle it ...
    module = new PiPoPoolModule(ret);
    return ret;
  }

private:
  typedef std::map<std::string, PiPoCreatorBase *> pipoMap;
  pipoMap map;

};

//============================================================================//

static PiPoPool *factory;

//============================================================================//

void
PiPoCollection::init(bool defaultPipo) {
  
  if (factory != NULL) {
    delete factory;
  }

  factory = new PiPoPool();

  Array<File> pipos;
  DynamicLibrary *dlyb;
  String pipoNamesString = String::empty;
  bool firstPipo = true;

  File appLocation = File::getSpecialLocation(File::currentApplicationFile);
  String appPath = appLocation.getFullPathName();
  String parentPath = appPath.upToLastOccurrenceOf("/", true, true);
  String pipoPath = parentPath + appLocation.getFileName() + "/Contents/Resources/pipo/";
  std::cout << pipoPath << std::endl;
  
  if (pipoPath.compare("") == 0) {
    return;
  }
  
  int numPipos = File(pipoPath).findChildFiles(pipos, File::findFiles, false, "*.dylib");
  int numLoadedPipos = 0;
  
  for (int i = 0; i < numPipos; i++) {
    dlyb = new DynamicLibrary();
    if (dlyb->open(pipos[i].getFullPathName())) {
      PiPoCreatorBase *(*getCreatorFun)() = (PiPoCreatorBase *(*)())dlyb->getFunction("getPiPoCreator");
      PiPoCreatorBase *pipoCreator = getCreatorFun();
      
      const char *(*getNameFun)() = (const char *(*)())dlyb->getFunction("getPiPoName");
      const char *pipoName = getNameFun();
      
      if (pipoCreator != NULL && pipoName != NULL) {
        addToCollection(pipoName, pipoCreator);
        if (firstPipo) {
          pipoNamesString += String(pipoName);
          firstPipo = false;
        } else {
          pipoNamesString += (" | "+String(pipoName));
        }
        numLoadedPipos++;
      }
      loadedPipoLibs.add(dlyb);
    }
  }
  //std::cout << "number of loaded dynamic pipos : " << numLoadedPipos << std::endl;
}

void
PiPoCollection::deinit() {
  if (factory != NULL) {
    delete factory;
    factory = NULL;
  }
}

void
PiPoCollection::addToCollection(std::string name, PiPoCreatorBase *creator) {
  factory->include(name, creator);
}

PiPo *
PiPoCollection::create(std::string name) {
  PiPoChain *chain = new PiPoChain(NULL, factory);
  
  if (chain->parse(name.c_str()) > 0 &&
      chain->instantiate() &&
      chain->connect(NULL)) {

    chain->copyPiPoAttributes();
    return static_cast<PiPo *>(chain);
  }
  return NULL;
}
