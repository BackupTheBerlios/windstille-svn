function barrobot()
{
  //cutscene_begin();

  barrobot <- Dialog(BOTTOM, "Bar Robot", "images/portraits/barrobot.sprite");
  jane  <- Dialog(TOP, "Jane", "images/portraits/jane.sprite");

  barrobot.show("Hello how are you?\n" +
                "<sleep>" +
                "Do you want a drink?")

    // yagor.show("How are you?<sleep>\nEverything fine today?")
    // jane.show("Jep, thanks for asking. Buts its a bit dark here. Do you have a flashlight?")
    // yagor.show("Sure, here take....")
    // fadeout()
    // wait_for_fade()
    // fadein();
    // yagor.show("Here it is.")
    // jane.show("Ok, thanks, time to explore this apartment a bit.")

    // objects.flashlight.set_active(true);
    // objects.flashlight.set_parent("player");

    //cutscene_end();
}

/* EOF */
