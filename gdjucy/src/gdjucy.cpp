#include <gdjucy.hpp>
#include <format>
#include <cassert>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/engine.hpp>

namespace godot {

GDJucy::GDJucy()
{
  std::cout << "GDJucy Construct passed!" << std::endl;
	//time_passed = 0;
    //sequencer.emitSignal = std::bind(&GDJucy::emitSignal, this, std::placeholders::_1);

  // エディタで実行されているかを確認し、エディタ内では開始処理をしない
  if (godot::Engine::get_singleton()->is_editor_hint()) {
    UtilityFunctions::print("is_editor_hint() == True and exit constructor!");
	  return;
  }
  UtilityFunctions::print("is_editor_hint() == False.");

  // 音の再生用コールバック関数の登録 --------------------------------------
  UtilityFunctions::print("Setting audio callback function is start.");
  tone_generator.setFrequency(frequency);
  player.setSource(&tone_generator);
  adm.addAudioCallback(&player);
  UtilityFunctions::print("Setting audio callback function finished.");
  // --------------------------------------------------------------------



  // オーディオデバイスの初期化 ---------------------------------------------------

  const int request_device_type_id = 0;
  const int request_device_name_id = 0;

  // 有効なjuce::AudioIODeviceTypeの列挙
  //     出力の例 
  //     Windows Audio
  //     Windows Audio(Exclusive Mode)
  //     Windows Audio(Low Latency Mode)
  //     DirectSound
  //
  UtilityFunctions::print("Initialising audio device is start.");
  const auto& device_types = adm.getAvailableDeviceTypes();

  for (auto type : device_types ) {
	  UtilityFunctions::print( std::format("{} {}", "Available Output Channel:", type->getTypeName().toStdString() ).c_str());
  }

  UtilityFunctions::print("------------------------------------");
  assert(!device_types.isEmpty());

  for (auto& type : device_types) {
      // Windows Audio (Low Latency Mode)みたいな名前の出力
	  UtilityFunctions::print( std::format("{} {}", "", type->getTypeName().toStdString() ).c_str() );

	  // Available Device : OUT (UA-3FX)みたいな名前の出力
	  type->scanForDevices();
	  auto devices = type->getDeviceNames();
	  for (const auto& device : devices) {
		  //DBG("  Available Device: " + device);
		  UtilityFunctions::print( std::format("{} {}",  " Available Device : ", device.toStdString() ).c_str() );
    }
  }


  //adm.setCurrentAudioDeviceType("ASIO", true);

  // オーディオデバイスタイプ名（APIの名前）を指定して有効にする
  adm.setCurrentAudioDeviceType(
	  device_types[request_device_type_id]->getTypeName(), false
  );


  // 現在有効なデバイスタイプを得る場合
  //auto current_device_type = adm.getCurrentDeviceTypeObject();

  auto current_type = adm.getCurrentDeviceTypeObject();

  // 有効なデバイスの列挙（内部に蓄積する）
  current_type->scanForDevices();

  // 入出力デバイスの例 ---------------
  // [out] OUT(UA - 3FX)
  // [out] DELL S2722DC(NVIDIA High Definition Audio)
  // [in] IN(UA - 3FX)
  // ----------------------------------

  // 出力デバイス一覧の取得
  for (auto name : current_type->getDeviceNames()) {
	  UtilityFunctions::print( std::format("{} {}", "[out] ", name.toStdString() ).c_str() );
  }

  // 入力デバイス一覧の取得
  for (auto name : current_type->getDeviceNames(true)) {
	  UtilityFunctions::print( std::format("{} {}",  "[in] ", name.toStdString() ).c_str() );
  }


  // 具体的なオーディオデバイスのオープン処理 ----
  // デフォルトデバイスでお手軽に鳴らせる初期化
  //adm.initialiseWithDefaultDevices(0, 2); // 第一引数はinput, 第二引数はoutput

  juce::AudioDeviceManager::AudioDeviceSetup setup;		// セットアップ情報

  setup.outputChannels.setRange(0, 2, true);		// 0番目から2つ分、つまり0番目と1番目のチャンネルを有効にしている（ステレオ）
  setup.inputChannels.setRange(0, 2, true);			// 
  setup.bufferSize = 2048;		// 512は成功するがUA33fxではノイズが入る
  setup.sampleRate = 48000;
  setup.useDefaultOutputChannels = false;
  setup.useDefaultInputChannels = false;

  UtilityFunctions::print("Request Device Settings ----------------------");
  UtilityFunctions::print( std::format("{}: {}", "Request device_type_id", request_device_type_id ).c_str() );
  UtilityFunctions::print( std::format("{}: {}", "Request device_name_id", request_device_name_id ).c_str() );
  UtilityFunctions::print( std::format("{}: {}", "Request setup.outputChannels (binary digits)", setup.outputChannels.toString(2).toStdString() ).c_str() );
  UtilityFunctions::print( std::format("{}: {}", "Request setup.inputChannels (binary digits)", setup.inputChannels.toString(2).toStdString()).c_str() );
  UtilityFunctions::print( std::format("{}: {}", "Request setup.bufferSize", setup.bufferSize ).c_str() );
  UtilityFunctions::print( std::format("{}: {}", "Request setup.sampleRate", setup.sampleRate ).c_str() );
  UtilityFunctions::print( std::format("{}: {}", "Request setup.useDefaultOutputChannels", setup.useDefaultOutputChannels ).c_str() );
  UtilityFunctions::print( std::format("{}: {}", "Request setup.useDefaultInputChannels", setup.useDefaultInputChannels ).c_str() );
  UtilityFunctions::print("");

  auto device_type = device_types[request_device_type_id];
  setup.outputDeviceName = device_type->getDeviceNames()[request_device_name_id];
  setup.inputDeviceName = device_type->getDeviceNames(true)[request_device_name_id];

  UtilityFunctions::print( std::format("{}: {}", "Selected device type", device_type->getTypeName().toStdString() ).c_str() );
  UtilityFunctions::print( std::format("{}: {}", "Selected output device name", setup.outputDeviceName.toStdString() ).c_str() );
  UtilityFunctions::print( std::format("{}: {}", "Selected input device name", setup.inputDeviceName.toStdString() ).c_str() );

  UtilityFunctions::print("------------------------------------");
  UtilityFunctions::print("adm.setAudioDeviceSetup() start.");

  auto error_message = adm.setAudioDeviceSetup(setup, false);		// ここで実際にセットアップ

  if (error_message.isEmpty()) {
    UtilityFunctions::print("No Error Message.");
  } else {
    UtilityFunctions::print( std::format("{} {}", "Error Message:", error_message.toStdString() ).c_str() );
  }
  assert(error_message.isEmpty());

  // UtilityFunctions::dbg() を短縮

  UtilityFunctions::print("adm.setAudioDeviceSetup() is successed.");
  UtilityFunctions::print("Initialising audio device is finished.");
  UtilityFunctions::print("");
  // ここまでオーディオデバイスの初期化 ---------------------------------------------------

  // セットアップ後の現在のオーディオデバイス情報を取得 ---------------------------
  UtilityFunctions::print("------------------------------------");
  UtilityFunctions::print("Getting current audio device infomation is started.");

  auto current_device = adm.getCurrentAudioDevice();

  // デバイス名とデバイスタイプ名
  UtilityFunctions::print( std::format("{} {}", "Device Type Name:", current_device->getTypeName().toStdString() ).c_str() );
  UtilityFunctions::print( std::format("{} {}", "Device Name:", current_device->getName().toStdString() ).c_str() );
  UtilityFunctions::print("------------------------------------");

  // 有効な出力チャンネル名の取得
  for (auto name : current_device->getOutputChannelNames()) {
	  UtilityFunctions::print( std::format("{} {}", "Available Output Channel:", name.toStdString()).c_str() );
  }

  // 有効な入力チャンネル名の取得
  for (auto name : current_device->getInputChannelNames()) {
	  UtilityFunctions::print( std::format("{} {}", "Available Input Channel:", name.toStdString()).c_str() );
  }

  // 有効なサンプリングレートの取得（doubleに注意）
  for (double sampling_rate : current_device->getAvailableSampleRates()) {
	  UtilityFunctions::print( std::format("{} {}", "Available Sampling Rates:", std::round(sampling_rate)).c_str() );
  }

  // 有効なバッファサイズの取得
  for (auto buffer_size : current_device->getAvailableBufferSizes()) {
	  UtilityFunctions::print( std::format("{} {}", "Available Buffer Sizes:", buffer_size).c_str() );
  }

  UtilityFunctions::print("------------------------------------");

  // 分かりやすさのために再度デバイス名を表示
  UtilityFunctions::print( std::format("{} {}", "Device Type Name:", current_device->getTypeName().toStdString() ).c_str() );
  UtilityFunctions::print( std::format("{} {}", "Device Name:", current_device->getName().toStdString() ).c_str() );

  // デフォルトのバッファサイズの取得
  auto default_buffer_size = current_device->getDefaultBufferSize();
  UtilityFunctions::print( std::format("{} {}", "default_buffer_size:", default_buffer_size).c_str() );

  // 現在デバイスに設定されているバッファサイズの取得
  auto current_buffer_size = current_device->getCurrentBufferSizeSamples();
  UtilityFunctions::print( std::format("{} {}", "current_buffer_size:", current_buffer_size).c_str() );

  // 現在デバイスに設定されているサンプリングレートの取得
  double current_sampling_rate = current_device->getCurrentSampleRate();
  UtilityFunctions::print( std::format("{} {}", "current_sampling_rate:", current_sampling_rate).c_str() );

  // 現在デバイスに設定されているビット深度の取得
  auto bit_depth = current_device->getCurrentBitDepth();
  UtilityFunctions::print( std::format("{} {}", "getCurrentBitDepth:", bit_depth).c_str() );

  // "アクティブ"な出力/入力チャンネル情報の取得
  juce::BigInteger active_out_ch = current_device->getActiveOutputChannels();
  UtilityFunctions::print( std::format("{} {}", "getActiveOutputChannels:", active_out_ch.toString(2).toStdString() ).c_str() );
  juce::BigInteger active_in_ch = current_device->getActiveInputChannels();
  UtilityFunctions::print(std::format("{} {}", "getActiveInputChannels:", active_in_ch.toString(2).toStdString() ).c_str() );

  auto output_latency = current_device->getOutputLatencyInSamples();
  UtilityFunctions::print( std::format("{} {}", "getOutputLatencyInSamples:", output_latency).c_str() );

  auto input_latency = current_device->getInputLatencyInSamples();
  UtilityFunctions::print( std::format("{} {}", "getInputLatencyInSamples:", input_latency).c_str() );

    
  UtilityFunctions::print("Getting audio device infomation is finished.");

  //std::this_thread::sleep_for(std::chrono::seconds(5));

}

GDJucy::~GDJucy()
{
	// エディタで実行されているかを確認し、エディタ内では開始処理をしない
	if (godot::Engine::get_singleton()->is_editor_hint()) {
		return;
	}
  UtilityFunctions::print("Closing audio device is start.");

    //delete [] pcmBuf;
    //pcmBuf = nullptr;
	adm.closeAudioDevice();
  
  UtilityFunctions::print("Closing audio device is finished.");

}

void GDJucy::_process(double_t delta) {
  Node::_process(delta);

  // エディタで実行されているかを確認し、エディタ内では開始処理をしない
  if (godot::Engine::get_singleton()->is_editor_hint()) {
	  return;
  }

  tone_generator.setFrequency(frequency);

  process_movement(delta);

}

void GDJucy::process_movement(double_t delta) {
  m_Velocity = Vector2(0.0f, 0.0f);
  Input& intutSingleton = *Input::get_singleton();

  if (intutSingleton.is_action_pressed("ui_right")) {
	m_Velocity.x += 1.0f;
  }

  if (intutSingleton.is_action_pressed("ui_left")) {
	m_Velocity.x -= 1.0f;
  }

  if (intutSingleton.is_action_pressed("ui_up")) {
	m_Velocity.y -= 1.0f;

  }

  if (intutSingleton.is_action_pressed("ui_down")) {
	m_Velocity.y += 1.0f;
  }

  set_position(get_position() + (m_Velocity * m_Speed * delta));
}

void GDJucy::_bind_methods() {
  UtilityFunctions::print("Binding methods start.");

  ClassDB::bind_method(D_METHOD("get_speed"), &GDJucy::get_speed);
  ClassDB::bind_method(D_METHOD("set_speed", "speed"), &GDJucy::set_speed);

  ClassDB::bind_method(D_METHOD("set_frequency", "frequency"), &GDJucy::setFrequency);
  ClassDB::bind_method(D_METHOD("get_frequency"), &GDJucy::getFrequency);

  ADD_GROUP("GDJucy", "movement_");
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "movement_speed"), "set_speed", "get_speed");

  UtilityFunctions::print("Binding methods finished.");
}


}