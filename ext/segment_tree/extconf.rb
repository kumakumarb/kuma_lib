# frozen_string_literal: true

require "mkmf"

$CXXFLAGS += " -std=c++17"
create_makefile("segment_tree/segment_tree")
