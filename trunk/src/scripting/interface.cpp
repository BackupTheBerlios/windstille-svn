#include "interface.hpp"
#include "sound/sound_manager.hpp"
#include "game_session.hxx"
#include "dialog_manager.hxx"
#include "script_manager.hpp"

namespace Scripting
{

void play_music(const std::string& musicfile)
{
  sound_manager->play_music(musicfile);
}

void play_sound(const std::string& soundfile)
{
  sound_manager->play(soundfile);
}

void show_dialog(const std::string& portrait, const std::string& text)
{
  DialogManager::current()->add_dialog(portrait, text);
  GameSession::current()->set_dialog_state();
}

void clear_dialog()
{
  DialogManager::current()->clear();
  GameSession::current()->set_game_state();
}

void wait(HSQUIRRELVM vm, float time)
{
  script_manager->set_wakeup_time(vm, time);
}

}

