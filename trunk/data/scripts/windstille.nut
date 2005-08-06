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

class Dialog {
  constructor(arg_align, arg_character, arg_portrait)
  {
    align = arg_align;
    character = arg_character;
    portrait = arg_portrait;
  }

  function set(text)
  {
    dialog_show(align, character, portrait, text);
    wait_for_dialog();
  }

  align = null;
  character = null;
  portrait = null;
}

function add_dialog(align, character, portrait, text)
{
  dialog_show(align, character, portrait, text);
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
