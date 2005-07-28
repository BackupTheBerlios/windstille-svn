
wait(1.5);
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
