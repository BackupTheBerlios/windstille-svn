function fade_in_out(r,g,b)
{
  fadeout(0.2, r,g,b);
  wait_for_fade();
  wait(1)
  caption_clear();
  fadein(3);
  wait_for_fade();
}

cutscene_begin();

set_camera_active(false);
objects.player.set_active(false);
set_view(0, 0);
spawn_object("scriptable-object", { name="planet", pos=[0,0],
                 sprite="images/planet.sprite", z_pos=1001 });
wait(3);

caption_add(BOTTOM, "The Big Boom. Humanity found out it could travel to the stars, and it did, en masse.");

wait_for_dialog();
fade_in_out(1,0,0)

caption_add(BOTTOM, "But it didn't take long for it to turn into The Big Bust. The guilds, the governments and the corporations staked out the best worlds.");

wait_for_dialog();
fade_in_out(1,1,1)

caption_add(BOTTOM, "Still, many individuals took their chances, even without military hardware and terraformers. Most worlds were so barren or so dangerous that whole colonies died out. In the Free Colonies that survive, life isn't easy.");

wait_for_dialog();
fade_in_out(1,0,1)

caption_end();

objects.player.set_active(true);
set_camera_active(true);
objects.planet.remove();

cutscene_end();

/* EOF */
