# frozen_string_literal: true

require "mkmf"

$CXXFLAGS += " -std=c++17"
create_makefile("max_flow/max_flow")
