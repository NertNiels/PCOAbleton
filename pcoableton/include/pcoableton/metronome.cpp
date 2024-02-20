#include "metronome.h"

void clearLine()
{
  std::cout << "   \r" << std::flush;
  std::cout.fill(' ');
}

void printHelp()
{
  std::cout << std::endl << " < L I N K  H U T >" << std::endl << std::endl;
  std::cout << "usage:" << std::endl;
  std::cout << "  enable / disable Link: a" << std::endl;
  std::cout << "  start / stop: space" << std::endl;
  std::cout << "  decrease / increase tempo: w / e" << std::endl;
  std::cout << "  decrease / increase quantum: r / t" << std::endl;
  std::cout << "  enable / disable start stop sync: s" << std::endl;
  std::cout << "  quit: q" << std::endl << std::endl;
}

void printStateHeader()
{
  std::cout
    << "enabled | num peers | quantum | start stop sync | tempo   | beats   | metro"
    << std::endl;
}

void printState(const std::chrono::microseconds time,
  const ableton::Link::SessionState sessionState,
  const bool linkEnabled,
  const std::size_t numPeers,
  const double quantum,
  const bool startStopSyncOn)
{
  using namespace std;
  const auto enabled = linkEnabled ? "yes" : "no";
  const auto beats = sessionState.beatAtTime(time, quantum);
  const auto phase = sessionState.phaseAtTime(time, quantum);
  const auto startStop = startStopSyncOn ? "yes" : "no";
  const auto isPlaying = sessionState.isPlaying() ? "[playing]" : "[stopped]";
  cout << defaultfloat << left << setw(7) << enabled << " | " << setw(9) << numPeers
       << " | " << setw(7) << quantum << " | " << setw(3) << startStop << " " << setw(11)
       << isPlaying << " | " << fixed << setw(7) << sessionState.tempo() << " | " << fixed
       << setprecision(2) << setw(7) << beats << " | ";
  for (int i = 0; i < ceil(quantum); ++i)
  {
    if (i < phase)
    {
      std::cout << 'X';
    }
    else
    {
      std::cout << 'O';
    }
  }
  clearLine();
}


void input(State& state)
{
  char in;

  for (;;)
  {
#if defined(LINK_PLATFORM_WINDOWS)
    HANDLE stdinHandle = GetStdHandle(STD_INPUT_HANDLE);
    DWORD numCharsRead;
    INPUT_RECORD inputRecord;
    do
    {
      ReadConsoleInput(stdinHandle, &inputRecord, 1, &numCharsRead);
    } while ((inputRecord.EventType != KEY_EVENT) || inputRecord.Event.KeyEvent.bKeyDown);
    in = inputRecord.Event.KeyEvent.uChar.AsciiChar;
#elif defined(LINK_PLATFORM_UNIX)
    in = static_cast<char>(std::cin.get());
#endif

    const auto tempo = state.link.captureAppSessionState().tempo();
    auto& engine = state.audioPlatform.mEngine;

    switch (in)
    {
    case 'q':
      state.running = false;
      clearLine();
      return;
    case 'a':
      state.link.enable(!state.link.isEnabled());
      break;
    case 'w':
      engine.setTempo(tempo - 1);
      break;
    case 'e':
      engine.setTempo(tempo + 1);
      break;
    case 'r':
      engine.setQuantum(engine.quantum() - 1);
      break;
    case 't':
      engine.setQuantum(std::max(1., engine.quantum() + 1));
      break;
    case 's':
      engine.setStartStopSyncEnabled(!engine.isStartStopSyncEnabled());
      break;
    case ' ':
      if (engine.isPlaying())
      {
        engine.stopPlaying();
      }
      else
      {
        engine.startPlaying();
      }
      break;
    }
  }
}

void metronome::starto() {
  State state;
  printHelp();
  printStateHeader();
  std::thread thread(input, std::ref(state));

  while (state.running)
  {
    const auto time = state.link.clock().micros();
    auto sessionState = state.link.captureAppSessionState();
    printState(time, sessionState, state.link.isEnabled(), state.link.numPeers(),
      state.audioPlatform.mEngine.quantum(),
      state.audioPlatform.mEngine.isStartStopSyncEnabled());
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
  thread.join();
}