/* Mostly copied from https://tylian.net/sslog/ */
Util = module.exports;

var CONST_EORZEA = 20.571428571428573;

Util.getEorzeaTime = function (date) {
  return new Date(date.getTime() * CONST_EORZEA);
}

Util.getEarthTime = function (date) {
  return new Date(date.getTime() / CONST_EORZEA);
}

Util.getWeather = function (date) {
  var unixSeconds = parseInt(date.getTime() / 1000);
  var bell = unixSeconds / 175;
  var increment = (bell + 8 - (bell % 8)) % 24;
  var totalDays = ((unixSeconds / 4200) << 32) >>> 0;
  var calcBase = totalDays * 100 + increment;
  var step1 = ((calcBase << 11) ^ calcBase) >>> 0;
  var step2 = ((step1 >>> 8) ^ step1) >>> 0;
  return step2 % 100;
}

Util.findZoneWeather = function (rates, date) {
  var weather = Util.getWeather(date)
  for (var i = 0; i < rates.length; i++) {
    if (weather < rates[i].rate) {
      return rates[i].weather;
    }
  }
  return "the fuck";
}

Util.isLogTimeActive = function (item, now) {
  now = now || new Date();
  var enow = Util.getEorzeaTime(now);
  var time = enow.getUTCHours() + enow.getUTCMinutes() / 60;

  var start = item.timeStart;
  var end = item.timeEnd;

  return (start <= end && start <= time && time < end) || (start > end && (start <= time || time < end));
}

Util.isLogWeatherActive = function (item, now) {
  if (item.weather == null || item.weather.length == 0) {
    return true;
  }

  now = now || new Date();
  var weather = Util.findZoneWeather(item.weatherRates, now);

  return item.weather.indexOf(weather) > -1;
}

Util.isLogActive = function (item, now) {
  now = now || new Date();
  return Util.isLogTimeActive(item, now) && Util.isLogWeatherActive(item, now);
}

Util.getNextActive = function (item) {
  var enow = Util.getEorzeaTime(new Date());
  enow.setUTCMinutes(0);
  enow.setUTCSeconds(0);

  var now = Util.getEarthTime(enow);
  var timeLength = ((24 + item.timeEnd) - item.timeStart) % 24;

  for (var i = 0; i < 10000; i++) {
    enow.setUTCHours(enow.getUTCHours() + (item.timeStart + 24 - enow.getUTCHours()) % 24);
    now.setTime(enow / CONST_EORZEA);
    for (var d = 0; d < timeLength; d++) {
      if (Util.isLogWeatherActive(item, now)) {
        return now;
      }

      enow.setUTCHours(enow.getUTCHours() + 1);
      now.setTime(enow.getTime() / CONST_EORZEA)
    }
  }

  throw new Error('Infinite loop detected!');
}

Util.getNextActiveEnd = function (item) {
  var now = Util.isLogActive(item) ? new Date() : Util.getNextActive(item);
  var enow = Util.getEorzeaTime(now);

  enow.setUTCMinutes(0);
  enow.setUTCSeconds(0);

  now.setTime(enow.getTime() / CONST_EORZEA);
  while (enow.getUTCHours() !== item.timeEnd) {
    if (!Util.isLogActive(item, now)) {
      return now;
    }

    enow.setUTCHours(enow.getUTCHours() + 1);
    now.setTime(enow.getTime() / CONST_EORZEA)
  }
  return now;
}

/*
class Time {
  constructor(hours, minutes) {
    this.hours = hours;
    this.minutes = minutes;
  }
}
*/

Util.TimedEvent = function(name, location, description, image, timeStart, timeEnd, type, weather, weatherRates) {
  this.name = name;
  this.location = location;
  this.description = description;
  this.image = image;
  this.timeStart = timeStart;
  this.timeEnd = timeEnd;
  this.type = type;
  this.weather = weather;
  this.weatherRates = weatherRates;
}