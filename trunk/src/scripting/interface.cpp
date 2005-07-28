#include <vector>
#include "wrapper.interface.hpp"
#include "interface.hpp"
#include "sound/sound_manager.hpp"
#include "game_session.hpp"
#include "dialog_manager.hpp"
#include "conversation.hpp"
#include "script_manager.hpp"
#include "sector.hpp"
#include "fonts.hpp"
#include "serialize.hpp"

namespace Scripting
{

void set_sector(const std::string& filename)
{
  GameSession::current()->set_sector(filename);
}

void play_music(const std::string& musicfile)
{
  sound_manager->play_music(musicfile);
}

void stop_music(float fadetime)
{
  sound_manager->stop_music(fadetime);
}

void play_sound(const std::string& soundfile)
{
  sound_manager->play(soundfile);
}

void wait(HSQUIRRELVM vm, float time)
{
  script_manager->set_wakeup_event(vm, ScriptManager::TIME, time);
}

void wait_for_dialog(HSQUIRRELVM vm)
{
  script_manager->set_wakeup_event(vm, ScriptManager::DIALOG_CLOSED);
}

void dialog_show(int alignment, const std::string& portrait, const std::string& text)
{
  DialogManager::current()->add_dialog(alignment, portrait, text);
  GameSession::current()->set_dialog_state();
}

bool run_before(HSQUIRRELVM vm)
{
  return script_manager->run_before(vm);
}

void load_state(HSQUIRRELVM v, const std::string& filename)
{
  sq_pushroottable(v);
  load_squirrel_table(v, -1, filename);
  sq_pop(v, 1);
}

void save_state(HSQUIRRELVM v, const std::string& filename)
{
  sq_pushroottable(v);
  save_squirrel_table(v, -1, filename);
  sq_pop(v, 1);
}

void list_objects()
{
  // std::vector<::GameObject*> didn't work me, the typedef is the workaround
  typedef ::GameObject GameObject;
  std::vector<GameObject*>* objects = Sector::current()->get_objects();
  
  for(std::vector<GameObject *>::iterator i = objects->begin(); i != objects->end(); ++i)
    {
      if (!(*i)->get_name().empty())
        console << (*i)->get_name() << std::endl;
    }
}

float get_game_speed()
{
  return game_speed;
}

void set_game_speed(float v)
{
  game_speed = v;
}

float get_text_speed()
{
  return text_speed;
}

void set_text_speed(float v)
{
  text_speed = v;
}

void conversation_add(const std::string& text)
{
  Conversation::current()->add(text);
}

void conversation_show()
{
  Conversation::current()->show();
}

int  conversation_get_selection()
{
  return Conversation::current()->get_selection();
}

void wait_for_conversation(HSQUIRRELVM v)
{
  script_manager->set_wakeup_event(v, ScriptManager::CONVERSATION_CLOSED);
}

int display(HSQUIRRELVM v) __custom
{
  console << squirrel2string(v, -1);
  return 0;
}

int println(HSQUIRRELVM v) __custom
{
  console << squirrel2string(v, -1) << std::endl;
  return 0;
}

void set_debug(bool t)
{
  debug = t;
}

bool get_debug()
{
  return debug;
}

void set_console_font(const std::string& font, int size)
{
  TTFFont* oldfont = Fonts::ttffont;

  try {
    Fonts::ttffont = new TTFFont("fonts/" + font, size);
    delete oldfont;
  } catch(std::exception& err) {
    console << err.what() << std::endl;
  }
}

}
