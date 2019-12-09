#include "includes.h"


void FollowRect::Init() {
  Display::Msg("Square track", "following.");
  BT_SERIAL.print("Battery level at the begining of rect run = "); BT_SERIAL.print(ReadPowerSupply()); BT_SERIAL.println("V");
  Reset();
}

void FollowRect::Update() {
  if (is_first_turn && !start_time) {
    /*  ReadPowerSupply has to be read once at the begining, 
        reading it during operation sometimes results in lower readings, 
        it's unstable.  */
    float voltage = ReadPowerSupply();
    voltage_adjustment_mult = power_supply_significance * (voltage - 6.6);
    BT_SERIAL.print("time mult = "); BT_SERIAL.print(voltage_adjustment_mult); BT_SERIAL.print(", voltage = "); BT_SERIAL.print(voltage); BT_SERIAL.println("V");
    AssignEstimatedTravelTime();
  }  

  if (!start_time)
    start_time = millis();

  bool l_,c,r;
  tracker_sensor->GetAll(l_,c,r);

  int time_since_last_turn = millis() - start_time;

  /*  Make sound at which point the front sensor readings should be ignored.  */
  if (!is_near_end && time_since_last_turn > is_near_end_threshold_time) {
    /*  262 is NOTE_C4 in the pitches.h file from toneMelody example provided by Emoro library  
        65 is NOTE_C2 */
    tone(BUZ_BUILTIN, 65, 200); 
    is_near_end = true;
  }

  /*  Make another sound using higher pitch when its time to turn.  */
  if (!reached_end && time_since_last_turn > travel_time) {
    /*  523 is NOTE_C5 in the pitches.h file from toneMelody example provided by Emoro library  
        131 is NOTE_C3  */
    tone(BUZ_BUILTIN, 131, 200); 
    reached_end = true;
  }   

  /*  Drive forward if the sensor shows correct position over the line.
      If the car is close to the turning spot, then blindly go forward until that spot is reached.  */
  if (!l_ && c && !r || is_near_end)
    drive.Forward();

  /*  Make slight corrections (left/right) but only if the car is not close to the turning spot.  
      The sharpness of the turns can be adjusted through bluetooth. 
      Using this method it was determined that 2.3 is around optimal value for it.  */
  if (!is_near_end) {
    if (l_ && !r)
      drive.Left(false, false, following_sharpness_div); // params: is_sharp (should 1 of the wheels move backward), back (should the car reverse while turning), speed_decrease_divider (how sharp the turn is)
      
    if (!l_ && r)
      drive.Right(false, false, following_sharpness_div);
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
    bool was_line_encountered = drive.Turn(135 * (turn_direction == CLOCKWISE ? 1 : -1), true, rebound_size); // true = stop_at_line
    Display::Msg("Square track", "following.");
    
    side_len = (side_len == long_side_len ? short_side_len : long_side_len);
    BT_SERIAL.println("side_len = " + String(side_len));
    
    AssignEstimatedTravelTime();
    is_first_turn = is_near_end = reached_end = false;
    start_time = NULL;


  }
}


void FollowRect::Reset() {
  start_time = NULL;
  travel_time = NULL; 
  side_len = long_side_len;
  is_near_end_threshold_time = NULL;
  voltage_adjustment_mult = NULL;
  turn_direction = NULL;
  is_first_turn = true;

  is_near_end = false;
  reached_end = false;
  
  following_sharpness_div = 2.0; 
  following_sharpness_div += voltage_adjustment_mult;
}


/*  It was observed that the decrease in lenght of the line does not result in direct equivalent decrease of the time required to reach the end.  
    For example, the optimal time to travel:
      - 70cm took 3.5s (around 50ms per 1 cm)
      - 42cm took 2.3s (around 54ms per 1cm) 

    In order to make rough estimate of the time required to travel the distance, the code is using linear interpolation using 2 collected data points above.
    In particular, the "map" function is used to translate the length of the track into estimated time required to travel to the end of it.

    e.g. 
    map(lenght_of_the_track_to_be_translated,   // (example scenario: the track is slightly longer, the task is to estimate how much time it will take to reach end of it using previously collected data)
        shorter_length,                         // used in previous test
        longer_length,                          // used in previous test
        time_required_to_travel_shorter_length, // observed in previous test
        time_required_to_travel_longer_length)  // observed in previous test 

    map(75.0
        42.0,
        70.0,
        2300.0,
        3520.0) 

    The return of such map funtion would be: 3737.

    Even better approach (but more complex to implement) would be to perform more timing tests and use cubic interpolation to estimate the time.
    It would allow for more precise estimation, which would be especially helpful when the track length is very low.
    */
void FollowRect::AssignEstimatedTravelTime(){
  travel_time = map(side_len, 42, 70, 2300, 3520);
  AdjustTravelTimeByBatteryLevel();  

  /*  Update at which point in time the car should ignore the front sensors.  */
  is_near_end_threshold_time = (int)((float)travel_time / (float)side_len * ((float)side_len - SENSORS_OFFSET));

  BT_SERIAL.println("travel_time = " + String(travel_time) + ", side_len = " + String(side_len) + ", is_near_end_threshold_time = " + String(is_near_end_threshold_time));
}

/*  Higher voltage results in lower estimated travel time.  */
void FollowRect::AdjustTravelTimeByBatteryLevel() {
  travel_time -= (travel_time * voltage_adjustment_mult);
}


void FollowRect::SetLongSideLen(int len) { 
  long_side_len = len; 
  BT_SERIAL.print("long_side_len = "); BT_SERIAL.println(long_side_len);
}

void FollowRect::SetShortSideLen(int len) { 
  short_side_len = len; 
  BT_SERIAL.print("short_side_len = "); BT_SERIAL.println(short_side_len);
}

void FollowRect::IncreaseFollowingSharpness() {
  following_sharpness_div -= 0.1;
  BT_SERIAL.print("following_sharpness = "); BT_SERIAL.println(following_sharpness_div);
}

void FollowRect::DecreaseFollowingSharpness() {
  following_sharpness_div += 0.1;
  BT_SERIAL.print("following_sharpness = "); BT_SERIAL.println(following_sharpness_div);
}

void FollowRect::IncreaseReboundSize() {
  rebound_size += 1;
  BT_SERIAL.print("rebound_size = "); BT_SERIAL.println(rebound_size);
}
void FollowRect::DecreaseReboundSize() {
  rebound_size -= 1;
  BT_SERIAL.print("rebound_size = "); BT_SERIAL.println(rebound_size);
}

void FollowRect::IncreasePowerSupplySignificance() {
  power_supply_significance += 0.01;
  BT_SERIAL.print("power_supply_significance = "); BT_SERIAL.println(power_supply_significance);
}

void FollowRect::DecreasePowerSupplySignificance() {
  power_supply_significance -= 0.01;
  BT_SERIAL.print("power_supply_significance = "); BT_SERIAL.println(power_supply_significance);
}


void FollowRect::OutputAllSettings() {
  BT_SERIAL.println("\n------------\nFollowRect SETTINGS:"); 
  BT_SERIAL.print("long_side_len = "); BT_SERIAL.println(long_side_len);
  BT_SERIAL.print("short_side_len = "); BT_SERIAL.println(short_side_len);
  BT_SERIAL.print("following_sharpness = "); BT_SERIAL.println(following_sharpness_div);
  BT_SERIAL.print("rebound_size = "); BT_SERIAL.println(rebound_size);
  BT_SERIAL.print("power_supply_significance = "); BT_SERIAL.println(power_supply_significance);
  BT_SERIAL.println("------------");
}

//
