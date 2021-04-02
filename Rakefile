# frozen_string_literal: true

require "bundler/gem_tasks"
require "rspec/core/rake_task"

RSpec::Core::RakeTask.new(:spec)

require "rake/extensiontask"

task build: :compile

Rake::ExtensionTask.new("kuma_lib") do |ext|
  ext.ext_dir = "ext/kuma_lib"
  ext.lib_dir = "lib/kuma_lib"
end

Rake::ExtensionTask.new("priority_queue") do |ext|
  ext.ext_dir = "ext/priority_queue"
  ext.lib_dir = "lib/kuma_lib"
end

Rake::ExtensionTask.new("kuma_set") do |ext|
  ext.ext_dir = "ext/kuma_set"
  ext.lib_dir = "lib/kuma_lib"
end

task default: %i[clobber compile spec]
