class Coroutine
  def initialize()
    @continuation = nil
    @status = :waiting
  end

  def status()
    return @status
  end

  def update()
    if @status != :finished then
      @status = :running

      catch :wait do
        if @continuation then
          @continuation.call
        else
          self.run()
        end
        @status = :finished
      end

      if @status == :running then
        @status = :waiting
      end
    end
  end
  
  def restart()
    @status = :waiting
  end

  def wait()
    callcc {|@continuation| 
      @status = :waiting
      throw :wait
    }
  end

  def waitTime(time)
    coroutine_wait(time)
    wait()
  end

  def waitFor(entity)
    coroutine_wait(entity)
    wait()
  end

  def waitFrame()
    wait()
  end
end

# EOF #
