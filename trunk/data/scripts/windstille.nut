/* This script is read on Windstille startup, add all helper functions
   that should be available in the console all the time here */

has_nightvision <- false;
function nightvision_enabled(...)
{
  if (vargc == 1) 
    {
      if (::has_nightvision) {
        ::activate_object("nightvision", vargv[0]); 
      } 
      else if (vargv[0]) 
        {
          ::spawn_object("nightvision", {x=5});
          ::has_nightvision = true;
        }
    } else {
      return true;
    }
}

function game_speed(...)
{
  if (vargc == 1) 
    {
        set_game_speed(vargv[0]);
    } else {
        return get_game_speed();
    }
}

function conversation_get()
{
  conversation_show();
  wait_for_conversation();
  return conversation_get_selection();
}

/* EOF */
