#include <gdjucy.hpp>
#include <format>
#include <cassert>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/engine.hpp>


// JUCE headers
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_formats/juce_audio_formats.h>

namespace godot {

GDJucy::GDJucy()
{
    std::cout << "GDJucy Constructor started!\n";

    // If running in the editor, do nothing. We don't initialize audio here.
    if (godot::Engine::get_singleton()->is_editor_hint()) {
        UtilityFunctions::print("Running inside the Godot editor: audio will not be initialized.");
        return;
    }

    UtilityFunctions::print("Constructor: Attempting audio initialization...");
    initialize_audio_device();
    UtilityFunctions::print("Constructor: Audio initialization complete.");
}

GDJucy::~GDJucy()
{
    // If running in the editor, do nothing.
    if (godot::Engine::get_singleton()->is_editor_hint()) {
        return;
    }

    UtilityFunctions::print("Destructor: Closing audio device...");
    adm.closeAudioDevice(); 
    UtilityFunctions::print("Destructor: Audio device closed.");
}

void GDJucy::_bind_methods()
{
    UtilityFunctions::print("Binding methods...");

    ClassDB::bind_method(D_METHOD("get_speed"), &GDJucy::get_speed);
    ClassDB::bind_method(D_METHOD("set_speed", "speed"), &GDJucy::set_speed);

    ClassDB::bind_method(D_METHOD("set_frequency", "frequency"), &GDJucy::setFrequency);
    ClassDB::bind_method(D_METHOD("get_frequency"), &GDJucy::getFrequency);

    ADD_GROUP("GDJucy", "movement_");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "movement_speed"), "set_speed", "get_speed");

    UtilityFunctions::print("Binding methods finished.");
}

void GDJucy::_process(double_t delta)
{
    Node::_process(delta);

    // If running in the editor, do nothing.
    if (godot::Engine::get_singleton()->is_editor_hint()) {
        return;
    }

    // Update the frequency for our tone generator.
    tone_generator.setFrequency(frequency);

    // Handle movement input (unrelated to audio).
    process_movement(delta);
}

void GDJucy::process_movement(double_t delta)
{
    m_Velocity = Vector2(0.0f, 0.0f);
    Input& inputSingleton = *Input::get_singleton();

    if (inputSingleton.is_action_pressed("ui_right")) {
        m_Velocity.x += 1.0f;
    }
    if (inputSingleton.is_action_pressed("ui_left")) {
        m_Velocity.x -= 1.0f;
    }
    if (inputSingleton.is_action_pressed("ui_up")) {
        m_Velocity.y -= 1.0f;
    }
    if (inputSingleton.is_action_pressed("ui_down")) {
        m_Velocity.y += 1.0f;
    }

    set_position(get_position() + (m_Velocity * m_Speed * delta));
}

