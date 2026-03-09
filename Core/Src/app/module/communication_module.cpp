// #include "app/module/communication_module.hpp"

// namespace app::module {

// CommunicationModule::CommunicationModule(app::device::Bluetooth &bluetooth)
//     : bluetooth_(bluetooth), rx_byte_(0), index_(0), receiving_(false),
//       escape_next_(false), command_ready_(false) {}

// void CommunicationModule::init() { bluetooth_.receiveIt(&rx_byte_, 1); }

// void CommunicationModule::update() {
//   uint8_t data = rx_byte_;

//   parseByte(data);

//   bluetooth_.receiveIt(&rx_byte_, 1);
// }

// void CommunicationModule::parseByte(uint8_t data) {
//   if (escape_next_) {
//     buffer_[index_++] = data;
//     escape_next_ = false;
//     return;
//   }

//   if (data == ESC_BYTE) {
//     escape_next_ = true;
//     return;
//   }

//   if (data == START_BYTE) {
//     receiving_ = true;
//     index_ = 0;
//     return;
//   }

//   if (!receiving_)
//     return;

//   if (data == END_BYTE) {
//     if (index_ >= 5) {
//       command_.x_angle = buffer_[0];
//       command_.y_angle = buffer_[1];
//       command_.up_speed = buffer_[2];
//       command_.down_speed = buffer_[3];
//       command_.drop = buffer_[4];

//       command_ready_ = true;
//     }

//     receiving_ = false;
//     index_ = 0;
//     return;
//   }

//   if (index_ < sizeof(buffer_)) {
//     buffer_[index_++] = data;
//   }
// }

// bool CommunicationModule::hasCommand() const { return command_ready_; }

// Command CommunicationModule::getCommand() {
//   command_ready_ = false;
//   return command_;
// }

// } // namespace app::module