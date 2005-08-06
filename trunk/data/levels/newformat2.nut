set_controller_help_active(true);
player_flames2.set_active(false);
player_flames.set_active(false);      

/* Some fun with fire */
function startfire()
{
  // Need to parent here because at top level "player" isn't yet created
  player_flames2.set_parent("player");
  player_flames.set_parent("player");      

  player_flames2.set_active(true);
  player_flames.set_active(true);
}

function stopfire()
{
  // FIXME: Don't just deactivate the GameObject, but let the particle
  // system run out (set_cycles(1.0)) also add a bunch of white smoke
  player_flames2.set_active(false);
  player_flames.set_active(false);
}

Block0Sign.start_flash(0.7);
Block0Sign.move_to(100, 370, 100, 2);

/* EOF */
