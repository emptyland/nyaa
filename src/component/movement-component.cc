#include "component/movement-component.h"
#include <math.h>

namespace nyaa {

namespace com {

float MovementComponent::GetMovingDirection() const {
    if (is_horizontal_stop()) {
        return 0;
    }
    return ::atan2f(speed().y, speed().x);
}

}  // namespace com

}  // namespace nyaa
