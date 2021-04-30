# SegmentTree

[AtCoder Library](https://github.com/atcoder/ac-library/blob/master/atcoder/segtree.hpp)のラッパーです．
一部まだできていません．

___
## クラスメソッド
**new(ary, e) {|val| ...} -> SegmentTree**
```ruby
a = Array.new(100) {|i| i}
st1 = Kumalib::SegmentTree.new(a, -1) {|val| val}
st2 = Kumalib::SegmentTree.new(a, 1) {|val| -val}
```
SegmentTreeのインスタンスを作成します．
`a`は元となる配列，`e`は評価値最低となるオブジェクトです．
ブロックによってオブジェクトを評価し，その評価値の大きい方が優先されます．
詳しい説明は`prod`の項を見てください．
そのため，ブロックは必ず64ビット整数を返すようにしてください．
___
## インスタンスメソッド
**set(pos, val) -> nil**
```ruby
a = Array.new(9) {|i| i}
st = Kumalib::SegmentTree.new(a, -1) {|val| val}
st.set(2, 10)
st.all_prod #=> 10
```
`pos`番目の要素を`val`に置き換えます．

**計算量** `O(log n)` (`n`は配列の長さ)
___

**get(pos) -> Object**
```ruby
a = Array.new(9) {|i| i}
st = Kumalib::SegmentTree.new(a, -1) {|val| val}
st.get(2) #=> 2 == a[2]
```
`pos`番目の要素を取得します．

**計算量** `O(1)`
___

**prod(l, r) -> Object**

**all_prod() -> Object**
```ruby
a = Array.new(9) {|i| i}
st = Kumalib::SegmentTree.new(a, -1) {|val| val}
st.prod(3, 7) #=> 7 == a[3 ... 7].max
st = Kumalib::SegmentTree.new(a, 1) {|val| -val}
st.prod(3, 7) #=> 3 == a[3 ... 7].min
```
`prod`は`a[l ... r]`の中で評価値最大の要素を返します．
`l >= r`の場合は定`e`が返ります．
`all_prod`は`a`の中で評価値最大の要素を返します．

**計算量** `O(log n)` (`n`は配列の長さ)