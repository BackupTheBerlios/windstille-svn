#include <vector>
#include "wrapper.interface.hpp"
#include "interface.hpp"
#include "sound/sound_manager.hpp"
#include "game_session.hpp"
#include "dialog_manager.hpp"
#include "conversation.hpp"
#include "script_manager.hpp"
#include "sector.hpp"
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

void show_dialog(HSQUIRRELVM vm, float fadein_time)
{
  (void) fadein_time;
  GameSession::current()->set_dialog_state();
  script_manager->set_wakeup_event(vm, ScriptManager::DIALOG_CLOSED, -1);
}

void hide_dialog(float fadeout_time)
{
  (void) fadeout_time;
  GameSession::current()->set_game_state();
}

void wait(HSQUIRRELVM vm, float time)
{
  script_manager->set_wakeup_event(vm, ScriptManager::TIME, time);
}

void add_dialog(int alignment, const std::string& portrait)
{
    DialogManager::current()->add_dialog(alignment, portrait);
    Sector::current()->get_player()->start_listening();
}

void add_question(const std::string& text)
{
    std::vector<std::string> answers;
    DialogManager::current()->add_question(text);
}

void add_answer(const std::string& answer)
{
    DialogManager::current()->add_answer(answer);
}

int dialog_answer()
{
    return DialogManager::current()->dialog_answer();
}

void end_dialog()
{
  DialogManager::current()->remove_dialog();
  GameSession::current()->set_game_state();
  Sector::current()->get_player()->stop_listening();
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

void activate_object(const std::string& name, bool active)
{
  ::GameObject* obj = Sector::current()->get_object(name);
  if (obj)
    obj->set_active(active);
  else
    console << "No such object: '" << name << "'" << std::endl;
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

void conversation_add(const std::string& text)
{
  Conversation::current()->add(text);
}

void conversation_show()
{
  Conversation::current()->show();
}

}
