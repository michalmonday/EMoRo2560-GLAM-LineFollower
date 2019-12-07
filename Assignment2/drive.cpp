#include "includes.h"

#define LEFT_WHEEL SERVO_1
#define RIGHT_WHEEL SERVO_0

#define BACKWARD  STOP - speed_
#define STOP      1500
#define FORWARD   STOP + speed_
#define FIX_DIRECTION(x, y) x == LEFT_WHEEL ? y - (y - STOP)*2 : y 

#define MAX_SPEED 1000

Drive drive;

Drive::Drive():
  speed_(1000)
{
  
}

void Drive::SetSpeed(int s) {
  speed_ = s;
  if (speed_ > MAX_SPEED) speed_ = MAX_SPEED;
  Serial.println("Speed set to = " + String(speed_));
} 

void Drive::WheelsServosWrite(int left_val, int right_val) {
  Serial.println("WheelsServosWrite left_val = " + String(left_val) + ", right_val = " + String(right_val));
  
  EmoroServo.write(LEFT_WHEEL, FIX_DIRECTION(LEFT_WHEEL, left_val));    
  EmoroServo.write(RIGHT_WHEEL, FIX_DIRECTION(RIGHT_WHEEL, right_val)); 
}

void Drive::Stop() { WheelsServosWrite(STOP, STOP); }
void Drive::Forward() { WheelsServosWrite(FORWARD, FORWARD); }
void Drive::Backward() { WheelsServosWrite(BACKWARD, BACKWARD); }

/* The divider determines how much 1 of the wheels slows down during the turn.
   If sharp arg is true then 1 of the wheels turns backwards. 
   Back parameters allows for convenient control of the car using bluetooth.  */
void Drive::Left(bool sharp, bool back, float divider) {
  if (sharp) {
    WheelsServosWrite(BACKWARD, FORWARD); 
  }
  else {
    if (!back) WheelsServosWrite(FORWARD - speed_/divider, FORWARD); // file:///C:/Users/mb19424/Downloads/819-Article%20Text-1846-1-10-20170130.pdf
    else WheelsServosWrite(BACKWARD + speed_/divider, BACKWARD);
  }
}

/* Equivalent to Left function */
void Drive::Right(bool sharp, bool back, float divider) {
  if (sharp) {
    WheelsServosWrite(FORWARD, BACKWARD);
  }
  else {
    if (!back) WheelsServosWrite(FORWARD, FORWARD - speed_/divider); // file:///C:/Users/mb19424/Downloads/819-Article%20Text-1846-1-10-20170130.pdf
    else WheelsServosWrite(BACKWARD, BACKWARD + speed_/divider);
  }
}

/*  Rebound is required when the Turn function recognizes line underneath the sensor.
    That is because the following stop of the turn happens too late.  */
void Drive::Rebound(bool right, int size_) {
  unsigned long start_time = millis();
  BT_SERIAL.println("Drive::Rebound start");

  Stop();
  delay(80);
  if (right) Turn(-size_, true);
  else       Turn(size_, true);
 
  Stop();
  BT_SERIAL.println("Drive::Rebound took " + String(millis() - start_time) + "ms. right = " + String(right) + ", size_ = " + String(size_));
}


/* Designed mainly for 90deg turns using top speed, other angles may not be very accurate  
   45 deg turn takes around 300ms
   90 deg turn takes around 470ms (but delay of 470 is not enough, 12 turns resulted in 90 degrees discrepancy, (470 + 470/12) was tested and working properly, 510ms is used because of that)
   180 deg turn takes around 940ms    

   Returns true if the line was found during the turn (assuming that stop_at_line argument was true, otherwise return is always false). */
bool Drive::Turn(int deg, bool stop_at_line) {
  static int rebound_size = 17;
  bool is_direction_right = deg > 0;
  unsigned long delay_time = (int)((float)abs(deg) * 5.67); // 510ms / 90 degrees
  
  if (is_direction_right) 
    Right(true);
  else                    
    Left(true);

  if (stop_at_line) {
    /*  If the turn supposed to end when black line is recognized under the sensor then
        checks must be made during the turn, therefore "delay" function can't be used. 
        For that reason the while loop below checks the time passed and reads the state of 
        the front infra red sensors.  */
    unsigned long start_time = millis();
    while (millis() - start_time < delay_time) {
      if (tracker_sensor->IsOnTheLine()){
        BT_SERIAL.println("Drive::Turn - LINE FOUND");
        Rebound(is_direction_right, rebound_size);
        return true;
      }
    }
  }
  else {
    /*  If stopping at line is not done then no additional actions have to be done while waiting. So simple delay is used.  */
    delay(delay_time);
  }
  Stop();
  return false;
}

/*  Inaccurate, probably need to delete it  */
void Drive::Turn_GyroBased(double angle, bool stop_at_line) {
  int cur_angle = (int)Gyr.readDegreesZ();
  int end_angle = (cur_angle + (int)angle) % 360;
  BT_SERIAL.println("Current angle = " + String(cur_angle) + ", angle provided = " + String((int)angle) + ", end angle = " + String(end_angle));
  unsigned long start_time = millis();
  TurnAbsolute_GyroBased((double)end_angle, stop_at_line);
  BT_SERIAL.println(String((int)angle) + "deg. angle turn took " + String(millis() - start_time) + "ms." );
}

/*  Inaccurate, probably need to delete it  */
void Drive::TurnAbsolute_GyroBased(double angle, bool stop_at_line) {
  Serial.println("TurnAtAngle");
  if (GetAnglesDiff(angle, Gyr.readDegreesZ()) > 0) {
    Right(true);
    while (GetAnglesDiff(angle, Gyr.readDegreesZ()) > 0) { 
      int cur_ang = (int)Gyr.readDegreesZ();
      BT_SERIAL.println("angle = " + String((int)angle) + ", Gyr.readDegreesZ = " + String(cur_ang) + ", GetAnglesDiff = " + String((int)GetAnglesDiff(angle, Gyr.readDegreesZ())));
      BT_SERIAL.println(tracker_sensor->StrValues());
      if (stop_at_line && tracker_sensor->IsOnTheLine()) {
        Stop();
        Display::Msg("LINE FOUND");
        BT_SERIAL.println("LINE FOUND");
        delay(1000);
        break;
      }
    }
  }
  else {
    Left(true);
    while (GetAnglesDiff(angle, Gyr.readDegreesZ()) < 0) { 
      if (stop_at_line && tracker_sensor->IsOnTheLine()) {
        Stop();
        Display::Msg("LINE FOUND");
        delay(1000);
        break;
      }
    }
  }
  Stop();
  Serial.println("TurnAtAngle (return)");
}
