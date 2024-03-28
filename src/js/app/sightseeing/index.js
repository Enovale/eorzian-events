/* Mostly copied from https://tylian.net/sslog/ */
Sights = module.exports;

var ajax = require("../../lib/ajax");
var Settings = require("../../settings");
var Util = require("../util");

var jsonLength = Settings.option("lastSightJsonLength");
var parsedData = Settings.option("parsedSights");

Sights.fetchData = function(success, failure) {
    if (parsedData == null) {
        parsedData = [];
        jsonLength = null;
    }

    ajax(
        { url: "https://tylian.net/sslog/js/data.js", type: "text" },
        function (sslogData) {
            if (jsonLength == null || jsonLength != sslogData.length) {
                console.log("Parsing new sightseeing data...");
                console.log(sslogData.length);
                console.log(jsonLength);
                Settings.option("lastSightJsonLength", sslogData.length);
                var jsonStr = sslogData.split("var logData = ")[1];
                var json = JSON.parse(jsonStr);
                for(var i = 0; i < json.length; i++) {
                    var item = json[i];
                    parsedData.push(new Util.TimedEvent(item.name, item.location, item.description, null, item.time[0], item.time[1], "sightseeing", item.weather, item.weatherRate));
                }
                Settings.option("parsedSights", parsedData);
            }

            success(parsedData);
        },
        function(data, status, req) {
            failure(data, status, req);
        }
    );
}