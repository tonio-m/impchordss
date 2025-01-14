#pragma once
#include <array>
#include <vector>
#include "RtMidi.h"

struct MidiHandler {
    RtMidiIn* midiin = nullptr;
    inline static std::array<int, 36> pianoNotesActive = {};
    inline static std::array<std::string, 12> noteStrings = {"c", "c#", "d", "d#", "e", "f", "f#", "g", "g#", "a", "a#", "b"};

    inline static std::array<RtMidi::Api, 4> midiDriverApis = {RtMidi::MACOSX_CORE, RtMidi::WINDOWS_MM, RtMidi::LINUX_ALSA, RtMidi::UNIX_JACK};
    inline static std::vector<std::string> midiDriverNames = {"MacOSX Core", "Windows MM", "Linux Alsa", "Linux JACK"};
    
    static void staticMidiCallback(double deltatime, std::vector<unsigned char>* message, void* userData) {
        if (message->size() > 1) {
            int noteIndex = static_cast<int>(message->at(1)) - 48;
            pianoNotesActive[noteIndex] = !pianoNotesActive[noteIndex];
        }
    }

    ~MidiHandler() {
        if (midiin) {
            delete midiin;
        }
    }

    void init(int midiDriverIndex) {
        if (midiin) {
            delete midiin;
        }
        midiin = new RtMidiIn(midiDriverApis[midiDriverIndex]);
        changePort(0);
    }

    std::vector<std::string> getMidiInputs() {
        std::vector<std::string> ports;
        if (midiin) {
            unsigned int nPorts = midiin->getPortCount();
            for (unsigned int i = 0; i < nPorts; i++) {
                ports.push_back(midiin->getPortName(i));
            }
        }
        return ports;
    }

    void changePort(unsigned int portNumber) {
        try {
            midiin->closePort();
            midiin->openPort(portNumber);
            midiin->setCallback(&staticMidiCallback);
            midiin->ignoreTypes(false, false, false);
        } catch (RtMidiError &error) {
            error.printMessage();
        }
    }

    static bool isNoteActive(int index) {
        return pianoNotesActive[index];
    }

    std::string activeNotesString() {
        std::string result;
        for (size_t i = 0; i < pianoNotesActive.size(); ++i) {
            if (pianoNotesActive[i]) {
                result += noteStrings[i%12] + " ";
            }
        }
        return result;
    }
};
