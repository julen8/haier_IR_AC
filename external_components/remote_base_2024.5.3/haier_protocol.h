#pragma once

#include "esphome/core/component.h"
#include "esphome/core/helpers.h"
#include "esphome/components/remote_base/remote_base.h"
#include <array>
#include <utility>
#include <vector>

namespace esphome {
namespace remote_base {

class HaierData {
 public:
  // Make default
  HaierData() {}
  // Make from initializer_list
  HaierData(std::initializer_list<uint8_t> data) {
    std::copy_n(data.begin(), std::min(data.size(), this->data_.size()), this->data_.begin());
  }
  // Make from vector
  HaierData(const std::vector<uint8_t> &data) {
    std::copy_n(data.begin(), std::min(data.size(), this->data_.size()), this->data_.begin());
  }
  // Default copy constructor
  HaierData(const HaierData &) = default;

  uint8_t *data() { return this->data_.data(); }
  const uint8_t *data() const { return this->data_.data(); }
  uint8_t size() const { return this->data_.size(); }
  bool is_valid() const { return this->data_[OFFSET_CS] == this->calc_cs_(); }
  void finalize() { this->data_[OFFSET_CS] = this->calc_cs_(); }
  bool is_compliment(const HaierData &rhs) const;
  std::string to_string() const { return format_hex_pretty(this->data_.data(), this->data_.size()); }
  // compare only 40-bits
  bool operator==(const HaierData &rhs) const {
    return std::equal(this->data_.begin(), this->data_.begin() + OFFSET_CS, rhs.data_.begin());
  }
  enum HaierDataType : uint8_t {
    HAIER_TYPE_CONTROL = 0xA1,
    HAIER_TYPE_SPECIAL = 0xA2,
    HAIER_TYPE_FOLLOW_ME = 0xA4,
  };
  HaierDataType type() const { return static_cast<HaierDataType>(this->data_[0]); }
  template<typename T> T to() const { return T(*this); }
  uint8_t &operator[](size_t idx) { return this->data_[idx]; }
  const uint8_t &operator[](size_t idx) const { return this->data_[idx]; }

 protected:
  uint8_t get_value_(uint8_t idx, uint8_t mask = 255, uint8_t shift = 0) const {
    return (this->data_[idx] >> shift) & mask;
  }
  void set_value_(uint8_t idx, uint8_t value, uint8_t mask = 255, uint8_t shift = 0) {
    this->data_[idx] &= ~(mask << shift);
    this->data_[idx] |= (value << shift);
  }
  void set_mask_(uint8_t idx, bool state, uint8_t mask = 255) { this->set_value_(idx, state ? mask : 0, mask); }
  static const uint8_t OFFSET_CS = 13;
  // 48-bits data
  std::array<uint8_t, 14> data_;
  // Calculate checksum
  uint8_t calc_cs_() const;
};

class HaierProtocol : public RemoteProtocol<HaierData> {
 public:
  void encode(RemoteTransmitData *dst, const HaierData &src) override;
  optional<HaierData> decode(RemoteReceiveData src) override;
  void dump(const HaierData &data) override;
};

class HaierBinarySensor : public RemoteReceiverBinarySensorBase {
 public:
  bool matches(RemoteReceiveData src) override {
    auto data = HaierProtocol().decode(src);
    return data.has_value() && data.value() == this->data_;
  }
  void set_code(const std::vector<uint8_t> &code) { this->data_ = code; }

 protected:
  HaierData data_;
};

using HaierTrigger = RemoteReceiverTrigger<HaierProtocol>;
using HaierDumper = RemoteReceiverDumper<HaierProtocol>;

template<typename... Ts> class HaierAction : public RemoteTransmitterActionBase<Ts...> {
  TEMPLATABLE_VALUE(std::vector<uint8_t>, code)
  void set_code_static(std::vector<uint8_t> code) { code_static_ = std::move(code); }
  void set_code_template(std::function<std::vector<uint8_t>(Ts...)> func) { this->code_func_ = func; }

  void encode(RemoteTransmitData *dst, Ts... x) override {
    HaierData data;
    if (!this->code_static_.empty()) {
      data = HaierData(this->code_static_);
    } else {
      data = HaierData(this->code_func_(x...));
    }
    data.finalize();
    HaierProtocol().encode(dst, data);
  }

 protected:
  std::function<std::vector<uint8_t>(Ts...)> code_func_{};
  std::vector<uint8_t> code_static_{};
};

}  // namespace remote_base
}  // namespace esphome