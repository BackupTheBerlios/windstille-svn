like_frank <- false;
bob_seen_before <- false;
bob_knows_your_mercenary <- false;
bob_knows_job <- false;

/* Some debugging/testing stuff */
function testo(spritename)
{
  spawn_object("test-object", {
      name="Test",
      x = 435,
      y = 709,
      sprite = spritename
      });
}
