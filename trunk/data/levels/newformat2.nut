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
  player_flames2.set_active(false);
  player_flames.set_active(false);
}

/* play_sound("sounds/speech/vr_intro.ogg"); */
dialog(TOP, "portrait", 
       "<sin>Welcome</sin> to the <large>VR training program</large>. Here you " +
       "will learn the <sin>basic manoeuvering</sin> abilities of <large>your</large> " +
       "powersuit,<sleep> jumping,<sleep> running, <sleep> climbing<sleep> and shooting. " +
       "We will start with <small>climbing</small>. See the <i>block</i> in front " +
       "of you? Press <b>Right</b> and <b>Jump</b> to hang on the ledge.");

Block0Sign.start_flash(0.7);
Block0Sign.move_to(100, 370, 100, 2);

/* EOF */
