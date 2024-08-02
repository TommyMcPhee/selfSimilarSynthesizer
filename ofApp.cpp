#include "ofApp.h"

void ofApp::setup() {
    audioDevices = stream.getDeviceList();
    for (int i = 0; i < audioDevices.size(); i++) {
        string audioDevice = audioDevices.at(i).name;
        selectAudioDevice.add(audioDevice);
    }

    apis = {
{"UNSPECIFIED", ofSoundDevice::Api::UNSPECIFIED},
{"DEFAULT", ofSoundDevice::Api::DEFAULT},
{"ALSA", ofSoundDevice::Api::ALSA}, /*!< The Advanced Linux Sound Architecture API. */
{"PULSE", ofSoundDevice::Api::PULSE}, /*!< The Linux PulseAudio API. */
{"OSS", ofSoundDevice::Api::OSS}, /*!< The Linux Open Sound System API. */
{"JACK", ofSoundDevice::Api::JACK}, /*!< The Jack Low-Latency Audio Server API. */
{"OSX_CORE", ofSoundDevice::Api::OSX_CORE}, /*!< Macintosh OS-X Core Audio API. */
{"MS_WASAPI", ofSoundDevice::Api::MS_WASAPI}, /*!< The Microsoft WASAPI API. */
{"MS_ASIO", ofSoundDevice::Api::MS_ASIO}, /*!< The Steinberg Audio Stream I/O API. */
{"MS_DS", ofSoundDevice::Api::MS_DS} /*!< The Microsoft Direct Sound API. */
    };

    selectApi.add("UNSPECIFIED");
    selectApi.add("DEFAULT");
    selectApi.add("ALSA");
    selectApi.add("PULSE");
    selectApi.add("OSS");
    selectApi.add("JACK");
    selectApi.add("OSX_CORE");
    selectApi.add("MS_WASAPI");
    selectApi.add("MS_ASIO");
    selectApi.add("MS_DS");

    midiDevices = midiIn.getInPortList();
    for (int i = 0; i < midiDevices.size(); i++) {
        string midiDevice = midiDevices.at(i);
        selectMidiPorts.add(midiDevice);
    }
    for (int i = 1; i < 17; i++) {
        selectMidiChannels.add(i);
    }

    selectFrequencyUnit.add("Hertz (Hz)");
    selectFrequencyUnit.add("MIDI Note Number");
    selectFrequencyUnit.add("Ratio to Sample Rate (typically < 1)");
    selectFrequencyUnit.add("Period (seconds)");
    selectFrequencyUnit.add("Period (samples)");

    selectTimeUnit.add("Seconds");
    selectTimeUnit.add("Samples");
    selectTimeUnit.add("Cycles");

    selectBoundary.add("No Boundary");
    selectBoundary.add("Frequency: Hertz (Hz)");
    selectBoundary.add("Frequency: MIDI Note Number");
    selectBoundary.add("Frequency: Ratio to Sample Rate (typically < 1)");
    selectBoundary.add("Frequency: Period (seconds)");
    selectBoundary.add("Frequency: Period (samples)");
    selectBoundary.add("Duration: Seconds");
    selectBoundary.add("Duration: Samples");
    selectBoundary.add("Duration: Cycles");
    selectBoundary.add("Ratio");

    selectAudioDevice.disableMultipleSelection();
    selectApi.disableMultipleSelection();
    selectSampleRate.disableMultipleSelection();
    selectMidiPorts.enableMultipleSelection();
    selectMidiChannels.enableMultipleSelection();
    selectFrequencyUnit.disableMultipleSelection();
    selectTimeUnit.disableMultipleSelection();
    selectBoundary.disableMultipleSelection();

    selectAudioDevice.setSelectedValueByIndex(0, false);
    selectApi.setSelectedValueByIndex(0, false);
    selectSampleRate.setSelectedValueByIndex(0, false);
    selectFrequencyUnit.setSelectedValueByIndex(0, false);
    selectTimeUnit.setSelectedValueByIndex(0, false);
    selectBoundary.setSelectedValueByIndex(0, false);

    frequencyUnit = selectFrequencyUnit.selectedValue.toString();
    timeUnit = selectTimeUnit.selectedValue.toString();
    boundaryMode = selectBoundary.selectedValue.toString();

    gui.setup();
    guiPosition = { 0, -18, 0 };
    gui.setPosition(guiPosition);

    gui.add(&selectAudioDevice);
    gui.add(&selectApi);
    gui.add(&selectSampleRate);
    gui.add(bufferSizeInput.setup("Buffer Size", 256, 1, 65536));
    gui.add(&selectMidiPorts);
    gui.add(&selectMidiChannels);
    gui.add(tuningMIDIInput.setup("MIDI Tuning Note", 69.0, FLT_MIN, FLT_MAX));
    gui.add(tuningFrequencyInput.setup("Tuning Note Frequency (Hz)", 440.0, FLT_MIN, FLT_MAX));
    gui.add(setInput.setup("Set (seperate values with spaces)", "1 2 5 8"));
    gui.add(frequencyInput.setup("Frequency Input Mode (Manual/MIDI)", true));
    gui.add(&selectFrequencyUnit);
    gui.add(manualFrequencyInput.setup("Center Frequency", 256.0, FLT_MIN, FLT_MAX));
    gui.add(timeInput.setup("Duration Input Mode (Manual/MIDI", true));
    gui.add(&selectTimeUnit);
    gui.add(manualTimeInput.setup("Center Duration", 1 / 256.0, FLT_MIN, FLT_MAX));
    gui.add(&selectBoundary);
    gui.add(boundaryInput.setup("Boundary", 16.0, FLT_MIN, FLT_MAX));
    gui.add(midiRender.setup("Render from MIDI Note On OR Off", false));
    gui.add(midiLoop.setup("Loop while MIDI Note On (requires manual time input)", false));
    gui.add(render.setup("Render", false));
    gui.add(loop.setup("Loop", false));

    resizeGui();

    lastTuningMIDIInput = tuningMIDIInput;
    lastTuningFrequencyInput = tuningFrequencyInput;
    lastSetInput = (string)setInput;
    lastFrequencyInputState = frequencyInput;
    lastTimeInputState = timeInput;
    lastManualFrequencyInput = manualFrequencyInput;
    lastManualTimeInput = manualTimeInput;
    lastManualBoundaryInput = boundaryInput;

    scoreSetup();
    audioSetup();
}

