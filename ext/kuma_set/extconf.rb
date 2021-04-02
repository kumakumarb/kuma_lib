# frozen_string_literal: true

require "mkmf"

$CXXFLAGS += " -std=c++17 -stdlib=libc++"
create_makefile("kuma_set/kuma_set")
