#include <thread>
#include <iostream>
#include <ableton/Link.hpp>
#include "ableton/AudioPlatform_Dummy.hpp"
#include <cmath>
#include <iomanip>

struct State
{
  std::atomic<bool> running;
  ableton::Link link;
  ableton::linkaudio::AudioPlatform audioPlatform;

  State()
    : running(true)
    , link(120.)
    , audioPlatform(link)
  {
  }
};

namespace metronome {
    void starto();
}