void ofApp::update() {
    if (gui.isMinimized()) {
        gui.maximize();
    }
    if (gui.getPosition() != guiPosition) {
        gui.setPosition(guiPosition);
    }
    if (selectAudioDevice.selectedValue.toString() != device.name) {
        device = stream.getMatchingDevices(selectAudioDevice.selectedValue).at(0);
        selectSampleRate.clear();
        for (int i = 0; i < device.sampleRates.size(); i++) {
            selectSampleRate.add(device.sampleRates.at(i));
        }
        audioSetup();
    }
    if (apis[selectApi.selectedValue.toString()] != api) {
        api = apis[selectApi.selectedValue.toString()];
        audioSetup();
    }
    if (selectSampleRate.selectedValue.toString() != ofToString(sampleRate)) {
        sampleRate = ofToFloat(selectSampleRate.selectedValue.toString());
        audioSetup();
    }
    if (lastBufferSize != bufferSizeInput) {
        bufferSize = bufferSizeInput;
        audioSetup();
        lastBufferSize = bufferSize;
    }
    if (selectMidiPorts.getAllSelected().size() != midiPorts.size()) {
        midiPorts = selectMidiPorts.getAllSelected();
        midiSetup();
    }
    else {
        for (int i = 0; i < selectMidiPorts.getAllSelected().size(); i++) {
            if (selectMidiPorts.getAllSelected().at(i) != midiPorts.at(i)) {
                midiPorts = selectMidiPorts.getAllSelected();
                midiSetup();
                break;
            }
        }
    }
    if (selectMidiChannels.getAllSelected().size() != midiChannels.size()) {
        midiChannels = selectMidiChannels.getAllSelected();
    }
    else {
        for (int i = 0; i < selectMidiChannels.getAllSelected().size(); i++) {
            if (selectMidiChannels.getAllSelected().at(i) != midiChannels.at(i)) {
                midiChannels = selectMidiChannels.getAllSelected();
                break;
            }
        }
    }
    if (midiLoop) {
        timeInput = true;
    }
}

