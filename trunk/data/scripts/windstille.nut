/* This script is read on Windstille startup, add all helper functions
   that should be available in the console all the time here */

has_nightvision <- false;
function nightvision_enabled(...)
{
  if (vargc == 1) 
    {
      if (::has_nightvision) {
        nightvision.set_active(vargv[0]); 
      } 
      else if (vargv[0]) 
        {
          ::spawn_object("nightvision", {});
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

function dialog(align, portrait, text)
{
        dialog_show(align, portrait, text);
        wait_for_dialog();
}

/* Some debugging/testing stuff */
function testo(spritename)
{
  spawn_object("test-object", {
      name = "Test",
      pos = [435, 709],
      sprite = spritename
      });
}

/* EOF */
