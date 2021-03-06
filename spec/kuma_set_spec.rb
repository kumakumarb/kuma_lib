# frozen_string_literal: true

KSet = KumaLib::KumaSet

RSpec.describe KSet do
  it "test_each" do
    ks = KSet.new {|val| val}
    ks.push(1, 2, 1, 3)
    expect(ks.map(&:itself).sort).to eq [1, 2, 3]
  end

  it "test_empty_and_size" do
    ks = KSet.new {|val| val}
    ks.push(1, 2)
    expect(ks.size).to eq 2
    ks.erase(1)
    expect(ks.empty?).to be false
    ks.clear
    expect(ks.empty?).to be true
  end

  it "test_insert_and_push" do
    ks = KSet.new {|val| val}
    expect(ks.insert(0)).to eq true
    expect(ks.insert(0)).to eq false
    ks.push(3, 4, 3, 5)
    expect(ks.size).to eq 4
    ks << 2 << 3
    expect(ks.size).to eq 5
  end

  it "test_erase_and_include" do
    ks = KSet.new {|val| val}
    ks.push(*(0..9))
    expect(ks.include?(0)).to eq true
    expect(ks.erase(0)).to eq true
    expect(ks.member?(0)).to eq false
  end

  it "test_find" do
    ks = KSet.new {|val| val % 2}
    ks.push(0, 1)
    expect(ks.find(3)).to eq 1
  end
end