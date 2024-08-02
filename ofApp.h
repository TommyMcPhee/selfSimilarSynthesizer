#pragma once

#include "ofMain.h"
#include "ofxMidi.h"
#include "ofxGui.h"
#include "ofxDropdown.h"
#include "sinOsc.hpp"

class ofApp : public ofBaseApp {

public:
    void setup();
    void update();
    void draw();
    void windowResized(int w, int h);
    void exit();
    void ofSoundStreamSetup(ofSoundStreamSettings settings);
    void audioOut(ofSoundBuffer& buffer);
    void newMidiMessage(ofxMidiMessage& message);

    void getFrequency();
    void getTime();
    void getBoundary();

    void audioSetup();
    void midiSetup();
    float midiToFrequency(float noteNumber);
    void scoreSetup();
    void clearScore();
    void reset();

    vector<float> getSet(string input);
    vector<float> getSetMagnitudes(vector<float> bipolarSet);
    vector<float> scaleSet(vector<float> unipolarSet);
    vector<float> rescaleSet(float boundaryRatio, vector<float> scaledSet);

    float interpolateMultiplier(float current, float next, float elapsedTime, float totalTime);
    float getSample();
    void updateTime();
    void startMidiTimer();
    void stopMidiTimer();
    void resizeGui();

    int sampleRate;
    int bufferSize;
    int numberOfChannels;
    ofSoundDevice device;
    ofSoundDevice::Api api;
    ofSoundStreamSettings streamSettings;
    ofSoundStream stream;

    bool midiFrequency;
    bool midiBoundary;

    float lastBufferSize;
    float lastTuningMIDIInput;
    float lastTuningFrequencyInput;
    bool lastFrequencyInputState;
    bool lastTimeInputState;
    string lastSetInput;
    float lastManualFrequencyInput;
    float lastManualTimeInput;
    float lastManualBoundaryInput;
    bool lastRenderState;

    string frequencyUnit;
    string timeUnit;
    string boundaryMode;

    vector<float> bipolarSet;
    vector<float> unipolarSet;
    vector<float> scaledSet;
    vector<int> indicies;
    vector<float> times;
    vector<sinOsc> oscillators;

    float fundamentalFrequency;
    float fundamentalTime;
    float boundaryRatio;
    bool isBoundary = false;

    float minimum;
    int maximumIndex;
    int size;
    bool midiTimer;
    int duration;

    ofxMidiIn midiIn;
    ofxMidiMessage midiMessage;

    ofxPanel gui;
    ofRectangle guiShape;
    glm::vec3 guiPosition;
    float width;
    float height;

    ofxToggle frequencyInput;
    ofxToggle timeInput;
    ofxToggle midiRender;
    ofxToggle midiLoop;
    ofxToggle render;
    ofxToggle loop;

    vector<ofSoundDevice> audioDevices;
    std::unordered_map<string, ofSoundDevice::Api> apis;
    vector<unsigned int> sampleRates;

    vector<string> midiDevices;
    vector<string> midiPorts;
    vector<int> midiChannels;

    ofxDropdown_<string> selectAudioDevice{ "Audio Device" };
    ofxDropdown_<string> selectApi{ "API" };
    ofxDropdown_<int> selectSampleRate{ "Sample Rate" };
    ofxDropdown_<string> selectMidiPorts{ "MIDI Ports" };
    ofxDropdown_<int> selectMidiChannels{ "MIDI Channels" };
    ofxDropdown_<string> selectFrequencyUnit{ "Frequency Units" };
    ofxDropdown_<string> selectTimeUnit{ "Duration Units" };
    ofxDropdown_<string> selectBoundary{ "Boundary Type and Units" };

    ofxTextField setInput;
    ofxIntField bufferSizeInput;
    ofxFloatField tuningMIDIInput;
    ofxFloatField tuningFrequencyInput;
    ofxFloatField manualFrequencyInput;
    ofxFloatField manualTimeInput;
    ofxFloatField boundaryInput;

    bool audioInstanstiation = false;
};