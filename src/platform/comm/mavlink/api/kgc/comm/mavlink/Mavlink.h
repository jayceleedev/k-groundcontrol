// [0.1.0 T1–T6] specs/features/0.1.0-link.md
#pragma once
#define MAVLINK_COMM_NUM_BUFFERS 16
#define MAVLINK_SEPARATE_HELPERS
#include <ardupilotmega/mavlink.h>
#ifdef KGC_MAVLINK_IMPLEMENTATION
#include <mavlink_helpers.h>
#endif
#include <QMetaType>
Q_DECLARE_METATYPE(mavlink_message_t)
