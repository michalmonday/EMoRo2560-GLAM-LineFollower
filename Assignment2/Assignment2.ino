/*  CE243 C Programming and Embedded Systems - Assignment 2
    Deadline date: 10/12/2019  */
    
#include "includes.h"

/*  "bt" is a pointer to the bluetooth module object which was useful for debugging and testing.
    Testing behaviour of the robot with the cable connected is inconvenient. 
    Having the cable unplugged limits debugging capabilities. The Bluetooth
    class solves that problem by providing a way to debug and interact with the program
    using bluetooth terminal application.   */
Bt *bt;

Mode *modes[] = { new AvoidObstacles(), new FollowCircle(), new FollowRect(70, 42), new ResetMode() };

/*  Setup function is called only once at the begining of the program execution.  */
void setup() {  
  InitEmoro();   // Initializes all available inputs and outputs on EMoRo 2560.

  /*  Initiate servos (used as wheels).  */
  EmoroServo.attach(SERVO_0);      
  EmoroServo.attach(SERVO_1);     

  /*  Initiate ultrasonic sensor that will be used for obstacle avoidance by measuring distance to the object in front of the car.  */
  Ultrasonic.attach(GPP_0);

  /*  Set baud rate of serial communication (allowing to use "Tools->Serial Monitor" feature of Arduino IDE to debug the code.  */
  Serial.begin(9600);
  
  /* Output serial messages to diagnose problems in case of faulty sensor or incorrect wiring */
  if(!(ReadEmoroHardware()& LCD_AVAILABLE))
    Serial.println("LCD is not available");

  if(!(ReadEmoroHardware() & SW_AVAILABLE))
    Serial.println("Switches are not available");

  if(!(ReadEmoroHardware() & GYR_AVAILABLE))
    Serial.println("Gyroscope is not available");

  // remove it completely
  Gyr.init();
                            
  tracker_sensor = new TrackerSensor(IO_2, IO_3, IO_4); // left, center, right

  /* Call 4th button behaviour (reset), which displays the menu options. */
  Mode::Set(modes[3]);

  /* Initialize bluetooth by setting its' name and pin. */
  bt = new Bt("EMoRo mb19424", "3737");
}  

void loop() {
  bt->Update();

  /*  Declaring the array as static prevents unnecessary overwriting of memory at every iteration of the loop.
      The array is initialized only once. It's better than using global variable because it's close to where it's used.  */
  static int switch_id[] = { SW_1, SW_2, SW_3, SW_4 };
  if(ReadEmoroHardware() & SW_AVAILABLE)    // if switches are available
    for (int i = 0; i < 4; ++i) 
      if (ReadSwitch(switch_id[i]))
         Mode::Set(modes[i]);

  /* Each of the modes has its' own implementation of the Update function. */
  Mode::Get()->Update();
}
