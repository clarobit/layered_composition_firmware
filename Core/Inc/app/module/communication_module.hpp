// #pragma once

// #include "app/device/bluetooth.hpp"
// #include <stdint.h>

// namespace app::module {

// class CommunicationModule {
// public:
//   explicit CommunicationModule(app::device::Bluetooth &bluetooth);

//   void init();
//   void update();

//   bool hasCommand() const;
//   Command getCommand();

// private:
//   static constexpr uint8_t START_BYTE = 0xFF;
//   static constexpr uint8_t END_BYTE = 0xFE;
//   static constexpr uint8_t ESC_BYTE = 0xFD;

//   app::device::Bluetooth &bluetooth_;

//   uint8_t rx_byte_;

//   uint8_t buffer_[8];
//   uint8_t index_;

//   bool receiving_;
//   bool escape_next_;

//   bool command_ready_;

//   Command command_;

//   void parseByte(uint8_t data);
// };

// } // namespace app::module