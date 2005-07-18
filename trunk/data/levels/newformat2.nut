
function display_conversation()
{
  conversation_add("How are you?");
  conversation_add("Have you seen the fire over there?");
  conversation_add("What are those two other characters doing there?");
  conversation_add("Can you sell me a gun?");
  conversation_add("Bye");
  println(conversation_get());
}

wait(1.5);
/* play_sound("sounds/speech/vr_intro.ogg"); */
dialog(TOP + RIGHT, "human/portrait", 
       "Welcome to the VR training program. Here you " +
       "will learn the basic manoeuvering abilities of your " +
       "powersuit, jumping, running, climbing and shooting. " +
       "We will start with climbing. See the block in front " +
       "of you? Press [Right] and [Jump] to hang on the ledge.");

Block0Sign.enable();

/* EOF */
