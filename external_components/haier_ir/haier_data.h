#pragma once

#include "esphome/components/remote_base/haier_protocol.h"
#include "esphome/components/climate/climate_mode.h"

namespace esphome {
namespace haier_ir {

using climate::ClimateMode;
using climate::ClimateFanMode;
using climate::ClimateSwingMode;
using remote_base::HaierData;

class ControlData : public HaierData {
 public:
  // Default constructor (power: ON, mode: AUTO, fan: AUTO, temp: 25C)
  //ControlData() : HaierData({HAIER_TYPE_CONTROL, 0x82, 0x48, 0xFF, 0xFF}) {}
  ControlData() : HaierData({0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}) {}
  // Copy from Base
  ControlData(const HaierData &data) : HaierData(data) {}

  void set_temp(float temp);
  float get_temp() const;

  void set_mode(ClimateMode mode);
  ClimateMode get_mode() const;

  void set_fan_mode(ClimateFanMode mode);
  ClimateFanMode get_fan_mode() const;

  void set_swing_mode(ClimateSwingMode mode);
  ClimateSwingMode get_swing_mode() const;

  void set_other_data();

  // void set_sleep_preset(bool value) { this->set_mask_(1, value, 64); }
  // bool get_sleep_preset() const { return this->get_value_(1, 64); }

  // void set_fahrenheit(bool value) { this->set_mask_(2, value, 32); }
  // bool get_fahrenheit() const { return this->get_value_(2, 32); }

  // void fix();

 protected:
  enum Mode : uint8_t {
    MODE_COOL = 1,
    MODE_DRY = 2,
    MODE_HEAT = 4,
    MODE_FAN_ONLY = 6,
  };
  enum FanMode : uint8_t {
    FAN_AUTO = 5,
    FAN_LOW = 3,
    FAN_MEDIUM = 2,
    FAN_HIGH = 1,
  };

  enum SwingMode : uint8_t {
    SWING_OFF = 0,
    SWING_AUTO = 12,
    SWING_TOP = 2,
    SWING_BOTTOM = 10,
  };

  void set_fan_mode_(FanMode mode) { this->set_value_(5, mode, 7, 5); }
  FanMode get_fan_mode_() const { return static_cast<FanMode>(this->get_value_(5, 7, 5)); }
  void set_swing_mode_(SwingMode mode) { this->set_value_(1, mode, 15, 0); }
  SwingMode get_swing_mode_() const { return static_cast<SwingMode>(this->get_value_(1, 15, 0)); }
  void set_mode_(Mode mode) { this->set_value_(7, mode, 7, 5); }
  Mode get_mode_() const { return static_cast<Mode>(this->get_value_(7, 7, 5)); }
  void set_power_(bool value) { this->set_mask_(4, value, 64); }
  bool get_power_() const { return this->get_value_(4, 64); }
};

// class FollowMeData : public HaierData {
//  public:
//   // Default constructor (temp: 30C, beeper: off)
//   FollowMeData() : HaierData({HAIER_TYPE_FOLLOW_ME, 0x82, 0x48, 0x7F, 0x1F}) {}
//   // Copy from Base
//   FollowMeData(const HaierData &data) : HaierData(data) {}
//   // Direct from temperature and beeper values
//   FollowMeData(uint8_t temp, bool beeper = false) : FollowMeData() {
//     this->set_temp(temp);
//     this->set_beeper(beeper);
//   }

//   /* TEMPERATURE */
//   uint8_t temp() const { return this->get_value_(4) - 1; }
//   void set_temp(uint8_t val) { this->set_value_(4, std::min(MAX_TEMP, val) + 1); }

//   /* BEEPER */
//   bool beeper() const { return this->get_value_(3, 128); }
//   void set_beeper(bool value) { this->set_mask_(3, value, 128); }

//  protected:
//   static const uint8_t MAX_TEMP = 37;
// };

// class SpecialData : public HaierData {
//  public:
//   SpecialData(uint8_t code) : HaierData({HAIER_TYPE_SPECIAL, code, 0xFF, 0xFF, 0xFF}) {}
//   static const uint8_t VSWING_STEP = 1;
//   static const uint8_t VSWING_TOGGLE = 2;
//   static const uint8_t TURBO_TOGGLE = 9;
// };

}  // namespace haier_ir
}  // namespace esphome
