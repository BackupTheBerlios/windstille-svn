#include "interface.hpp"
#include "sound/sound_manager.hpp"
#include "game_session.hpp"
#include "dialog_manager.hpp"
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

void set_dialog(int alignment, const std::string& portrait,
        const std::string& text)
{
  DialogManager::current()->add_dialog(alignment, portrait, text);
}

void show_dialog(float fadein_time)
{
  (void) fadein_time;
  GameSession::current()->set_dialog_state();
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

void wait_for_dialog(HSQUIRRELVM vm)
{
  script_manager->set_wakeup_event(vm, ScriptManager::DIALOG_CLOSED, -1);
}

}

