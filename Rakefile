# frozen_string_literal: true

require "bundler/gem_tasks"
require "rspec/core/rake_task"

RSpec::Core::RakeTask.new(:spec)

require "rake/extensiontask"

task build: :compile

%w[
  kuma_lib
  priority_queue
  kuma_set
  cpp/map
  segment_tree
  max_flow
].each do |str|
  Rake::ExtensionTask.new(str) do |ext|
    ext.ext_dir = "ext/" + str
    ext.lib_dir = "lib/kuma_lib"
  end
end

task default: %i[clobber compile spec]
