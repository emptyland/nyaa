#pragma once
#ifndef NYAA_GAME_CONSTANTS_H_
#define NYAA_GAME_CONSTANTS_H_

#include <stdint.h>

namespace nyaa {

static constexpr int kRegionSize = 1024;

static constexpr int kTerrainMaxLevels = 16;

static constexpr int kTerrainSurfaceLevel = 0;

static constexpr int kDefaultViewportSize = 50;

// Terrain zone render
static constexpr double kDefaultZoneRotateAngleY = 45;

static constexpr double kDefaultZoneRotateAngleZ = 45;

static constexpr double kDefaultZoneScale = 0.08;

// enum Direction { kNone, kE, kS, kW, kN, kSE, kNE, kSW, kNW };

}  // namespace nyaa

#endif  // NYAA_GAME_CONSTANTS_H_