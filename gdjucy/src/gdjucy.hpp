#pragma once

#include <cstdint>
#include <godot_cpp/godot.hpp>
#include <godot_cpp/classes/node2d.hpp>
// #include <juce_events/juce_events.h>
// #include <juce_audio_basics/juce_audio_basics.h>

/*
These define doesn't work in juce_audio_devices.h for some reason.
Especially problematic when using ASIO.
(But it is effective to directly rewrite "juce_audio_devices.h")

#define JUCE_ASIO 1
#define JUCE_WASAPI 0
#define JUCE_WASAPI_EXCLUSIVE 0
#define JUCE_WASAPI_SHARED 0
#define JUCE_DIRECTSOUND 0
*/

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>
#include <juce_events/juce_events.h>
#include <juce_graphics/juce_graphics.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_gui_extra/juce_gui_extra.h>

#if defined (JUCE_PROJUCER_VERSION) && JUCE_PROJUCER_VERSION < JUCE_VERSION
/** If you've hit this error then the version of the Projucer that was used to generate this project is
    older than the version of the JUCE modules being included. To fix this error, re-save your project
    using the latest version of the Projucer or, if you aren't using the Projucer to manage your project,
    remove the JUCE_PROJUCER_VERSION define.
*/
#error "This project was last saved using an outdated version of the Projucer! Re-save this project with the latest version to fix this error."
#endif

#if ! DONT_SET_USING_JUCE_NAMESPACE
// If your code uses a lot of JUCE classes, then this will obviously save you
// a lot of typing, but can be disabled by setting DONT_SET_USING_JUCE_NAMESPACE.
using namespace juce;
#endif

#if ! JUCE_DONT_DECLARE_PROJECTINFO
namespace ProjectInfo
{
    const char* const  projectName = "GDJucy";
    const char* const  companyName = "Individual";
    const char* const  versionString = "0.0.1";
    const int          versionNumber = 0x10000;
}
#endif


namespace godot {

class GDJucy : public Node2D {
  GDCLASS(GDJucy, Node2D)

 public:

  GDJucy();
  ~GDJucy();

  // Will be called by Godot when the class is registered
  // Use this to add properties to your class
  static void _bind_methods();

  void _process(double_t delta) override;

  // property setter
  void set_speed(float_t speed) {
	m_Speed = speed;
  }

  // property getter
  [[nodiscard]] float_t get_speed() const {
	return m_Speed;
  }

  // property setter
  void setFrequency(float_t speed) {
      frequency = speed;
  }

  // property getter
  [[nodiscard]] float_t getFrequency() const {
      return frequency;
  }


 private:
  Vector2 m_Velocity;

  // This will be a property later (look into _bind_methods)
  float_t m_Speed = 500.0f;
  double frequency = 440.0;
  void process_movement(double_t delta);

  juce::AudioDeviceManager adm;
  juce::ToneGeneratorAudioSource tone_generator;
  juce::AudioSourcePlayer player;
};

}