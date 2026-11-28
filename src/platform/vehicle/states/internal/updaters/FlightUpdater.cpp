// [0.6.0 5.4 T2] specs/features/0.6.0-fly.md
#include "FlightUpdater.h"
namespace kgc::vehicle::states {
FlightBundle updateFlight(const mavlink_message_t& message)
{
    mavlink_extended_sys_state_t state{};
    mavlink_msg_extended_sys_state_decode(&message, &state);
    FlightBundle bundle;
    bundle.landedState = state.landed_state;
    bundle.vtolState = state.vtol_state;
    return bundle;
}
}
