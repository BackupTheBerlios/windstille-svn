#!/usr/bin/ruby -w

require "rexml/document"

$revisions = []

class Revision
  attr_reader :title, :timestamp, :contributor, :comment, :data

  def initialize(title, timestamp, contributor, comment, data)
    @title       = title
    @timestamp   = timestamp
    @contributor = contributor
    @comment     = comment
    @data        = data
  end

  def to_s()
    return "#{@timestamp} - #{@title} - #{@contributor} - #{@comment}"
  end
end

def process_revision(title, revision)
  timestamp  = nil
  data       = nil
  comment    = nil
  contribute = nil

  revision.each_element{ |el|
    case el.name
    when "timestamp" 
      timestamp   = el.text
    when "contributor" 
      contribute = el[0].text
    when "comment" 
      comment = el.text
    when "text"       
      data    = el.text
    else
      # puts "Unhandled: #{el.name}"
    end
  }

  # puts "#{title} - #{timestamp} by #{contribute}"
  $revisions.push(Revision.new(title, timestamp, contribute, comment, data))
end

def process_page(page)
  title = nil
  page.each_element{ |el|
    case el.name
    when "title"
      title = el.text.to_s
    when "revision"
      process_revision(title, el)
    else
      # puts "Unhandled: #{el.name}"
    end
  }
end

if ARGV.length == 0 then
  puts "Usage: mediawiki2git.rb FILENAME..."
else
  ARGV.each{|filename|
    xml = REXML::Document.new(File.new(filename).read())
    xml.root.each_element{ |el|
      case el.name
      when "page":
          process_page(el)
      when "siteinfo":
          
      else 
        raise "Unexpected Tag #{el.name}"
      end
    }
  }

  $revisions.sort!{ |lhs, rhs| lhs.timestamp <=> rhs.timestamp }
  $revisions.each{|rev|
    puts rev
    
    out = File.new(rev.title, 'w')
    out.write(rev.data)
    out.close()

    # system('git', 'add',  rev.title)
    # system('git', 'commit', '-m', rev.to_s)

    system('svn', 'add',  rev.title)
    system('svn', 'commit', '-m', rev.to_s)
  }
end

# EOF #
