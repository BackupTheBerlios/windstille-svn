#include "interface.hpp"
#include "sound/sound_manager.hpp"
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

}

