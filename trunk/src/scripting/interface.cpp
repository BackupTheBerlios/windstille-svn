#include "interface.hpp"
#include "sound/sound_manager.hpp"
#include "game_session.hxx"
#include "dialog_manager.hxx"

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

void create_dialog(const std::string& portrait, const std::string& text)
{
  DialogManager::current()->add_dialog(portrait, text);
}

void clear_dialog()
{
  DialogManager::current()->clear();
}

void show_dialog()
{
  GameSession::current()->set_dialog_state();
}

}

