#include "includes.h"

Bt::Bt(char name_[16], char pin[16], bool d)
  : debug(d)
{
  if(!(ReadEmoroHardware() & BLUETOOTH_AVAILABLE)) {
    Serial.println("Bluetooth is not available");
  }
  else if (Bluetooth.changeNameAndPasskey(name_, pin)) {
      Serial.println("Failed setting bluetooth name and passkey");
  }
  BT_SERIAL.setTimeout(50); // default timeout is 1000ms so it would make the bt communication slow
}

void Bt::CheckProtocol(String str) { 

  
  if (str.length() == 1){
    char c = str[0];
    switch(c){
      /* Bunch of functions to remotely and manually control the car. Just for convenience of use.  */
      case 's': drive.Stop();             break;
      case 'f': drive.Forward();          break; 
      case 'b': drive.Backward();         break;  
      case 'l': drive.Left();             break;
      case 'r': drive.Right();            break;
      case 'a': drive.Left(true);         break; // sharp
      case 'd': drive.Right(true);        break;
      case 'z': drive.Left(false, true);  break; // back (not sharp)
      case 'c': drive.Right(false, true); break;
    }

    /*  When number between 1 and 4 is received then call corresponding funcitonality of a switch.  */
    if (c >= '1' && c <= '4')
        Mode::Set(modes[c - '1']);
  }

  /*  Set the speed of the car (max 1000, it's 1000 by default)  */ 
  if (str.startsWith("spd="))
      drive.SetSpeed(str.substring(4).toInt());

  /*  Circle track direction.  */
  if (str.equals("cr_right"))
    reinterpret_cast<FollowCircle*>(modes[1])->SetClockwise();

  if (str.equals("cr_left"))
    reinterpret_cast<FollowCircle*>(modes[1])->SetAntiClockwise();

  /*  Turn at the specified number of deegres.
      Positive number supplied = move to the right
      Negative number supplied = move to the left  */
  if (str.startsWith("a=")) {
    int angle=0;
    sscanf(str.c_str(), "a=%d", &angle);
    // TurnAtAngleRelative_GyroBased(angle, false);  
    drive.Turn(angle);
  }

  /*  Same as "a=" above but will stop once the black line is spotted under the tracker_sensor module.  */
  if (str.startsWith("a_stop_at_line=")) {
    int angle=0;
    sscanf(str.c_str(), "a_stop_at_line=%d", &angle);
    //TurnAtAngleRelative_GyroBased(angle, true);    
    drive.Turn(angle, true);
  }

  /*  Requests and sends back battery level (used for adjusting time between turns in square following function)  */
  if (str.equals("battery_level")) {
    BT_SERIAL.print("battery level = "); BT_SERIAL.print(ReadPowerSupply()); BT_SERIAL.println("V");
  }
  
  /*  Update line lengths used for rect track following.  */
  if (str.startsWith("lsl=")) {
    int len=0;
    sscanf(str.c_str(), "lsl=%d", &len);
    FollowRect * fr = reinterpret_cast<FollowRect*>(modes[2]);
    fr->SetLongSideLen(len);
  }

  if (str.startsWith("ssl=")) {
    int len=0;
    sscanf(str.c_str(), "ssl=%d", &len);
    FollowRect * fr = reinterpret_cast<FollowRect*>(modes[2]);
    fr->SetShortSideLen(len);
  }

  if (str.equals("ifs")) {
    FollowRect * fr = reinterpret_cast<FollowRect*>(modes[2]);
    fr->IncreaseFollowingSharpness();
  }

  if (str.equals("dfs")) {
    FollowRect * fr = reinterpret_cast<FollowRect*>(modes[2]);
    fr->DecreaseFollowingSharpness();
  }
  /*  Allows to examine the exact contents of the string by outputting its' ascii values through serial.
      This way bytes like '\n' and '\r' can be spotted.  */
  Serial.println("Protocol::Check, str consists of:");
  for (int i = 0; i < str.length(); i++)
    Serial.println(str[i],DEC);
}


/*  Reads the string received through bluetooth.
    It also passes input of Serial monitor through bluetooth 
    (used for communication testing purposes in the early development stage)  */
void Bt::Update() {
  if (Bluetooth.connection()) {
    String rec;
  
    while (BT_SERIAL.available())
      rec += BT_SERIAL.readString();
  
    if (rec.length()) {
      rec.trim(); // removes \r\n and other leading/trailing whitespace, ref: https://www.arduino.cc/reference/en/language/variables/data-types/string/functions/trim/
  
      if (debug)
        Display::Msg(rec);
        
      CheckProtocol(rec);
      Serial.println("BT rec: " + rec);
    }
  
    while (Serial.available())
      BT_SERIAL.write(Serial.read());
  }
}