//------------------------------------------------------------------------------------
// Production-Ready Audio Initialization
//------------------------------------------------------------------------------------
void GDJucy::initialize_audio_device()
{
    UtilityFunctions::print("initialize_audio_device: Scanning for available devices...");

    // Register the audio callback so our tone generator can be heard.
    tone_generator.setFrequency(frequency);
    player.setSource(&tone_generator);
    adm.addAudioCallback(&player);

    // List available device types
    auto device_types = adm.getAvailableDeviceTypes();
    if (device_types.isEmpty()) {
        UtilityFunctions::print("No audio device types available! Audio cannot be initialized.");
        return;
    }

    UtilityFunctions::print("Available audio device types:");
    for (auto* type : device_types) {
        UtilityFunctions::print(std::format("   {}", type->getTypeName().toStdString()).c_str());
    }

    // --- PRODUCTION APPROACH: Use default devices, then selectively override if needed. ---
    // Initialize with default input = 0, output = 2 (stereo).
    {
        UtilityFunctions::print("Attempting to initialize with default devices (stereo output)...");
        const String error = adm.initialiseWithDefaultDevices(/* numInputChannels  = */ 0,
                                                              /* numOutputChannels = */ 2);

        if (!error.isEmpty()) {
            UtilityFunctions::print(std::format("Error initializing default devices: {}", 
                                                error.toStdString()).c_str());
            return;
        }
    }

    // Retrieve the newly active device and its setup
    auto* current_device = adm.getCurrentAudioDevice();
    if (current_device == nullptr) {
        UtilityFunctions::print("No current audio device found after default initialization!");
        return;
    }

    // Read back the default config
    double current_sample_rate     = current_device->getCurrentSampleRate();
    int    current_buffer_size     = current_device->getCurrentBufferSizeSamples();
    double default_sample_rate     = current_sample_rate;
    int    default_buffer_size     = current_buffer_size;
    auto   available_sample_rates  = current_device->getAvailableSampleRates();
    auto   available_buffer_sizes  = current_device->getAvailableBufferSizes();

    UtilityFunctions::print(std::format("Default device: {}", current_device->getName().toStdString()).c_str());
    UtilityFunctions::print(std::format("    Default sample rate: {}", default_sample_rate).c_str());
    UtilityFunctions::print(std::format("    Default buffer size: {}", default_buffer_size).c_str());

    // Example: If your project absolutely requires 48000, check if it's supported, otherwise keep default.
    const double desired_sample_rate = 48000.0; 
    if (std::find(available_sample_rates.begin(), 
                  available_sample_rates.end(), 
                  desired_sample_rate) != available_sample_rates.end())
    {
        // 48k is supported, let's set it. 
        current_sample_rate = desired_sample_rate;
    } else {
        UtilityFunctions::print(std::format(
            "48 kHz is not available. Using default rate: {} Hz",
            current_sample_rate).c_str());
    }

    // Example: If you want a buffer size close to 512, choose from available.
    int desired_buffer_size = 512; 
    if (std::find(available_buffer_sizes.begin(),
                  available_buffer_sizes.end(),
                  desired_buffer_size) != available_buffer_sizes.end())
    {
        current_buffer_size = desired_buffer_size;
    }
    else {
        UtilityFunctions::print(std::format(
            "{} samples buffer is not available. Using default buffer size: {}",
            desired_buffer_size, current_buffer_size).c_str());
    }

    // Construct the new setup from the current one.
    juce::AudioDeviceManager::AudioDeviceSetup setup;
    adm.getAudioDeviceSetup(setup);  // Start with existing defaults

    // Modify only what we need to change
    setup.sampleRate  = current_sample_rate;
    setup.bufferSize  = current_buffer_size;
    // The bit below just ensures we maintain current in/out channel activation:
    setup.useDefaultOutputChannels = true;
    setup.useDefaultInputChannels  = false;

    UtilityFunctions::print(std::format("Finalizing Audio Setup:\n"
                                        "    Sample Rate: {}\n"
                                        "    Buffer Size: {}",
                                        setup.sampleRate,
                                        setup.bufferSize).c_str());

    // Apply the changes
    const String error_message = adm.setAudioDeviceSetup(setup, /* treatAsChosenDevice */ true);
    if (!error_message.isEmpty()) {
        UtilityFunctions::print(std::format("Error applying audio device setup: {}",
                                            error_message.toStdString()).c_str());
        // In production, you may want to handle or fall back to a safe config here.
    } else {
        UtilityFunctions::print("Audio device setup applied successfully!");
    }

    // Double-check final configuration
    current_device = adm.getCurrentAudioDevice();
    if (current_device != nullptr) {
        UtilityFunctions::print(std::format("Current Device Name: {}",
                                            current_device->getName().toStdString()).c_str());
        UtilityFunctions::print(std::format("Current Sample Rate: {}",
                                            current_device->getCurrentSampleRate()).c_str());
        UtilityFunctions::print(std::format("Current Buffer Size: {}",
                                            current_device->getCurrentBufferSizeSamples()).c_str());
    }
}

//------------------------------------------------------------------------------------
// Getters/Setters for any custom properties
//------------------------------------------------------------------------------------
float GDJucy::get_speed() const
{
    return m_Speed;
}

void GDJucy::set_speed(float speed)
{
    m_Speed = speed;
}

float GDJucy::getFrequency() const
{
    return frequency;
}

void GDJucy::setFrequency(float freq)
{
    frequency = freq;
}

} // namespace godot