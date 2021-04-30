# frozen_string_literal: true

MaxFlow = KumaLib::MaxFlow

RSpec.describe MaxFlow do
  it "all test" do
    mfg = MaxFlow.new(3);
    expect(mfg.add_edge(0, 1, 2)).to eq 0
    expect(mfg.add_edge(0, 2, 1)).to eq 1
    expect(mfg.add_edge(1, 2, 3)).to eq 2

    expect(mfg.get_edge(0)).to eq [0, 1, 2, 0]
    expect(mfg.edges).to eq [[0, 1, 2, 0], [0, 2, 1, 0], [1, 2, 3, 0]]

    expect(mfg.flow(0, 2)).to eq 3
    expect(mfg.min_cut(0)).to eq [true, false, false]
  end
end
