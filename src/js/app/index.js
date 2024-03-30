var Nodes = require('./nodes');
var Sights = require('./sightseeing');
var timeline = require('pebble-timeline-js');

var monitoredEvents = [
  "Silkworm Cocoon",
  "Caiman",
  "Darksteel Ore",
  "Barracuda Piers"
]

var today = new Date();
today.setUTCHours(0,0,0,0);

var nodeData;
var sightData;
Nodes.fetchData(
  function (data) {
    nodeData = data;
    console.log("Node Data Fetched");

    Sights.fetchData(
      function (data) {
        sightData = data;
        console.log("Sightseeing data fetched");
        sendOverData();
      },
      function (error) {
        console.log("Error!");
      }
    );
  },
  function (error) {
    console.log("Error!");
  }
);

function typeToEnum(type) {
  if (type == "mining") {
    return 0;
  } else if (type == "botany") {
    return 1;
  } else if (type == "fishing") {
    return 2;
  } else if (type == "sightseeing") {
    return 3;
  }
}

function sendOverData() {
  console.log("Sending data now.");

  var monitoredData = [];
  for (var j = 0; j < nodeData.length; j++) {
    for (var l = 0; l < monitoredEvents.length; l++) {
      if (nodeData[j].name == monitoredEvents[l]) {
        monitoredData[l] = nodeData[j];
      }
    }
  }

  for (var j = 0; j < sightData.length; j++) {
    for (var l = 0; l < monitoredEvents.length; l++) {
      if (sightData[j].name == monitoredEvents[l]) {
        monitoredData[l] = sightData[j];
      }
    }
  }

  var i = 0;
  success = function () {
    i++;
    // TODO
    //if (i < nodeData.length) {
    if (i < 4) {
      sendOverEvent(monitoredData[i], i, success);
    } else {
      Pebble.sendAppMessage({ "DoneWithData": 1 });
    }
  }
  sendOverEvent(monitoredData[i], i, success);
}

function sendOverEvent(event, i, success, failure) {
  var dict = {
    "NewEvent": 1,
    "EventName": event.name,
    "EventIndex": i,
    "EventLocation": event.location,
    "EventDescription": event.description,
    "EventTimeStart": event.timeStart * 60 * 60,
    "EventTimeEnd": event.timeEnd * 60 * 60,
    "EventType": typeToEnum(event.type)
  }

  Pebble.sendAppMessage(dict, function () {
    console.log('Message sent successfully: ' + JSON.stringify(dict));
    createPin(event, function() {});
    success(dict);
  }, function (e) {
    console.log('Message failed: ' + JSON.stringify(e));
    failure(e);
  });
}

function pad(num, size) {
  num = num.toString();
  while (num.length < size) num = "0" + num;
  return num;
}

function createPin(event, callback) {
  var startTime = Util.getNextActive(event);
  var endTime = Util.getNextActiveEnd(event);
  var eorziaStartTime = Util.getEorzeaTime(startTime);
  var eorziaEndTime = Util.getEorzeaTime(endTime);
  var pin = {
    "id": "pin-" + event.id,
    "time": startTime.toISOString(),
    "duration": (event.timeEnd - event.timeStart) * 60,
    "layout": {
      "type": "calendarPin",
      "title": event.name,
      "body": event.description,
      "locationName": event.location,
      "shortSubtitle": formatTime(eorziaStartTime) + "-" + formatTime(eorziaEndTime),
      "tinyIcon": "app://images/ICON_" + event.type.toUpperCase()
    }
  };
  console.log(JSON.stringify(pin));

  timeline.insertUserPin(pin, callback);
}

function formatTime(date) {
  var hours = date.getUTCHours();
  var realHours = hours > 12 ? hours - 12 : hours;
  var dayPart = hours >= 12 ? "PM" : "AM";
  return realHours + ":" + pad(date.getUTCMinutes(), 2) + dayPart;
}

Pebble.addEventListener('appmessage', function (dict) {

});