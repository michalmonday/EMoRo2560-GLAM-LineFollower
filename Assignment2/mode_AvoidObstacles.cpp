#include "includes.h"

#define OBSTACLE_DISTANCE_THRESHOLD 50

void AvoidObstacles::Init() {
  Display::Msg("moving forward");
}

void AvoidObstacles::Update() {
  int dist = Ultrasonic.read(GPP_0);
  /*
  int val = Ultrasonic.read(GPP_0);
  –Result of function:
  (0-399)–Sensor distancein cm
  (400)–Sensor out of range
  (-1) –Error: Argument „port“ out of range: [GPP_0–GPP_7]
  (-2)–Error: Ultrasonic sensor is notinitialized  
  */
  
  Serial.println("AvoidObstacles, dist = " + String(dist));
  
  static unsigned long last_display_update = 0;
  if (millis() - last_display_update > 100) {
    bool is_obstacle = dist < OBSTACLE_DISTANCE_THRESHOLD;
    Display::Msg(is_obstacle ? "obstacle detected" : "moving forward", "GPP_0 = " + String(dist));

    Serial.println("AvoidObstacles, is_obstacle = " + String(is_obstacle));
    last_display_update = millis();
    
    if (is_obstacle) {
      drive.Stop();

      /*  Make 3 quick low sound effects to notify the usef about detected obstacle.  */
      PlayDetectionSound();

      /*  Move 360 degrees around in order to find the best angle.  */
      double best_angle = FindBestAngle();
      String cur_angle = String((int)Gyr.readDegreesZ());
      
      Display::Msg("best = ", String((int)best_angle), "current = ", cur_angle);
      BT_SERIAL.println("Best angle = " + String((int)best_angle) + ", current angle = " + cur_angle);
      drive.Stop();

      delay(presentation ? 3000 : 100);
      
      int diff = GetAnglesDiff(best_angle, Gyr.readDegreesZ());
      Display::Msg("Turning " + String(diff), "degrees");
      BT_SERIAL.println("Turning " + String(diff) + " degrees");
            
      delay(presentation ? 3000 : 100);

      drive.Turn(diff);
      drive.Stop();
    } 
  }

  drive.Forward();
}

void AvoidObstacles::Reset() {

}


/*  Turn around 360 degrees while repetitively checking ultrasonic distance sensor, trying to find optimal direction to follow.
    (optimal direction in terms of avoiding obstacles) 
    
    Gyroscope sensor is used to check the current angle.  */
double AvoidObstacles::FindBestAngle() {
  /*  diagnostic_reading_counter increases each time the Ultrasonic sensor reading is done during the turn.
      Low number would indicate possibly inaccurate estimation of the best angle.  */
  int diagnostic_reading_counter = 0;
  
  double start_angle = Gyr.readDegreesZ();
  double angle_where_max_dist;
  int max_dist = 0;

  BT_SERIAL.println("start_angle = " + String((int)start_angle));
  
  bool angle_exceeded_360 = false;
  double new_angle, last_angle;
  int dist;
  drive.Right(true);
  delay(200);
  while ((new_angle = Gyr.readDegreesZ()) < start_angle || !angle_exceeded_360) {  
    /*  + 10 to avoid false detection of full circle while standing still due to fluctuating sensor reading  */
    if ((new_angle + 10.0) < last_angle) { 
      if (last_angle < start_angle && angle_exceeded_360) {
        break;
      }
      angle_exceeded_360 = true;
    }

    /* Save new largest distance position (and the distance itself) */
    if ((dist = Ultrasonic.read(GPP_0)) > max_dist) {
      max_dist = dist;
      angle_where_max_dist = new_angle;

      /* Make sound (with higher pitch than the one when obstacle was detected). 
         Using sound instead of display/serial message gives better insight in this case. 
         (because we can't look at 2 things at once and time plays big role here)  */
      tone(BUZ_BUILTIN, 131, 50); // NOTE_C3  131
    }
    last_angle = new_angle;

    static int last_dist = 0;
    if (dist != last_dist || dist == 400)
      diagnostic_reading_counter++;
    last_dist = dist;
  }
  BT_SERIAL.println("angle_where_max_dist = " + String((int)angle_where_max_dist) + ", max_dist = " + String(max_dist) + ", number of distinct readings done during 360 turn = " + String(diagnostic_reading_counter));
  return angle_where_max_dist;
}

void AvoidObstacles::PlayDetectionSound() {
  tone(BUZ_BUILTIN, 33, 50); // NOTE_C1  33
  delay(100);
  tone(BUZ_BUILTIN, 33, 50); // NOTE_C1  33
  delay(100);
  tone(BUZ_BUILTIN, 33, 50); // NOTE_C1  33
  delay(500);
}
