/* This file allows to include all the necessary header files concisely using a single "#include" statement. */

#ifndef INCLUDES_H
#define INCLUDES_H

#include "Assignment2.h"

/* Bluetooth class allows controlling the car remotely and debugging its' state conveniently.
   Its' use is straightforward and includes:
   - initialization
        Bt *bt = new Bt("EMoRo mb19424", "3737");
   - 1 instruciton to be called in a loop
        bt->Update();
   - sending messages using BT_SERIAL alias of Serial1
        BT_SERIAL.println("Some var =" + String(some_var));  */
#include "bluetooth.h"

/* Display namespace encapsulates "Msg" functions with 1,2 and 4 parameters. */
#include "display.h"

/* Drive class provides the functions to control the movement of Emoro robot. */
#include "drive.h"

/* Mode is an abstract class. Thanks to it (and the use of polymorphism), 
   different modes can be switched in a concise way. */
#include "mode.h"

/*  Set of subclasses implementing the Mode abstract class.  */
#include "mode_AvoidObstacles.h"
#include "mode_FollowCircle.h"
#include "mode_FollowRect.h"
#include "mode_ResetMode.h"

/* TrackerSensor class holds functions to get state of the 3 sensors from the following module: https://www.ebay.co.uk/itm/4Pcs-3-Way-Infrared-IR-Line-Tracking-Sensor-Module-For-Raspberry-Pi-U3/193184039763
   It can be done using tracker_sensor.GetAll method.  */
#include "tracker_sensor.h"

#endif
