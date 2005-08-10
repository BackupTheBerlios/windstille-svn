set_camera_active(false);
objects.player.set_active(false);
set_view(0, 0);
spawn_object("scriptable-object", { name="planet", pos=[0,0],
    sprite="images/planet.sprite", z_pos=1001 });
wait(3);
add_caption(BOTTOM, "The Big Boom. Humanity found out it could travel to the stars, and it did, en masse.");
wait(10);
add_caption(BOTTOM, "But it didn't take long for it to turn into The Big Bust. The guilds, the governments and the corporations staked out the best worlds.");
wait(10);
add_caption(BOTTOM, "Still, many individuals took their chances, even without military hardware and terraformers. Most worlds were so barren or so dangerous that whole colonies died out. In the Free Colonies that survive, life isn't easy.");
wait(15);
end_caption();
objects.player.set_active(true);
set_camera_active(true);
objects.planet.remove();

/* EOF */
