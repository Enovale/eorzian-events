/*
 * Copyright (c) 2015 Pebble Technology
 */

#pragma once

#include <pebble.h>

typedef enum {
  EVENT_TYPE_MINING,
  EVENT_TYPE_BOTANY,
  EVENT_TYPE_FISHING,
  EVENT_TYPE_SIGHTSEEING
} WeatherAppIcon;

GDrawCommandImage *weather_app_resources_get_icon(WeatherAppIcon icon);
