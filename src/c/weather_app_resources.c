/*
 * Copyright (c) 2015 Pebble Technology
 */

#include <pebble.h>
#include "weather_app_resources.h"

GDrawCommandImage *weather_app_resources_get_icon(WeatherAppIcon icon) {
  switch (icon) {
    case EVENT_TYPE_MINING:
      return gdraw_command_image_create_with_resource(RESOURCE_ID_ICON_MINING_SMALL);

    case EVENT_TYPE_BOTANY:
      return gdraw_command_image_create_with_resource(RESOURCE_ID_ICON_BOTANY);

    case EVENT_TYPE_FISHING:
      return gdraw_command_image_create_with_resource(RESOURCE_ID_ICON_FISHING);

    default:
    case EVENT_TYPE_SIGHTSEEING:
      return gdraw_command_image_create_with_resource(RESOURCE_ID_ICON_SIGHTSEEING_SMALL);
  }
}
