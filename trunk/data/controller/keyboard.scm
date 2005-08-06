;; -*- scheme -*-
(windstille-controller
 (x-axis
  (keyboard-axis (minus "left") (plus "right"))
  (joystick-axis (device 0)  (axis 2)))

 (y-axis
  (keyboard-axis (minus "up") (plus "down"))
  (joystick-axis (device 0)  (axis 2)))
 
 (primary-button
  (keyboard-button (key "s"))
  (joystick-button (device 0)  (button 2)))

 (secondary-button
  (keyboard-button (key "d"))
  (joystick-button (device 0)  (button 3)))

 (tertiary-button
  (keyboard-button (key "a"))
  (joystick-button (device 0)  (button 1)))

 (pda-button
  (keyboard-button (key "w"))
  (joystick-button (device 0)  (button 0)))

 (aim-button
  (keyboard-button (key "shift"))
  (joystick-button (device 0)  (button 7)))
 
 (pause-button
  (keyboard-button (key "p"))
  (joystick-button (device 0)  (button 9)))
 )


;; EOF ;;