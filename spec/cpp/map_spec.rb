# frozen_string_literal: true

CppMap = KumaLib::Cpp::Map

RSpec.describe CppMap do
  sh = CppMap.new
  sh[1] = 2
  sh[-1] = -2
  em = CppMap.new

  it "default" do
    expect(sh.default).to eq nil
    expect(sh.default = 1).to eq 1
    expect(sh.default).to eq 1
    sh.default = nil
  end

  it "first" do
    expect(em.first).to eq nil
    expect(sh.first).to eq [-1, -2]
  end

  it "min" do
    expect(em.min).to eq nil
    expect(em.min(1)).to eq []
    expect(sh.min).to eq [-1, -2]
    expect(sh.min(1)).to eq [[-1, -2]]
    expect(sh.min(3)).to eq [[-1, -2], [1, 2]]
    expect(sh.min(-1)).to eq []
  end

  it "last" do
    expect(em.last).to eq nil
    expect(sh.last).to eq [1, 2]
  end

  it "max" do
    expect(em.max).to eq nil
    expect(em.max(1)).to eq []
    expect(sh.max).to eq [1, 2]
    expect(sh.max(1)).to eq [[1, 2]]
    expect(sh.max(3)).to eq [[1, 2], [-1, -2]]
    expect(sh.max(-1)).to eq []
  end

  it "each (select)" do
    expect(sh.select {|key, val| key > 0}).to eq [[1, 2]] 
  end

  it "reverse_each.to_a" do
    expect(sh.reverse_each.to_a).to eq [[1, 2], [-1, -2]]
  end

  it "empty" do
    expect(em.empty?).to eq true
    expect(sh.empty?).to eq false
  end

  it "size" do
    expect(em.size).to eq 0
    expect(sh.size).to eq 2
  end

  it "clear" do
    expect(sh.clear).to eq nil
    expect(sh.size).to eq 0
  end

  it "insert" do
    expect(sh.insert(1, 2)).to eq true
    expect(sh[1]).to eq 2
    expect(sh.insert(1, 3)).to eq false
    expect(sh[1]).to eq 2
    expect(sh.insert!(1, 3)).to eq false
    expect(sh[1]).to eq 3
    expect(sh[1] = 2).to eq 2
    expect(sh[1]).to eq 2
  end

  it "erase" do
    expect(em.erase(0)).to eq false
    expect(sh.erase(1)).to eq true
    expect(sh.size).to eq 0
  end

  it "merge!" do
    em[0] = 10
    em[2] = 2
    sh[0] = 0
    sh[1] = 1
    expect(sh.merge!(em)).to eq nil
    expect(sh.to_a).to eq [[0,0], [1,1], [2,2]]
    em.clear
    expect(sh.to_a).to eq [[0,0], [1,1], [2,2]]
  end

  it "[]" do
    expect(sh[0]).to eq 0
    expect(sh[3]).to eq sh.default
  end

  it "at" do
    expect(sh.at(0)).to eq 0
    begin
      sh.at(-1)
    rescue => e
      expect(e.class) == IndexError
    end
  end

  it "find" do
    expect(sh.find(0)).to eq [0,0]
    expect(sh.find(-1)).to eq nil
  end

  it "include?" do
    expect(sh.include?(0)).to eq true
    expect(sh.include?(-1)).to eq false
  end

  it "****_bound" do
    expect(sh.lower_bound(1)).to eq [1, 1]
    expect(sh.not_lower_bound(1)).to eq [0, 0]
    expect(sh.upper_bound(1)).to eq [2, 2]
    expect(sh.not_upper_bound(1)).to eq [1, 1]
  end
end