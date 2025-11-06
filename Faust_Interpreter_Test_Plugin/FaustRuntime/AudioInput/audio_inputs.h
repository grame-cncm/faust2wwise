#ifndef AUDIO_INPUTS_H
#define AUDIO_INPUTS_H

#include "Generators/sine440.cpp"
#include "Generators/wind.cpp"
#include "Generators/complexRain.cpp"
#include "Generators/fmSynthAnalog.cpp"
#include "Generators/insects.cpp"
#include "Generators/windchimes.cpp"
#include "Generators/waveSynthAnalog.cpp"

#define AUDIO_INPUT_LIST \
    X(Sine440, L"Sine wave (440Hz)") \
    X(Wind, L"Wind") \
    X(ComplexRain, L"Complex Rain") \
    X(FMSynthAnalog, L"FMSynthAnalog") \
    X(Insects, L"Insects") \
    X(WaveSynthAnalog, L"WaveSynthAnalog") \
    X(Windchimes, L"Windchimes")

#define DEFAUT_AUDIO_INPUT_FOR_EFFECTS 0

#include <string>
#include <vector>
#include <utility>

typedef std::vector<std::pair<int, const wchar_t*>> audioInputList;

namespace AudioInputs{

    audioInputList getAudioInputList();

    dsp* createAudioInputById(int id);
}

#endif