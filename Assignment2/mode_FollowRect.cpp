#include "includes.h"


void FollowRect::Init() {
  Display::Msg("Square track", "following.");
}

void FollowRect::Update() {

  /*
    Track has around 70x42cm size
    Wheels - sensor distance is around 20cm

    Travelling through 70cm side takes around 3270ms
    Is_near_end should be true when 70cm - sensors_offset is reached.
    So 70cm - 20cm
    So 3720ms / 70.0 * 50.0
    So 2657ms // possibly -100 or -200 for safety (to prevent turning when perpendicular line is approached)
   */


  // init static vars at runtime
  if (!start_time) start_time = millis();
  if (!travel_time) {
    float voltage = ReadPowerSupply();
    voltage_adjustment_mult = 0.1 * (voltage - 6.6); // 0.042 because when voltage was at 8.95 then 10% speed decrease was required

    travel_time = LONG_SIDE_TRAVEL_TIME - (unsigned long)((float)LONG_SIDE_TRAVEL_TIME * voltage_adjustment_mult);
    is_near_end_threshold_time = (unsigned long)((float)travel_time / side_len * (side_len - SENSORS_OFFSET));

    BT_SERIAL.print("time mult = "); BT_SERIAL.print(voltage_adjustment_mult); BT_SERIAL.print(", voltage = "); BT_SERIAL.print(voltage); BT_SERIAL.println("V");
    BT_SERIAL.println("travel_time = " + String(travel_time) + ", side_len = " + String((int)side_len) + ", is_near_end_threshold_time = " + String(is_near_end_threshold_time));
  }  

  bool l_,c,r;
  tracker_sensor->GetAll(l_,c,r);

  // not needed, delete
  //bool orig_l = !digitalRead(IO_0);
  //bool orig_r = !digitalRead(IO_1);

  int time_since_last_turn = millis() - start_time;
  static bool is_near_end = false;
  static bool reached_end = false; 

  /*  Make sound at which point the front sensor readings should be ignored.  */
  if (!is_near_end && time_since_last_turn > is_near_end_threshold_time) {
    /*  262 is NOTE_C4 in the pitches.h file from toneMelody example provided by Emoro library  */
    tone(BUZ_BUILTIN, 262, 300); 
    is_near_end = true;
  }

  /*  Make another sound using higher pitch when its time to turn.  */
  if (!reached_end && time_since_last_turn > travel_time) {
    /*  523 is NOTE_C5 in the pitches.h file from toneMelody example provided by Emoro library  */
    tone(BUZ_BUILTIN, 523, 300); 
    reached_end = true;
  }   

  /*  Drive forward if the sensor shows correct position over the line.
      If the car is close to the turning spot, then blindly go forward until that spot is reached.  */
  if (!l_ && c && !r || is_near_end)
    drive.Forward();

  /*  Make slight corrections (left/right) but only if the car is not close to the turning spot.  */
  if (!is_near_end) {
    if (l_ && !r)
      drive.Left();
      
    if (!l_ && r)
      drive.Right();
  }

  /*  Recognize which what to turn but only before the first turn.
      Rarely (1 in around 20 turns) this is misrecognized so that's why it's done only once. */
  if (is_near_end && is_first_turn) {
    if (l_ && c && !r)
      turn_direction = ANTI_CLOCKWISE;

    if (!l_ && c && r)
      turn_direction = CLOCKWISE;
  }

  /*  If the car reached the turning spot then turn and update the information about next turning spot.
      It also sends diagnostic information through bluetooth, allowing the method to be adjusted.
      This debugging method allowed to adjust time calculation due to battery voltage level.
      (which results in higher speed of the car, that is a factor that is currently taken into consideration by the code).  */
  if (reached_end) {
    drive.Stop();
    Display::Msg("Sharp turning", "90 degrees.");
    bool was_line_encountered = drive.Turn(135 * (turn_direction == CLOCKWISE ? 1 : -1), true); // true = stop_at_line
    Display::Msg("Square track", "following.");
    start_time = 0;
    
    if (is_side_long) {
      BT_SERIAL.println("orig travel time = " + String(SHORT_SIDE_TRAVEL_TIME));
      travel_time = SHORT_SIDE_TRAVEL_TIME - (unsigned long)((float)SHORT_SIDE_TRAVEL_TIME * voltage_adjustment_mult);
      side_len = SHORT_SIDE_LINE_LEN;
    }
    else {
      BT_SERIAL.println("orig travel time = " + String(LONG_SIDE_TRAVEL_TIME));
      travel_time = LONG_SIDE_TRAVEL_TIME - (unsigned long)((float)LONG_SIDE_TRAVEL_TIME * voltage_adjustment_mult);
      side_len = LONG_SIDE_LINE_LEN;
    }

    /*  Update at which point in time the car should ignore the front sensors.  */
    is_near_end_threshold_time = (int)((float)travel_time / side_len * (side_len - SENSORS_OFFSET)) - 100;
    BT_SERIAL.println("travel_time = " + String(travel_time) + ", side_len = " + String((int)side_len) + ", is_near_end_threshold_time = " + String(is_near_end_threshold_time));
    
    is_side_long = !is_side_long;
    is_first_turn = is_near_end = reached_end = false;
  }
}

void FollowRect:: Reset() {
  is_side_long = true;
  start_time = NULL;
  travel_time = NULL; 
  LONG_SIDE_LINE_LEN;
  is_near_end_threshold_time = NULL;
  voltage_adjustment_mult = NULL;
  turn_direction = NULL;
  is_first_turn = true;
}
