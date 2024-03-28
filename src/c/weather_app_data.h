/*
 * Copyright (c) 2015 Pebble Technology
 */

#pragma once

#include <pebble.h>

struct EventsCardsMainWindowViewModel;

typedef void (*EventsCardsMainWindowViewModelFunc)(struct EventsCardsMainWindowViewModel* model);

typedef struct {
  EventsCardsMainWindowViewModelFunc announce_changed;
  struct {
    GColor top;
    GColor bottom;
    int32_t to_bottom_normalized;
  } bg_color;
  char *name;
  struct {
    uint8_t value;
    char text[8];
  } active;
  struct {
    GDrawCommandImage *draw_command;
    int32_t to_square_normalized;
  } icon;
  struct {
    int16_t idx;
    int16_t num;
    char text[8];
  } pagination;
  struct {
    uint8_t start_hours;
    uint8_t start_minutes;
    uint8_t end_hours;
    uint8_t end_minutes;
    char text[20];
  } startend;
  char *description;
} EventsCardsMainWindowViewModel;

//! calls model's .announce_changed or does nothing if NULL
void weather_app_main_window_view_model_announce_changed(EventsCardsMainWindowViewModel *model);

typedef struct {
  char *name;
  char *description;
  int icon;
  int32_t current;
  int32_t end_time;
  int32_t start_time;
} EventDataPoint;

typedef struct {
  uint8_t active;
  uint8_t start_hours;
  uint8_t start_minutes;
  uint8_t end_hours;
  uint8_t end_minutes;
} EventsDataViewNumbers;


void weather_app_view_model_set_highlow(EventsCardsMainWindowViewModel *model, uint8_t end_hours, uint8_t end_minutes, uint8_t start_hours, uint8_t start_minutes);

void weather_app_view_model_set_temperature(EventsCardsMainWindowViewModel *model, int32_t value);
void weather_app_view_model_set_icon(EventsCardsMainWindowViewModel *model, GDrawCommandImage *image);

EventsDataViewNumbers weather_app_data_point_view_model_numbers(EventDataPoint *data_point);

GDrawCommandImage *weather_app_data_point_create_icon(EventDataPoint *data_point);

void weather_app_view_model_fill_strings_and_pagination(EventsCardsMainWindowViewModel *view_model, EventDataPoint *data_point);

void weather_view_model_fill_numbers(EventsCardsMainWindowViewModel *model, EventsDataViewNumbers numbers);

void weather_app_view_model_fill_all(EventsCardsMainWindowViewModel *model, EventDataPoint *data_point);

void weather_app_view_model_fill_colors(EventsCardsMainWindowViewModel *model, GColor color);

void weather_app_view_model_deinit(EventsCardsMainWindowViewModel *model);

GColor weather_app_data_point_color(EventDataPoint *data_point);

int weather_app_num_data_points(void);

EventDataPoint *weather_app_data_point_at(int idx);
EventDataPoint *weather_app_data_point_delta(EventDataPoint *dp, int delta);
