puts "Doing a require..."
require "/home/ingo/projects/windstille/trunk/data/coroutine.rb"
puts "Doing a require...done"

class MyDoSomething<Coroutine
  def run() 
    @count = 0
    while true
      if @count >= 100 then
        # puts "Still running: #{@count}"
        @count = 0
      end
      @count += 1
      #puts "waitTime(5)"
      waitTime(5)
    end
  end
end

player_set_pos(258, 485)
player_set_direction("east")

game_add_water(1724, 1650, 3014, 1756)

game_add_water(1454, 2160, 2262, 2255)
game_add_water(2606, 2254, 2896, 2393)
game_add_water(4403, 3523, 6217, 3664)

game_add_water(438, 3329, 1100, 3542)

game_add_water(4792, 876, 5581, 980)
game_add_water(8890, 2130, 9031, 2261)

game_add_water(8378, 2130, 8520, 2261)

game_add_water(9007, 3549, 9934, 3674)

game_add_igel(668, 460)
game_add_igel(1611, 748)
game_add_igel(1993, 637)
game_add_igel(2568, 788)
game_add_igel(3038, 733)
game_add_igel(2503, 2128)
game_add_igel(3136, 2357)
game_add_igel(3431, 2360)
game_add_igel(3748, 2411)
game_add_igel(4060, 2325)
game_add_igel(4358, 2319)
game_add_igel(7091, 1182)
game_add_igel(7523, 684)
game_add_igel(7797, 685)
game_add_igel(5851, 481)
game_add_igel(5883, 451)
game_add_igel(6097, 461)
game_add_igel(1829, 190)
game_add_igel(579, 71)
game_add_igel(8570, 842)
game_add_igel(9193, 912)
game_add_igel(8986, 891)
game_add_igel(8590, 1954)
game_add_igel(7550, 1888)
game_add_igel(7214, 2124)
game_add_igel(7572, 2330)
game_add_igel(8374, 2454)
game_add_igel(7679, 3355)

game_add_brush_light(7360, 3840)
game_add_brush_exit(7360, 3648)

def startup_dialog2()
  dialog_clear()
  dialog_add("hero/portrait",
             "I repeat, buggy techdemo, incomplete, just one level, no guns only mines. Controlls via cursor key, drop mines with <control>-<down>.\n\n")
  dialog_add_answer("Ok", proc{})
  dialog_show
end

def startup_dialog()
  dialog_clear()
  dialog_add("hero/portrait",
             "This is #{$windstille_package_string}. It is a simple techdemo and " \
             "thus incomplete and a bit buggy. " \
             "The final Windstille game might also differ in quite some details. " \
             "Anyway, this techdemo provides one simple playable level. " \
             "Your goal is simply to collect as many diamonds as possible and " \
             "to kill as many enemies as you like as fast as you can. " \
             "At the end of the level you will see how many how many diamonds " \
             "you collected. " \
             "<control>-<down> will lay mines, cursor keys for controlls.\n\n" \
             "Understood everything?")
  dialog_add_answer("Yes", proc{})
  dialog_add_answer("No",  proc{ startup_dialog2()})
  dialog_show()
  remove_trigger()
end

def end_dialog
  dialog_clear()
  dialog_add("hero/portrait",
             "Mission complete!\n\n" \
             "Time needed: #{game_get_time_str()}\n" \
             "Diamonds collected: #{(game_get_max_diamonds() - game_get_diamonds()).to_i}/" \
             "#{game_get_max_diamonds().to_s}\n\n" \
             "Congratulations you finished #{$windstille_package_string}" \
             ". More levels and other stuff will follow once it is done. ")
  dialog_add_answer("Exit Game", proc{ game_quit() })
  dialog_show()
  remove_trigger()
end

puts "Ruby: adding triggre"

add_region_trigger(6000, 3968, 9000, 5000, proc{end_dialog})

# startup_dialog()

puts "Ruby: adding coroutine"
coroutine_add(MyDoSomething.new())

$igel1 = spawn_entity('frog', 500, 500)
$igel2 = spawn_entity('frog', 500, 550)
$igel3 = spawn_entity('frog', 500, 600)

$igel3.bind($igel2)
$igel2.bind($igel1)

class IgelMover<Coroutine
  def initialize(entity, speed)
    super()
    @entity = entity
    @speed  = speed
  end
  
  def run() 
    puts "Running coroutine: #{@entity}"
    @entity.move_to(@speed/2, 0)
    while true
      waitFor(@entity)
      @entity.move_to(-@speed, 0)
      waitFor(@entity)
      @entity.move_to(@speed, 0)
    end
  end
end

coroutine_add(IgelMover.new($igel1, 100))
coroutine_add(IgelMover.new($igel2, 50))
coroutine_add(IgelMover.new($igel3, 75))

# EOF #
