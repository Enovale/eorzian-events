/*
 * Copyright (c) 2015 Pebble Technology
 */

#pragma once

#include <pebble.h>
#include "weather_app_data.h"

Animation *weather_app_create_view_model_animation_numbers(EventsCardsMainWindowViewModel *view_model, EventDataPoint *next_data_point);

Animation *weather_app_create_view_model_animation_bgcolor(EventsCardsMainWindowViewModel *view_model, EventDataPoint *next_data_point);

Animation *weather_app_create_view_model_animation_icon(EventsCardsMainWindowViewModel *view_model, EventDataPoint *next_data_point, uint32_t duration);
