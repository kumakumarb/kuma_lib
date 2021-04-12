# frozen_string_literal: true

SegTree = KumaLib::SegmentTree

RSpec.describe SegTree do
  it "test_set_and_get" do
    a = Array.new(10) {rand(0...10)}
    st = SegTree.new(a, -1) {|val| val}
    expect(st.get(0)).to eq a[0]
    st.set(0, 11)
    expect(st.all_prod).to eq 11
  end

  it "test_prod" do
    a = Array.new(10) {rand(0...10)}
    st = SegTree.new(a, 100) {|val| -val}
    expect(st.prod(5, 10)).to eq a[5...10].min
    expect(st.all_prod).to eq a.min
  end

  xit "test_max_right" do
    a = Array.new(10) {rand(0...10)}
    a << 11
    st = SegTree.new(a, -1) {|val| val}
    expect(st.max_right(0, 10)).to eq 10
  end

  xit "test_min_left" do
    a = Array.new(10) {rand(1...10)}
    a[1] = 11
    st = SegTree.new(a, -1) {|val| val}
    expect(st.min_left(6, 10)).to eq 2
  end
end