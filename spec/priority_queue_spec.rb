# frozen_string_literal: true

PrioQueue = KumaLib::PriorityQueue

RSpec.describe PrioQueue do
  it "test_top" do
    pq = PrioQueue.new {|v| v}
    expect(pq.top).to eq nil
    pq.push(3)
    expect(pq.top).to eq 3
  end

  it "test_push_and_pop" do
    pq = PrioQueue.new {|v| v}
    pq.push(3, 1, 8)
    expect(pq.top).to eq 8
    pq.pop
    expect(pq.top).to eq 3
    pq << 4 << 2
    expect(pq.top).to eq 4
  end

  it "test_shift_and_empty" do
    pq = PrioQueue.new {|v| v}
    expect(pq.shift).to eq nil
    pq.push(2, 3)
    expect(pq.shift).to eq 3
    expect(pq.size).to eq 1
  end

  it "test_size_and_clear" do
    pq = PrioQueue.new {|v| v}
    pq.push(*(1..10))
    expect(pq.size).to eq 10
    pq.clear
    expect(pq.empty?).to eq true
  end
end