void ofApp::draw() {
    gui.draw();
}

void ofApp::windowResized(int w, int h) {
    resizeGui();
}

void ofApp::ofSoundStreamSetup(ofSoundStreamSettings settings) {

}

void ofApp::audioOut(ofSoundBuffer& buffer) {
    if (selectFrequencyUnit.selectedValue.toString() != frequencyUnit) {
        if (frequencyInput) {
            getFrequency();
        }
        frequencyUnit = selectFrequencyUnit.selectedValue.toString();
    }
    if (selectTimeUnit.selectedValue.toString() != timeUnit) {
        if (timeInput) {
            getTime();
        }
        timeUnit = selectTimeUnit.selectedValue.toString();
    }
    if (selectBoundary.selectedValue.toString() != boundaryMode) {
        getBoundary();
        boundaryMode = selectBoundary.selectedValue.toString();
    }
    if (lastTuningMIDIInput != tuningMIDIInput || lastTuningFrequencyInput != tuningFrequencyInput) {
        if (!frequencyInput || midiFrequency || midiBoundary) {
            reset();
        }
    }
    if (lastFrequencyInputState != frequencyInput) {
        reset();
        if (!frequencyInput) {
            midiFrequency = true;
        }
        lastFrequencyInputState = frequencyInput;
    }
    if (lastTimeInputState != timeInput) {
        reset();
        lastTimeInputState = timeInput;
    }
    if (lastSetInput != (string)setInput) {
        reset();
        lastSetInput = (string)setInput;
    }
    if (lastManualFrequencyInput != manualFrequencyInput && frequencyInput) {
        getFrequency();
        lastManualFrequencyInput = manualFrequencyInput;
    }
    if (lastManualTimeInput != manualTimeInput && timeInput) {
        getTime();
        lastManualTimeInput = manualTimeInput;
    }
    if (lastManualBoundaryInput != boundaryInput) {
        getBoundary();
        lastManualBoundaryInput = boundaryInput;
    }

    if (!audioInstanstiation) {
        scoreSetup();
    }

    for (int a = 0; a < buffer.getNumFrames(); a++) {
        float sample = 0;
        if (midiTimer) {
            duration++;
        }
        else {
            if (render) {
                sample = getSample();
                updateTime();
            }
        }
        for (int b = 0; b < buffer.getNumChannels(); b++) {
            buffer[a * buffer.getNumChannels() + b] = sample;
        }
    }
}

void ofApp::newMidiMessage(ofxMidiMessage& message) {
    ofxMidiMessage virtualMessage;
    for (int i = 0; i < midiChannels.size(); i++) {
        if (message.channel == midiChannels.at(i)) {
            virtualMessage = message;
        }
    }
    if (virtualMessage.status == MIDI_NOTE_ON || midiTimer == false) {
        if (!timeInput) {
            startMidiTimer();
        }
        else {
            if (!frequencyInput) {
                fundamentalFrequency = midiToFrequency(virtualMessage.pitch);
                reset();
            }
            if (midiRender) {
                render = true;
            }
            if (midiLoop) {
                loop = true;
            }
        }
    }
    if (virtualMessage.status == MIDI_NOTE_OFF) {
        if (timeInput || midiLoop) {
            loop = false;
        }
        else {
            stopMidiTimer();
            if (!frequencyInput) {
                fundamentalFrequency = midiToFrequency(virtualMessage.pitch);
                reset();
            }
            else {
                reset();
                if (midiRender) {
                    render = true;
                }
            }
        }
    }
}

