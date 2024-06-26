/*
 * Copyright (c) 2015 Pebble Technology
 */

#include <pebble.h>
#include "weather_app_animations.h"
#include "weather_app_private.h"

typedef void (*WeatherDataAnimatedNumbersSetter)(EventsAppData *data, EventsDataViewNumbers numbers);

static EventsDataViewNumbers get_animated_numbers(EventsCardsMainWindowViewModel *model) {
  return (EventsDataViewNumbers) {
    .active = model->active.value,
    .start_hours = model->startend.start_hours,
    .start_minutes = model->startend.start_minutes,
    .end_hours = model->startend.end_hours,
    .end_minutes = model->startend.end_minutes,
  };
}

static void set_animated_numbers(EventsCardsMainWindowViewModel *model, EventsDataViewNumbers numbers) {
  weather_view_model_fill_numbers(model, numbers);
  weather_app_main_window_view_model_announce_changed(model);
}

static inline int32_t distance_interpolate(const int32_t normalized, uint8_t from, uint8_t to) {
  return from + ((normalized * (to - from)) / ANIMATION_NORMALIZED_MAX);
}

void property_animation_update_animated_numbers(PropertyAnimation *property_animation, const uint32_t distance_normalized) {
  EventsDataViewNumbers from, to;
  property_animation_from(property_animation, &from, sizeof(from), false);
  property_animation_to(property_animation, &to, sizeof(to), false);

  EventsDataViewNumbers current = (EventsDataViewNumbers) {
    .active = distance_interpolate(distance_normalized, from.active, to.active),
    .end_hours = distance_interpolate(distance_normalized, from.end_hours, to.end_hours),
    .end_minutes = distance_interpolate(distance_normalized, from.end_minutes, to.end_minutes),
    .start_hours = distance_interpolate(distance_normalized, from.start_hours, to.start_hours),
    .start_minutes = distance_interpolate(distance_normalized, from.start_minutes, to.start_minutes),
  };
  PropertyAnimationImplementation *impl = (PropertyAnimationImplementation *) animation_get_implementation((Animation *) property_animation);
  WeatherDataAnimatedNumbersSetter setter = (WeatherDataAnimatedNumbersSetter)impl->accessors.setter.grect;

  void *subject;
  if (property_animation_get_subject(property_animation, &subject) && subject) {
    setter(subject, current);
  }
}

static const PropertyAnimationImplementation s_animated_numbers_implementation = {
  .base = {
    .update = (AnimationUpdateImplementation) property_animation_update_animated_numbers,
  },
  .accessors = {
    .setter = { .grect = (const GRectSetter) set_animated_numbers, },
    .getter = { .grect = (const GRectGetter) get_animated_numbers, },
  },
};


Animation *weather_app_create_view_model_animation_numbers(EventsCardsMainWindowViewModel *view_model, EventDataPoint *next_data_point) {
  PropertyAnimation *number_animation = property_animation_create(&s_animated_numbers_implementation, view_model, NULL, NULL);
  EventsDataViewNumbers numbers = get_animated_numbers(view_model);
  property_animation_from(number_animation, &numbers, sizeof(numbers), true);
  numbers = weather_app_data_point_view_model_numbers(next_data_point);
  property_animation_to(number_animation, &numbers, sizeof(numbers), true);
  return (Animation *) number_animation;
}

// --------------------------

EventsCardsMainWindowViewModel *view_model_from_animation(Animation *animation) {
  void *subject = NULL;
  property_animation_get_subject((PropertyAnimation *) animation, &subject);
  return subject;
}

static void update_bg_color_normalized(Animation *animation, const uint32_t distance_normalized) {
  EventsCardsMainWindowViewModel *view_model = view_model_from_animation(animation);

  view_model->bg_color.to_bottom_normalized = distance_normalized;
  weather_app_main_window_view_model_announce_changed(view_model);
}

static const PropertyAnimationImplementation s_bg_color_normalized_implementation = {
  .base = {
    .update = (AnimationUpdateImplementation) update_bg_color_normalized,
  },
};

static void bg_colors_animation_started(Animation *animation, void *context) {
  EventsCardsMainWindowViewModel *view_model = view_model_from_animation(animation);

  EventDataPoint *dp = context;
  GColor color = weather_app_data_point_color(dp);

  // before, .top and .bottom are set to the current color, see weather_app_view_model_fill_colors()
  if (animation_get_reverse(animation)) {
    view_model->bg_color.top = color;
  } else {
    view_model->bg_color.bottom = color;
  }

  weather_app_main_window_view_model_announce_changed(view_model);
}

static void bg_colors_animation_stopped(Animation *animation, bool finished, void *context) {
  EventsCardsMainWindowViewModel *view_model = view_model_from_animation(animation);

  EventDataPoint *dp = context;
  GColor color = weather_app_data_point_color(dp);

  weather_app_view_model_fill_colors(view_model, color);
}

Animation *weather_app_create_view_model_animation_bgcolor(EventsCardsMainWindowViewModel *view_model, EventDataPoint *next_data_point) {
  Animation *bg_animation = (Animation *) property_animation_create(&s_bg_color_normalized_implementation, view_model, NULL, NULL);
  animation_set_handlers(bg_animation, (AnimationHandlers){
    .started = bg_colors_animation_started,
    .stopped = bg_colors_animation_stopped,
  }, next_data_point);
  return bg_animation;
}

// -------------------------

static void update_icon_square_normalized(Animation *animation, const uint32_t distance_normalized) {
  EventsCardsMainWindowViewModel *view_model = view_model_from_animation(animation);

  view_model->icon.to_square_normalized = distance_normalized;
  weather_app_main_window_view_model_announce_changed(view_model);
}

static const PropertyAnimationImplementation s_icon_scquare_normalized_implementation = {
  .base = {
    .update = (AnimationUpdateImplementation) update_icon_square_normalized,
  },
};

static void replace_icon_stop_handler(Animation *animation, bool finished, void *context) {
  EventsCardsMainWindowViewModel *view_model = view_model_from_animation(animation);
  GDrawCommandImage *icon = context;
  weather_app_view_model_set_icon(view_model, icon);
}

Animation *weather_app_create_view_model_animation_icon(EventsCardsMainWindowViewModel *view_model, EventDataPoint *next_data_point, uint32_t duration) {
  Animation *icon_animation_to_square = (Animation *) property_animation_create(&s_icon_scquare_normalized_implementation, view_model, NULL, NULL);
  animation_set_duration(icon_animation_to_square, duration / 2);
  animation_set_curve(icon_animation_to_square, AnimationCurveEaseIn);

  Animation *icon_animation_from_square = animation_clone(icon_animation_to_square);
  animation_set_reverse(icon_animation_from_square, true);

  GDrawCommandImage *icon = weather_app_data_point_create_icon(next_data_point);
  animation_set_handlers(icon_animation_to_square, (AnimationHandlers) {
    .stopped = replace_icon_stop_handler,
  }, icon);

  return animation_sequence_create(icon_animation_to_square, icon_animation_from_square, NULL);
}
