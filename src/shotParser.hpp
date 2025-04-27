#ifndef __SHOT_PARSER__
#define __SHOT_PARSER__

#include <fstream>
#include <string>

enum t_units {
    YARDS = 0,
    METERS
};

typedef struct ball_data {
    float speed;
    float spinAxis;
    float totalSpin;
    float backSpin;
    float sideSpin;
    float HLA;
    float VLA;
    float carryDistance; // optional
} t_ball_data;

typedef struct club_data {
    float speed;
    float aoa;
    float faceToTarget;
    float lie;
    float loft;
    float path;
    float speedAtImpact;
    float verticalFaceImpact;
    float horizontalFaceImpact;
    float closureRate;
} t_club_data;

typedef struct shot_options {
    bool containsBallData;
    bool containsClubData;
    bool LMReady;
    bool LMBallDetected;
    bool isHeartBeat;

} t_shot_options;

typedef struct shot_data {
    std::string deviceID;
    t_units units;
    int shotNumber;
    std::string APIversion;
    t_ball_data ball_data;
    t_club_data club_data;
    t_shot_options shot_options;
} t_shot_data;

t_shot_data parse_json_shot_string(const std::string& shot_string);
t_shot_data parse_json_shot_file(const std::string& path);


#endif