void ofApp::midiSetup() {
    for (int i = 0; i < midiDevices.size(); i++) {
        if (midiDevices.at(i) != midiPorts.at(i)) {
            midiIn.closePort();
            break;
        }
    }
    for (int i = 0; i < midiPorts.size(); i++) {
        midiIn.openPort(midiPorts.at(i));
    }
}

void ofApp::getFrequency() {
    std::map<std::string, int> frequencyMap{ {"Hertz (Hz)", 1}, {"MIDI Note Number", 2}, {"Ratio to Sample Rate (typically < 1)", 3}, {"Period (seconds)", 4}, {"Period (samples)", 5} };
    switch (frequencyMap[selectFrequencyUnit.selectedValue.toString()]) {
    case 1: {
        midiFrequency = false;
        fundamentalFrequency = manualFrequencyInput;
        break;
    }
    case 2: {
        midiFrequency = true;
        fundamentalFrequency = midiToFrequency(manualFrequencyInput);
        break;
    }
    case 3: {
        midiFrequency = false;
        fundamentalFrequency = manualFrequencyInput * sampleRate;
        break;
    }
    case 4: {
        midiFrequency = false;
        fundamentalFrequency = 1.0 / manualFrequencyInput;
        break;
    }
    case 5: {
        midiFrequency = false;
        fundamentalFrequency = sampleRate / manualFrequencyInput;
        break;
    }
    }
    reset();
}

void ofApp::getTime() {
    std::map<string, int> timeMap{ {"Seconds", 1}, {"Samples", 2}, {"Cycles", 3} };
    switch (timeMap[selectTimeUnit.selectedValue.toString()]) {
    case 1: {
        fundamentalTime = manualTimeInput * sampleRate;
        break;
    }
    case 2: {
        fundamentalTime = manualTimeInput;
        break;
    }
    case 3: {
        fundamentalTime = manualTimeInput * sampleRate / fundamentalFrequency;
        break;
    }
    }
    reset();
}

void ofApp::getBoundary() {
    std::map<string, int> boundaryMap{ {"No Boundary", 1}, {"Frequency: Hertz (Hz) or Period (seconds)", 2 }, {"Frequency: MIDI Note Number", 3}, {"Frequency: Ratio to Sample Rate (typically < 1)", 4}, {"Duration: Seconds", 5}, {"Duration: Samples", 6}, {"Duration: Cycles", 7}, {"Ratio", 8} };
    switch (boundaryMap[selectBoundary.selectedValue.toString()]) {
    case 1: {
        midiBoundary = false;
        isBoundary = false;
        break;
    }
    case 2: {
        midiBoundary = false;
        isBoundary = true;
        boundaryRatio = boundaryInput / fundamentalFrequency;
        break;
    }
    case 3: {
        midiBoundary = true;
        isBoundary = true;
        boundaryRatio = midiToFrequency(boundaryInput) / fundamentalFrequency;
        break;
    }
    case 4: {
        midiBoundary = false;
        isBoundary = true;
        boundaryRatio = boundaryInput * sampleRate / fundamentalFrequency;
        break;
    }
    case 5: {
        midiBoundary = false;
        isBoundary = true;
        boundaryRatio = boundaryInput * sampleRate / fundamentalTime;
        break;
    }
    case 6: {
        midiBoundary = false;
        isBoundary = true;
        boundaryRatio = boundaryInput / fundamentalTime;
        break;
    }
    case 7: {
        midiBoundary = false;
        isBoundary = true;
        boundaryRatio = boundaryInput * sampleRate * fundamentalFrequency / fundamentalTime;
        break;
    }
    case 8: {
        midiBoundary = false;
        isBoundary = true;
        boundaryRatio = boundaryInput;
        break;
    }
    }
    if (boundaryRatio < 1) {
        boundaryRatio = 1.0 / boundaryRatio;
    }
    reset();
}

