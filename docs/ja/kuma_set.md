# KumaSet

C++のSetのバインディングです．
評価関数で整数値を返す任意のオブジェクトを格納できます．

___
## 特異メソッド
___

**new {|val| ...} -> KumaSet**
```ruby
x = Kumalib::KumaSet.new {|val| val}
y = Kumalib::KumaSet.new {|val| val.to_i}
```
KumaSetのインスタンスを作成します．
ブロックによってオブジェクトを評価し，その評価値によってオブジェクトが等しいかどうかを確認します．
そのため，ブロックは必ず64ビット整数を返すようにしてください．
___
## インスタンスメソッド
___

**first -> Object | nil**

**min -> Object | nil**
```ruby
x = Kumalib::KumaSet.new {|val| val}
x << 100 << 0
x.first # => 0
```
評価値最小の要素を返します．
集合が空の場合は`nil`を返します．

**計算量** `O(1)`
___

**last -> Object | nil**

**max -> Object | nil**
```ruby
x = Kumalib::KumaSet.new {|val| val}
x << 100 << 0
x.last # => 100
```
評価値最大の要素を返します．
集合が空の場合は`nil`を返します．

**計算量** `O(1)`
___

**each -> Enumerator**

**each {|val| ...} -> self**
```ruby
x = Kumalib::KumaSet.new {|val| val}
x << 100 << 0
i = 0
x.each {|val| i += val}
p i # => 100
```
各要素に対してブロックを実行します．
`Enumerable`を`include`しているため，`map`や`to_a`など多くのメソッドがサポートされています．
ただし次に示すものは再定義しています．
* `find`
* `first`
* `max`
* `member?`
* `min`

詳しくは[ルリマ](https://docs.ruby-lang.org/ja/latest/class/Enumerable.html)参照お願いします．
___

**empty? -> bool**
```ruby
x = Kumalib::KumaSet.new {|val| val}
x.empty? # => true
```
集合が空かどうかを返します．

**計算量** `O(1)`
___

**size -> Integer**
```ruby
x = Kumalib::KumaSet.new {|val| val}
x.size # => 0
```
集合の大きさを返します．

**計算量** `O(1)`
___

**clear -> nil** (破壊的)
```ruby
x = Kumalib::KumaSet.new {|val| val}
x.insert(0)
x.clear
x.size # => 0
```
集合を空にします．

**計算量** `O(n)` (`n`は集合のサイズ)
___

**insert(Object) -> bool** (破壊的)

**push(Object, ...) -> nil** (破壊的)

**<< Object -> self** (破壊的)
```ruby
x = Kumalib::KumaSet.new {|val| val}
x.insert(0) # => true
x.insert(0) # => false
x.push(1, 2, 1, 3) # => nil
x.size # => 4
x << 5 << 0 # => x
x.szie # => 5
```
集合に要素を追加します．
`insert`は既に同じ評価値のオブジェクトがあれば`false`を，ない場合は`true`を返します．

**計算量** 要素1つ追加するのに`O(log n)` (`n`は集合のサイズ)
___

**erase(Object) -> bool** (破壊的)
```ruby
x = KumaLib::KumaSet.new {|val| val}
x.push(*(0..3))
x.erase(0)
x.to_a # => [1, 2, 3]
```
引数と同じ評価値の要素を削除します．
削除した場合に`true`を，削除しなかった場合に`false`を返します．

**計算量** `O(log n)` (`n`は集合のサイズ)
___

**include?(Object) -> bool**

**member?(Object) -> bool**
```ruby
x = KumaLib::KumaSet.new {|val| val}
x << 0
x.include?(0) # => true
```
引数と同じ評価値のものが集合に含まれているかどうかを返します．

**計算量** `O(log n)` (`n`は集合のサイズ)
___

**find(Object) -> Object | nil**
```ruby
x = KumaLib::KumaSet.new {|val| val % 2}
x.push(0, 1)
x.find(3) # => 1
```
引数と同じ評価値の要素を返します．ない場合は`nil`を返します．

**計算量** `O(log n)` (`n`は集合のサイズ)
___

**lower_bound(Object) -> Object | nil**
```ruby
x = KumaLib::KumaSet.new {|val| val}
x.push(*(0..100))
x.lower_find(44) # => 44
```
引数の評価値以上の評価値最小の要素を返します．
ない場合は`nil`を返します．

**計算量** `O(log n)` (`n`は集合のサイズ)
___

**upper_bound(Object) -> Object | nil**
```ruby
x = KumaLib::KumaSet.new {|val| val}
x.push(*(0..100))
x.upper_bound(44) # => 45
```
引数の評価値よりも大きい評価値最小の要素を返します．
ない場合は`nil`を返します．

**計算量** `O(log n)` (`n`は集合のサイズ)