/* This script is read on Windstille startup, add all helper functions
   that should be available in the console all the time here */

class Nightvision {
  has_object = false

  function _set(key, val) 
  {
    if (key == "enabled") {
      if (has_object) {
        ::activate_object("nightvision", val); 
      } else if (val) {
        ::spawn_object("nightvision", {x=5});
      }
    }
    return val
  }

  function _get(key) 
  { 
    if (key == "enabled") {
      ::print("Getter");
    }
    return true;
  }

  function p() {
    ::print("Hello World");
  }
}

nightvision <- Nightvision()

/* EOF */