void ofApp::audioSetup() {
    streamSettings.setOutListener(this);
    stream.setDevice(device);
    numberOfChannels = device.outputChannels;
    streamSettings.setApi(api);
    sampleRate = 48000;
    streamSettings.sampleRate = sampleRate;
    streamSettings.numInputChannels = 0;
    streamSettings.numOutputChannels = numberOfChannels;
    streamSettings.bufferSize = bufferSize;
    stream.setup(streamSettings);
}

float ofApp::midiToFrequency(float noteNumber) {
    return tuningFrequencyInput * glm::pow(2.0, (noteNumber - tuningMIDIInput) / 12.0);
}

void ofApp::scoreSetup() {
    bipolarSet = getSet((string)setInput);
    size = bipolarSet.size();
    unipolarSet = getSetMagnitudes(bipolarSet);
    scaledSet = scaleSet(unipolarSet);
    if (isBoundary) {
        scaledSet = rescaleSet(boundaryRatio, scaledSet);
    }

    vector<string> setComponents;
    for (int i = 0; i < size; i++) {
        setComponents.push_back(ofToString(bipolarSet.at(i)));
    }
    lastSetInput = ofJoinString(setComponents, " ");
    lastManualFrequencyInput = manualFrequencyInput;
    lastManualTimeInput = manualTimeInput;
    lastManualBoundaryInput = boundaryInput;
    lastRenderState = render;
    for (int i = 0; i < size; i++) {
        indicies.push_back(0);
        times.push_back(fundamentalTime * scaledSet.at(i));
        sinOsc oscillator(fundamentalFrequency * scaledSet.at(i), 0, 1.0 / size, sampleRate);
        oscillators.push_back(oscillator);
    }
    audioInstanstiation = true;
}

void ofApp::clearScore() {
    indicies.clear();
    times.clear();
    oscillators.clear();
    bipolarSet.clear();
    unipolarSet.clear();
    scaledSet.clear();
    times.clear();
    indicies.clear();
}

void ofApp::reset() {
    clearScore();
    scoreSetup();
};

vector<float> ofApp::getSet(string input) {
    vector<string> setInput;
    vector<float> set;
    setInput = ofSplitString(input, " ");
    for (int i = 0; i < setInput.size(); i++) {
        set.push_back(ofToFloat(setInput.at(i)));
    }
    size = set.size();
    return set;
}

vector<float> ofApp::getSetMagnitudes(vector<float> bipolarSet) {
    for (int i = 0; i < size; i++) {
        bipolarSet.at(0) = abs(bipolarSet.at(0));
    }
    return bipolarSet;
}

vector<float> ofApp::scaleSet(vector<float> unipolarSet) {
    float sum = 0;
    float maximum = unipolarSet.at(0);
    minimum = unipolarSet.at(0);
    maximumIndex = 0;
    for (int i = 0; i < size; i++) {
        sum += unipolarSet.at(i);
        if (unipolarSet.at(i) < minimum) {
            minimum = unipolarSet.at(i);
        }
        else {
            if (unipolarSet.at(i) > maximum) {
                maximumIndex = i;
            }
        }
    }
    float multiplier = (float)size / sum;
    for (int i = 0; i < size; i++) {
        unipolarSet.at(i) *= multiplier;
    }
    return unipolarSet;
}

vector<float> ofApp::rescaleSet(float boundaryRatio, vector<float> scaledSet) {
    float positiveMaximum = 0;
    float negativeMaximum = 0;
    float originalMaximum;
    float scale;
    for (int a = 0; a < size; a++) {
        float positiveLocalMaximum = 0;
        float negativeLocalMaximum = 0;
        float testRatio;
        for (int b = 0; b < size; b++) {
            testRatio = scaledSet.at(a) / scaledSet.at(b);
            if (testRatio < 1) {
                testRatio = -1.0 / testRatio;
                if (testRatio < negativeLocalMaximum) {
                    negativeLocalMaximum = testRatio;
                }
            }
            else {
                if (testRatio > positiveLocalMaximum) {
                    positiveLocalMaximum = testRatio;
                }
            }
        }
        positiveMaximum += positiveLocalMaximum;
        negativeMaximum -= negativeLocalMaximum;
    }
    if (negativeMaximum > positiveMaximum) {
        originalMaximum = negativeMaximum;
    }
    else {
        originalMaximum = positiveMaximum;
    }
    scale = boundaryRatio / originalMaximum;
    for (int i = 0; i < size; i++) {
        scaledSet.at(i) *= scale;
    }
    return scaledSet;
}

