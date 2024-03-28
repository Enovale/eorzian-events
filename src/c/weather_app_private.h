/*
 * Copyright (c) 2015 Pebble Technology
 */

#pragma once

#include <pebble.h>
#include "weather_app_data.h"

typedef struct {
  EventDataPoint *data_point;
  EventsCardsMainWindowViewModel view_model;
  Animation *previous_animation;
  TextLayer *fake_statusbar;
  TextLayer *pagination_layer;
  TextLayer *name_layer;
  Layer *horizontal_ruler_layer;
  TextLayer *event_active_layer;
  TextLayer *startend_layer;
  TextLayer *description_layer;
  Layer *icon_layer;
} EventsAppData;
