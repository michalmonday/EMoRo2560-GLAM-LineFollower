#include "includes.h"

#define DIR_DETECTION_TURN_THRESHOLD 3

void FollowCircle::Init() { 
  Display::Msg("Circle track", "following.");
  BT_SERIAL.print("Battery level at the begining of circle run = "); BT_SERIAL.print(ReadPowerSupply()); BT_SERIAL.println("V");
  drive.SetSpeed(600);
  drive.Forward();
}

void FollowCircle::Update() {
  bool l_,c,r;
  tracker_sensor->GetAll(l_,c,r);

  if (tracker_sensor->IsOnTheLine()) {
    last_iteration_had_right_turn = false;
    last_iteration_had_left_turn = false;
  }

  if (!direction){
    if (tracker_sensor->IsOnTheLine())
      drive.Forward();
      
    if (l_ && !r) {
      drive.Left(false, false, 1.8);
  
      if (!last_iteration_had_left_turn){
        number_of_left_turns++;
        OutputTurnsCounts();
        
        last_iteration_had_left_turn = true;
        last_iteration_had_right_turn = false;
      }
    }

    if (!l_ && r) {
      drive.Right(false, false, 1.8);
  
      if (!last_iteration_had_right_turn) {
        number_of_right_turns++;      
        OutputTurnsCounts();
        
        last_iteration_had_right_turn = true;
        last_iteration_had_left_turn = false;
      }
    }

    if (number_of_left_turns >= DIR_DETECTION_TURN_THRESHOLD)
      SetDirection(ANTI_CLOCKWISE);

    if (number_of_right_turns >= DIR_DETECTION_TURN_THRESHOLD)
      SetDirection(CLOCKWISE);
  }

  if (direction) {
    if (tracker_sensor->IsOnTheLine())
      SlightTurnTechnique();


      
    bool turn_needed =  ((l_ && !r) || (!l_ && r));
    if (turn_needed) {
      if (direction == CLOCKWISE) {
        float s = (r ? in_turn_sharpness : out_turn_sharpness);
        if (last_turn_sharpness != s) {
          last_turn_change_time = millis();

          prev_turn_sharpness = last_turn_sharpness;
          last_turn_sharpness = s;
        }
        drive.Right(false, false, SmoothenSharpness(s));
      }
      else {
        float s = (l_ ? in_turn_sharpness : out_turn_sharpness);
        if (last_turn_sharpness != s) {
          last_turn_change_time = millis();

          prev_turn_sharpness = last_turn_sharpness;
          last_turn_sharpness = s;
        }
        drive.Left(false, false, SmoothenSharpness(s));
      }


    }
  }
}

float FollowCircle::SmoothenSharpness(float s) {
  long delta = millis() - last_turn_change_time;
  if (delta >= 100) {
    BT_SERIAL.print("(" + String(millis()) + " - " + String(last_turn_change_time) + "ms) "); BT_SERIAL.print("using orig = ");  BT_SERIAL.println(s);
    return s;
  }

  float s_diff = prev_turn_sharpness - s;
  float sharpness_to_add = (100.0 - (float)delta) / 100.0 * s_diff;

  BT_SERIAL.print("(" + String(millis()) + " - " + String(last_turn_change_time) + "ms) "); BT_SERIAL.print("orig = "); BT_SERIAL.print(s); BT_SERIAL.print(", adjusted = "); BT_SERIAL.println(s + sharpness_to_add);
  return s + sharpness_to_add;
}

void FollowCircle::Reset() {
  number_of_left_turns = number_of_right_turns = NULL; 
  last_iteration_had_left_turn = last_iteration_had_right_turn = false;
  direction = NULL;
}

void FollowCircle::SetDirection(int dir) {
  direction = dir;
  BT_SERIAL.println("The number of " + String((dir == CLOCKWISE ? "right" : "left")) + " turns exceeded threshold. " + (dir == CLOCKWISE ? "Clockwise" : "Anti-Clockwise") + " direction was detected.");
  DirectionFoundSoundEffect();
  drive.SetSpeed(1000);
}

/*  Knowing the direction, slightly turn to the inside of the circle even when the sensor indicates being centrally over the black line.
    This way the movement is smooth.  */
void FollowCircle::SlightTurnTechnique() {
  if (last_turn_sharpness != slight_turn_technique_sharpness) {
    last_turn_change_time = millis();    
    prev_turn_sharpness = last_turn_sharpness;
    last_turn_sharpness = slight_turn_technique_sharpness;
  }
  if (direction == CLOCKWISE)
    drive.Right(false, false, SmoothenSharpness(slight_turn_technique_sharpness));
  else
    drive.Left(false, false, SmoothenSharpness(slight_turn_technique_sharpness));

}


void FollowCircle::OutputDirection() {
  if (!direction) 
    BT_SERIAL.println("direction = NULL");
  else
    BT_SERIAL.println("direction = " + String(direction == CLOCKWISE ? "CLOCKWISE" : "ANTI-CLOCKWISE"));
}

void FollowCircle::OutputTurnsCounts(){
  if (!direction)
    BT_SERIAL.println("L=" + String(number_of_left_turns) + ", R=" + String(number_of_right_turns));
}

/* Adapted from: https://www.hackster.io/jrance/super-mario-theme-song-w-piezo-buzzer-and-arduino-1cc2e4 */
void FollowCircle::DirectionFoundSoundEffect() {
  if (mario_sound) {
    drive.Stop();
    static int melody[] = {
      2637, 2637, 0, 2637,
      0, 2093, 2637, 0,
      3136, 0, 0,  0,
      1568, 0, 0, 0
    };
  
    static int size = sizeof(melody) / sizeof(int);
    for (int i = 0; i < size; i++) {
      tone(BUZ_BUILTIN, melody[i], 83);
      delay(108);
      tone(BUZ_BUILTIN, 0, 83);
    }
    delay(200);
    SlightTurnTechnique();
    return;
  }

  // C2
  tone(BUZ_BUILTIN, 65, 200);
}
