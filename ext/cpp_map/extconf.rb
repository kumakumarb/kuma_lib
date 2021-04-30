# frozen_string_literal: true

require "mkmf"

$CXXFLAGS += " -std=c++17"
create_makefile("cpp_map/cpp_map")
