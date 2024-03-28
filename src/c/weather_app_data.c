/*
 * Copyright (c) 2015 Pebble Technology
 */

#include <pebble.h>
#include <math.h>
#include "weather_app_data.h"
#include "weather_app_resources.h"

void weather_app_main_window_view_model_announce_changed(EventsCardsMainWindowViewModel *model) {
  if (model->announce_changed) {
    model->announce_changed((struct EventsCardsMainWindowViewModel *)model);
  }
}

void weather_app_view_model_set_highlow(EventsCardsMainWindowViewModel *model, uint8_t end_hours, uint8_t end_minutes, uint8_t start_hours, uint8_t start_minutes) {
  model->startend.end_hours = end_hours;
  model->startend.end_minutes = end_minutes;
  model->startend.start_hours = start_hours;
  model->startend.start_minutes = start_minutes;
  snprintf(model->startend.text, sizeof(model->startend.text), "%d:%.2d - %d:%.2d", model->startend.start_hours, model->startend.start_minutes, model->startend.end_hours, model->startend.end_minutes);
}

void weather_app_view_model_set_temperature(EventsCardsMainWindowViewModel *model, int32_t value) {
  model->active.value = value;
  snprintf(model->active.text, sizeof(model->active.text), "%d°", model->active.value);
}

void weather_app_view_model_set_icon(EventsCardsMainWindowViewModel *model, GDrawCommandImage *image) {
  free(model->icon.draw_command);
  model->icon.draw_command = image;
  weather_app_main_window_view_model_announce_changed(model);
}

EventsDataViewNumbers weather_app_data_point_view_model_numbers(EventDataPoint *data_point) {
  double end_hours;
  uint8_t end_minutes = modf((double)data_point->end_time / 60 / 60, &end_hours) * 60;
  double start_hours;
  uint8_t start_minutes = modf((double)data_point->start_time / 60 / 60, &start_hours) * 60;
  return (EventsDataViewNumbers){
      .active = data_point->current,
      .end_hours = end_hours,
      .end_minutes = end_minutes,
      .start_hours = start_hours,
      .start_minutes = start_minutes,
  };
}

int weather_app_index_of_data_point(EventDataPoint *dp);

void weather_app_view_model_fill_strings_and_pagination(EventsCardsMainWindowViewModel *view_model, EventDataPoint *data_point) {
  view_model->name = data_point->name;
  view_model->description = data_point->description;

  view_model->pagination.idx = (int16_t)(1 + weather_app_index_of_data_point(data_point));
  view_model->pagination.num = (int16_t)weather_app_num_data_points();
  snprintf(view_model->pagination.text, sizeof(view_model->pagination.text), "%d/%d", view_model->pagination.idx, view_model->pagination.num);
  weather_app_main_window_view_model_announce_changed(view_model);
}


GDrawCommandImage *weather_app_data_point_create_icon(EventDataPoint *data_point) {
  return weather_app_resources_get_icon(data_point->icon);
}


void weather_view_model_fill_numbers(EventsCardsMainWindowViewModel *model, EventsDataViewNumbers numbers) {
  weather_app_view_model_set_temperature(model, numbers.active);
  weather_app_view_model_set_highlow(model, numbers.end_hours, numbers.end_minutes, numbers.start_hours, numbers.start_minutes);
}

void weather_app_view_model_fill_colors(EventsCardsMainWindowViewModel *model, GColor color) {
  model->bg_color.top = color;
  model->bg_color.bottom = color;
  weather_app_main_window_view_model_announce_changed(model);
}

GColor weather_app_data_point_color(EventDataPoint *data_point) {
  return data_point->current > 90 ? GColorOrange : GColorPictonBlue;
}

void weather_app_view_model_fill_all(EventsCardsMainWindowViewModel *model, EventDataPoint *data_point) {
  EventsCardsMainWindowViewModelFunc annouce_changed = model->announce_changed;
  memset(model, 0, sizeof(*model));
  model->announce_changed = annouce_changed;
  weather_app_view_model_fill_strings_and_pagination(model, data_point);
  weather_app_view_model_set_icon(model, weather_app_data_point_create_icon(data_point));
  weather_app_view_model_fill_colors(model, weather_app_data_point_color(data_point));
  weather_view_model_fill_numbers(model, weather_app_data_point_view_model_numbers(data_point));

  weather_app_main_window_view_model_announce_changed(model);
}

void weather_app_view_model_deinit(EventsCardsMainWindowViewModel *model) {
  weather_app_view_model_set_icon(model, NULL);
}

static EventDataPoint s_data_points[] = {
    {
        .name = "PALO ALTO",
        .description = "Light Rain.",
        .icon = WEATHER_APP_ICON_LIGHT_RAIN,
        .current = 68,
        .end_time = 70,
        .start_time = 60,
    },
    {
        .name = "LOS ANGELES",
        .description = "Clear throughout the day.",
        .icon = WEATHER_APP_ICON_SUNNY_DAY,
        .current = 100,
        .end_time = 100,
        .start_time = 80,
    },
    {
        .name = "SAN FRANCISCO",
        .description = "Rain and Fog.",
        .icon = WEATHER_APP_ICON_HEAVY_SNOW,
        .current = 60,
        .end_time = 62,
        .start_time = 56,
    },
    {
        .name = "SAN DIEGO",
        .description = "Surfboard :)",
        .icon = WEATHER_APP_ICON_GENERIC_WEATHER,
        .current = 110,
        .end_time = 120,
        .start_time = 9,
    },
};

int weather_app_num_data_points(void) {
  return ARRAY_LENGTH(s_data_points);
}

EventDataPoint *weather_app_data_point_at(int idx) {
  if (idx < 0 || idx > weather_app_num_data_points() - 1) {
    return NULL;
  }

  return &s_data_points[idx];
}

int weather_app_index_of_data_point(EventDataPoint *dp) {
  for (int i = 0; i < weather_app_num_data_points(); i++) {
    if (dp == weather_app_data_point_at(i)) {
      return i;
    }
  }
  return -1;
}

EventDataPoint *weather_app_data_point_delta(EventDataPoint *dp, int delta) {
  int idx = weather_app_index_of_data_point(dp);
  if (idx < 0) {
    return NULL;
  }
  return weather_app_data_point_at(idx + delta);
}
