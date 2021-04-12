# frozen_string_literal: true

require "mkmf"

$CXXFLAGS += " -std=c++17"
create_makefile("kuma_sorted_set/kuma_sorted_set")
