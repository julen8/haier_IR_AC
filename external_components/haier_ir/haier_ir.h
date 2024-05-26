#pragma once

#include "esphome/components/climate_ir/climate_ir.h"
#include "haier_data.h"

namespace esphome {
namespace haier_ir {

// Temperature
const uint8_t HAIER_TEMPC_MIN = 16;  // Celsius
const uint8_t HAIER_TEMPC_MAX = 30;  // Celsius
// const uint8_t haier_TEMPF_MIN = 61;  // Fahrenheit
// const uint8_t haier_TEMPF_MAX = 86;  // Fahrenheit

class HaierIR : public climate_ir::ClimateIR {
 public:
  HaierIR()
      : climate_ir::ClimateIR(
            HAIER_TEMPC_MIN, HAIER_TEMPC_MAX, 1.0f, true, true,
            {climate::CLIMATE_FAN_AUTO, climate::CLIMATE_FAN_LOW, climate::CLIMATE_FAN_MEDIUM,
             climate::CLIMATE_FAN_HIGH},
            {climate::CLIMATE_SWING_OFF, climate::CLIMATE_SWING_VERTICAL, climate::CLIMATE_SWING_HORIZONTAL, climate::CLIMATE_SWING_BOTH}
            ) {}

  /// Override control to change settings of the climate device.
  void control(const climate::ClimateCall &call) override;

  /// Set use of Fahrenheit units
  // void set_fahrenheit(bool value) {
  //   this->fahrenheit_ = value;
  //   this->temperature_step_ = value ? 0.5f : 1.0f;
  // }

 protected:
  /// Transmit via IR the state of this climate controller.
  void transmit_state() override;
  void transmit_(HaierData &data);
  /// Handle received IR Buffer
  bool on_receive(remote_base::RemoteReceiveData data) override;
  bool on_haier_(const HaierData &data);
  // bool fahrenheit_{false};
  // bool swing_{false};
};

}  // namespace haier_ir
}  // namespace esphome
