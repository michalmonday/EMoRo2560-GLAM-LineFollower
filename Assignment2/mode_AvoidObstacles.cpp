#include "includes.h"

#define OBSTACLE_DISTANCE_THRESHOLD 50

void AvoidObstacles::Init() {
  Display::Msg("moving forward");
}

void AvoidObstacles::Update() {
  int dist = Ultrasonic.read(GPP_0);
  Serial.println("ObstacleAvoider, dist = " + String(dist));
  
  static unsigned long last_display_update = 0;
  if (millis() - last_display_update > 100) {
    bool is_obstacle = dist < OBSTACLE_DISTANCE_THRESHOLD;
    Display::Msg(is_obstacle ? "obstacle detected" : "moving forward", "GPP_0 = " + String(dist));

    Serial.println("ObstacleAvoider, is_obstacle = " + String(is_obstacle));
    last_display_update = millis();
    
    if (is_obstacle) {
      drive.Stop();
      double best_angle = FindBestAngle();
      Display::Msg("BestAngle found", "", String((int)best_angle), "");
      drive.Stop();
      delay(2000);
      
      drive.TurnAbsolute_GyroBased(best_angle);
      drive.Stop();
      //while(true){}
    } 
  }

  drive.Forward();
  /*
  int val = Ultrasonic.read(GPP_0);
  –Result of function:
  (0-399)–Sensor distancein cm
  (400)–Sensor out of range
  (-1) –Error: Argument „port“ out of range: [GPP_0–GPP_7]
  (-2)–Error: Ultrasonic sensor is notinitialized  
  */
}

void AvoidObstacles::Reset() {

}


/*  Turn around 360 degrees while repetitively checking ultrasonic distance sensor, trying to find optimal direction to follow.
    (optimal direction in terms of avoiding obstacles)  */
double AvoidObstacles::FindBestAngle() {
  double start_angle = Gyr.readDegreesZ();
  double angle_where_max_dist = (double)(((int)start_angle + 180) % 360);
  int max_dist = 0;

  Serial.println("FindBestAngle, angle = " + String((int)start_angle) + ", angle_where_max_dist = " + String((int)angle_where_max_dist));
  
  bool angle_exceeded_360 = false;
  double new_angle, last_angle;
  int dist;
  drive.Right(true);
  delay(200);
  while ((new_angle = Gyr.readDegreesZ()) < start_angle || !angle_exceeded_360) {  // + 10 to avoid false detection of full circle while standing still due to fluctuating sensor reading
    //Serial.println("FindBestAngle, new_angle = " + String((int)new_angle) + ", angle_exceeded_360 = " + String(angle_exceeded_360));
    
    if ((new_angle + 10.0) < last_angle) { // + 10 to avoid false detection of full circle while standing still due to fluctuating sensor reading
      if (last_angle < start_angle && angle_exceeded_360) {
        Serial.println("FindBestAngle (break), last_angle = " + String((int)last_angle) + ", start_angle = " + String((int)start_angle) + ", new_angle = " + String((int)new_angle));
        break;
      }

      angle_exceeded_360 = true;
    }

    if ((dist = Ultrasonic.read(GPP_0)) > max_dist) {
      max_dist = dist;
      angle_where_max_dist = new_angle;
      Serial.println("FindBestAngle (new max dist found), angle_where_max_dist = " + String((int)angle_where_max_dist) + ", max_dist = " + String(max_dist));
      BT_SERIAL.println("FindBestAngle (new max dist found), angle_where_max_dist = " + String((int)angle_where_max_dist) + ", max_dist = " + String(max_dist));
    }

    Serial.println("dist = " + String(dist));
    BT_SERIAL.println("dist = " + String(dist) + ",\t\tangle = " + String((int)new_angle));

    last_angle = new_angle;
  }

  Serial.println("FindBestAngle (return), angle_where_max_dist = " + String((int)angle_where_max_dist) + ", max_dist = " + String(max_dist));
  BT_SERIAL.println("FindBestAngle (return), angle_where_max_dist = " + String((int)angle_where_max_dist) + ", max_dist = " + String(max_dist));
  return angle_where_max_dist;
}
