// [0.6.0 5.2] specs/features/0.6.0-fly.md
#pragma once
namespace kgc::app::constants {
// 공중에서도 시동을 끄게 하는 COMPONENT_ARM_DISARM param2 값이에요(ArduPilot·PX4 같음).
inline constexpr float kEmergencyStopMagic = 21196.0F;
}
