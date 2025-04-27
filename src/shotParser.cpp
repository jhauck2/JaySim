#include "shotParser.hpp"
#include "nlohmann/json.hpp"
#include <stdio.h>

using json = nlohmann::json;

t_shot_data parse_json_shot_data(json json_data) {
    t_shot_data data;

    data.deviceID = json_data["DeviceID"];
    if (json_data["Units"] != NULL) {
        std::string units_s = json_data["Units"];
        if (units_s == "Yards") data.units = YARDS;
        else data.units = METERS;
    }
    else data.units = YARDS;
    data.shotNumber = json_data["ShotNumber"];
    data.APIversion = json_data["APIversion"];

    data.ball_data.speed = json_data["BallData"]["Speed"];
    data.ball_data.spinAxis = json_data["BallData"]["SpinAxis"];
    data.ball_data.totalSpin = json_data["BallData"]["TotalSpin"];
    data.ball_data.backSpin = json_data["BallData"]["BackSpin"];
    data.ball_data.sideSpin = json_data["BallData"]["SideSpin"];
    data.ball_data.HLA = json_data["BallData"]["HLA"];
    data.ball_data.VLA = json_data["BallData"]["VLA"];


    return data;
}

t_shot_data parse_json_shot_string(const std::string& shot_string) {
    json json_data = json::parse(shot_string);


    return parse_json_shot_data(json_data);
}

t_shot_data parse_json_shot_file(const std::string& path) {
    std::ifstream file(path);
    json json_data = json::parse(file);

    return parse_json_shot_data(json_data);

}