float ofApp::interpolateMultiplier(float current, float next, float elapsedTime, float totalTime) {
    float interpolationAngle;
    interpolationAngle = ofMap(elapsedTime, 0.0, totalTime, -1.0 * TWO_PI, TWO_PI);
    float scaledMultiplier = glm::tanh(interpolationAngle);
    scaledMultiplier = ofMap(scaledMultiplier, -1, 1, current, next);
    return scaledMultiplier;
}

float ofApp::getSample() {
    float multiplier = 1.0;
    float sample = 0;
    float frequency;
    for (int i = 0; i < size; i++) {
        multiplier *= interpolateMultiplier(scaledSet.at(indicies.at(i)), scaledSet.at(indicies.at((i + 1) % size)), times.at(i), scaledSet.at(indicies.at(i)) * (fundamentalTime / sampleRate));
    }
    frequency = fundamentalFrequency * multiplier;
    for (int i = 0; i < size; i++) {
        oscillators.at(i).setFreq(frequency * scaledSet.at(i));
        sample += oscillators.at(i).getSample();
    }
    return sample;
}

void ofApp::updateTime() {
    for (int i = 0; i < size; i++) {
        times.at(i) -= 1.0;
        if (times.at(i) < 1.0) {
            if (i == maximumIndex && indicies.at(i) == size - 1 && !loop) {
                render = false;
                reset();
            }
            else {
                indicies.at(i) += 1;
                indicies.at(i) %= size;
                times.at(i) = times.at(i) + (fundamentalTime * scaledSet.at(indicies.at(i)));
            }
        }
    }
}

void ofApp::startMidiTimer() {
    duration = 0;
    midiTimer = true;
    render = false;
}

void ofApp::stopMidiTimer() {
    midiTimer = false;
    fundamentalTime = duration / sampleRate;
    if (midiRender) {
        render = true;
    }
}

void ofApp::resizeGui() {
    width = ofGetWidth() / 2.0;
    height = ofGetHeight() / 21.4375;
    guiShape.setWidth(width);
    guiShape.setHeight(height);
    gui.setShape(guiShape);
    gui.setWidthElements(width);
    gui.setSize(width, height);
    selectAudioDevice.setSize(width, height);
    selectAudioDevice.setDropdownElementsWidth(width);
    selectApi.setSize(width, height);
    selectApi.setDropdownElementsWidth(width);
    selectSampleRate.setSize(width, height);
    selectSampleRate.setDropdownElementsWidth(width);
    bufferSizeInput.setSize(width, height);
    selectMidiPorts.setSize(width, height);
    selectMidiPorts.setDropdownElementsWidth(width);
    selectMidiChannels.setSize(width, height);
    selectMidiChannels.setDropdownElementsWidth(width);
    tuningMIDIInput.setSize(width, height);
    tuningFrequencyInput.setSize(width, height);
    setInput.setSize(width, height);
    frequencyInput.setSize(width, height);
    selectFrequencyUnit.setSize(width, height);
    selectFrequencyUnit.setDropdownElementsWidth(width);
    manualFrequencyInput.setSize(width, height);
    timeInput.setSize(width, height);
    selectTimeUnit.setSize(width, height);
    selectTimeUnit.setDropdownElementsWidth(width);
    manualTimeInput.setSize(width, height);
    selectBoundary.setSize(width, height);
    selectBoundary.setDropdownElementsWidth(width);
    boundaryInput.setSize(width, height);
    midiRender.setSize(width, height);
    midiLoop.setSize(width, height);
    render.setSize(width, height);
    loop.setSize(width, height);
}

void ofApp::exit() {
    stream.close();
}
