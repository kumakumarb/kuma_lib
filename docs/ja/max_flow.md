# MaxFlow
最大流問題を解くライブラリです．
最小カット問題や2分グラフの最大マッチング問題などでも用いられます．

※ ノードの番号は `int` で持っています．
※ 流量と容量は`long long`で持っています．

## 特異メソッド
#### new (n) -> MaxFlow
```ruby
mfg = KumaLib::MaxFlow.new(10)
```
`n`頂点かつ枝の無いのグラフを作成します．

## インスタンスメソッド
#### add_edge(from, to, cap) -> Integer
```ruby
mfg.add_edge(0, 1, 2) # => 0
```
頂点`from`から頂点`to`にグラフに容量`cap`の有向辺を追加し，追加された枝の`ID`を返します．

___

#### get_edge(ID) -> [from, to, cap, flow]
```ruby
mfg.get_edge(0) # => [0, 1, 2, 0]
```
`ID`に対応する枝を返します．引数の`ID`は枝を追加した時の返り値を用います．

___

#### edges() -> Array\<edge>
```ruby
mfg.edges # => [[0, 1, 2, 0]]
```
グラフの枝を全て取得します．

___

#### change_edge(ID, new_cap, new_flow) -> nil
```ruby
mfg.change_edge(0, 3, 1) # => nil
```
`ID`に対応する枝の容量と流量を`new_cap`と`new_flow`に変更します．

___

#### flow(source, sink, flow_limit = 2^(63) - 1) -> Integer
```ruby
mfg.flow(0, 2) # => hoge
```
頂点`source`から頂点`sink`への最大流を計算します．
(グラフは計算後の残余グラフになります．)

___

#### min_cut(start) -> Array\<boolean>
```ruby
mfg.min_cut(0) # => example [true, false, true, ...]
```
残余グラフにおいて頂点`start`から各ノードへ移動可能かどうかを返します．
任意の頂点`x`に対して`x`に移動可能である時のみ`mfg.min_cut(0)[x] = true`，となります．

## リンク

[ac-library maxflow.hpp](https://github.com/atcoder/ac-library/blob/master/atcoder/maxflow.hpp)
[ac-library maxflow.md](https://github.com/atcoder/ac-library/blob/master/document_ja/maxflow.md)
[ac-library-rb max_flow.rb](https://github.com/universato/ac-library-rb/blob/main/lib/max_flow.rb)
[ac-library-rb max_flow.md](https://github.com/universato/ac-library-rb/blob/main/document_ja/max_flow.md)