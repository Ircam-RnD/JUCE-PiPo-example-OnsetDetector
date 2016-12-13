/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "JucePiPoHost.h"
#include "PiPoOutputter.h"
#include "AsyncRepainter.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/

class MainContentComponent   : public AudioAppComponent {
public:
  MainContentComponent() : notifier(this) {
    initPipoHost();
    color = Colour(0,0,0);
    setSize (200, 200);
    
    // specify the number of input and output channels that we want to open
    setAudioChannels (1, 2);
  }

  ~MainContentComponent() {
    shutdownAudio();
    delete host;
  }

  //============================================================================
  void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override {
    // This function will be called when the audio device is started, or when
    // its settings (i.e. sample rate, block size, etc) are changed.

    // You can use this function to initialise any resources you might need,
    // but be careful - it will be called on the audio thread, not the GUI thread.

    // For more details, see the help for AudioProcessor::prepareToPlay()
    
    msr = 1000. / sampleRate;
    audioInputVector.resize(samplesPerBlockExpected);
    
    if (this->jucePipo != NULL) {
      this->jucePipo->streamAttributes(true, sampleRate, 0,
                                       samplesPerBlockExpected, 1, NULL,
                                       false, 0, 1);
    }
  }

  //============================================================================
  void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override {
    AudioIODevice* device = deviceManager.getCurrentAudioDevice();
    const BigInteger activeInputChannels = device->getActiveInputChannels();
    const BigInteger activeOutputChannels = device->getActiveOutputChannels();

    const int maxInputChannels = activeInputChannels.getHighestBit() + 1;
    const int maxOutputChannels = activeOutputChannels.getHighestBit() + 1;
    
    for (int channel = 0; channel < maxOutputChannels; ++channel) {
      if ((!activeOutputChannels[channel]) || maxInputChannels == 0) {
        bufferToFill.buffer->clear(channel, bufferToFill.startSample, bufferToFill.numSamples);
      } else {
        const int actualInputChannel = channel % maxInputChannels;
        
        if (!activeInputChannels[actualInputChannel]) {
          bufferToFill.buffer->clear(actualInputChannel,
                                     bufferToFill.startSample, bufferToFill.numSamples);
        } else {
          const float *inBuffer = bufferToFill.buffer->getReadPointer(actualInputChannel,
                                                                      bufferToFill.startSample);
          
          if (audioInputVector.size () != bufferToFill.numSamples) {
            audioInputVector.resize(bufferToFill.numSamples);
          }

          for(int i = 0; i < bufferToFill.numSamples; i++) {
            audioInputVector[i] = inBuffer[i];
          }
          
          if (this->jucePipo != NULL) {
            this->jucePipo->frames(cnt, 1.0, &audioInputVector[0], 1, bufferToFill.numSamples);
            cnt += msr * bufferToFill.numSamples;
          }

          bufferToFill.buffer->clear(actualInputChannel, bufferToFill.startSample, bufferToFill.numSamples);
        }
      }
    }
//    bufferToFill.clearActiveBufferRegion();
  }

  void releaseResources() override {
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
  }

  //==============================================================================
  void paint (Graphics& g) override {
    g.fillAll(this->color);
  }
  
  void resized() override {
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
  }


private:
  //==============================================================================

  void initPipoHost() {
    this->host = new JucePiPoHost(this);
    this->jucePipo = NULL;
    
    cnt = 0;
    
    this->jucePipo = this->host->setPiPoChain("slice:fft:sum:scale:onseg");
    
    this->jucePipo->setAttr("slice.size", 1024);
    this->jucePipo->setAttr("slice.hop", 256);
    this->jucePipo->setAttr("slice.norm", "power");
    this->jucePipo->setAttr("fft.mode", "power");
    this->jucePipo->setAttr("fft.weighting", "itur468");
    this->jucePipo->setAttr("scale.inmin", 1.);
    this->jucePipo->setAttr("scale.inmax", 10.);
    this->jucePipo->setAttr("scale.outmin", 0.);
    this->jucePipo->setAttr("scale.outmax", 10.);
    this->jucePipo->setAttr("scale.func", "log");
    this->jucePipo->setAttr("scale.base", 10.);
    this->jucePipo->setAttr("onseg.duration", 1.);
    this->jucePipo->setAttr("onseg.min", 0.);
    this->jucePipo->setAttr("onseg.max", 1.);
    this->jucePipo->setAttr("onseg.mean", 0.);
    this->jucePipo->setAttr("onseg.stddev", 0.);
    this->jucePipo->setAttr("onseg.startisonset", 1.);
    this->jucePipo->setAttr("onseg.threshold", 15.);
    this->jucePipo->setAttr("onseg.offthresh", -120.);
    
    std::function<void(std::vector<PiPoValue>, MainContentComponent*)> logger
      = [](std::vector<PiPoValue> v, MainContentComponent *mcc) {
        std::cout << v[0] << std::endl;
        mcc->color = Colour(mcc->random.nextFloat() * 255,
                            mcc->random.nextFloat() * 255,
                            mcc->random.nextFloat() * 255);
        mcc->triggerAsyncUpdate();
    };
    
    this->host->onNewFrame(logger);
  }
  
  void triggerAsyncUpdate() {
    notifier.triggerAsyncUpdate();
  }

  Colour color;
  Random random;
  AsyncRepainter notifier;
  JucePiPoHost *host;
  JucePiPo *jucePipo;
  std::vector<float> audioInputVector;
  double cnt;
  double msr;
  
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};


// (This function is called by the app startup code to create our main component)
Component* createMainContentComponent()     { return new MainContentComponent(); }


#endif  // MAINCOMPONENT_H_INCLUDED
