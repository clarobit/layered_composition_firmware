#include "app/device/switch.hpp"

namespace app::device {

Switch::Switch(GPIO_TypeDef *port, uint16_t pin) : gpio_(port, pin) {}

bool Switch::isPressed() const { return gpio_.read(); }

} // namespace app::device