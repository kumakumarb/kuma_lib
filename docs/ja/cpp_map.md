# CppMap

C++の`std::map<long long int, void *>`のバインディングです．
Hashのように整数値をキーにして任意のオブジェクトを格納します．
Hashとの違いとしてキーを照準に取り出すことができます．

___
## 特異メソッド
___

#### new(defalt = nil) -> CppMap
```ruby
x = Kumalib::CppMap.new
y = Kumalib::CppMap.new(0)
```
CppMapのインスタンスを作成します．
引数でデフォルト値を指定します．
___
## インスタンスメソッド
___

#### default -> Object
#### default = Object -> nil
```ruby
y.default # => 1
y.default = 2 # => nil
```
デフォルト値を求めたり変更したりします．

**計算量** `O(1)`
___

#### first -> [key, val] | nil
```ruby
x.first # => nil
x[0] = 1
x.first # => [0, 1]
```
最小のキーと対応する値の組みを返します．
キーが存在しない場合は`nil`を返します．

**計算量** `O(1)`
___

#### min(m = 1) -> Array | [key, val] | nil
```ruby
x.min    # => [0, 1]  x が空の場合は nil を返す
x.min(1) # => [[0, 1]] x が空の場合は [] を返す
```
キーが小さい方から`m`個を配列にして返します．
`m`を指定しなかった場合は`first`と同じ処理を行います．
`m`を指定した場合，`m`が正の数でない時は空配列を返し，`m`が`x`のサイズより大きい時は返り値の配列の長さは`x`のサイズになります．

**計算量** `O(m)`
___

#### last -> [key, val] | nil
```ruby
x[1] = 2
x.last # => [1, 2]
```
最大のキーと対応する値の組みを返します．
集合が空の場合は`nil`を返します．

**計算量** `O(1)`
___

#### max(m = 1) -> Array | [key, val] | nil
```ruby
x.max    # => [1, 2]
x.max(2) # => [[1, 2], [0, 1]]
```
キーが大きい方から`m`個を配列にして返します．
`m`を指定しなかった場合は`first`と同じ処理を行います．
`m`を指定した場合，`m`が正の数でない時は空配列を返し，`m`が`x`のサイズより大きい時は返り値の配列の長さは`x`のサイズになります．

**計算量** `O(m)`
___

#### each -> Enumerator
#### each {|key, val| ...} -> self
```ruby
x.select {|key, val| key > 0} # => [[1, 2]]
```
キーの小さい方から順にブロックを実行します．
`Enumerable`を`include`しているため，`map`や`to_a`など多くのメソッドがサポートされています．
ただし次に示すものは再定義しています．
* `find`
* `first`
* `max`
* `member?`
* `min`

詳しくは[ルリマ](https://docs.ruby-lang.org/ja/latest/class/Enumerable.html)参照お願いします．
___

**reverse_each -> Enumerator**

**reverse_each {|key, val| ...} -> self**
```ruby
x.reverse_each.to_a # => [[1, 2], [0, 1]]
```
キーの大きい方から順にブロックを実行します．
___

#### empty? -> bool
```ruby
x.empty? # => false
```
空かどうかを返します．

**計算量** `O(1)`
___

#### size -> Integer
```ruby
x.size # => 2
```
大きさを返します．

**計算量** `O(1)`
___

#### clear -> nil
```ruby
x.clear # => nil
```
空にします．

**計算量** `O(x.size)`
___

#### insert(key, val) -> bool
#### insert!(key, val) -> bool
#### [key] = val -> val
#### emplace(key, val) -> bool
```ruby
x.insert(0, 1)  # => true
x.insert(0, 2)  # => false
x[0]            # => 1
x.insert!(0, 2) # => false
x[0]            # => 2
x[0] = 3        # => 3
x[0]            # => 3
```
要素を追加します．
返り値は`x`に同じキーが存在しなかった時に`true`となります．
ただし，**valが挿入されるかどうかはメソッドによって異なります．**
既にキーが存在する時，`insert`では`val`は挿入されません．
一方で同じ時`insert!`では`val`が挿入されます．（つまり上書きされます．）
`[]=`は`insert!`を実行するので上書きします．
`emplace`は現状`insert`と同じです．

**計算量** `O(log(x.size))`
___

#### erase(key) -> bool
```ruby
x.erase(1) # => false
```
要素を削除します．
削除した場合に`true`を，そもそもキーが存在しなかった場合に`false`を返します．

**計算量** `O(log(x.size))`
___

#### merge!(CppMap) -> nil
```ruby
x[0] = 0
y[0] = y[1] = 1
x.merge!(y) # => nil
x.to_a      # => [[0,0], [1,1]]
```
マージします．キーが重複した時はレシーバーの`x`が優先されます．

**計算量** `O(y.size * log(x.size + y.size))`
___

#### [key] -> val
#### at(key) -> val
```ruby
x[0] = 1
x[0] # => 1
x.at(0) # => 1
```
キーに対応するオブジェクトを取り出します．`[]`と`at`の違いはキーが存在しないときの処理です．
キーが存在しない時`[key]`は`[key] = defalt`**を実行します**．
一方で`at`は例外を発生させます．

**計算量** `O(log(x.size))`
___

#### find(key) -> [key, val] | nil
```ruby
x.find(0) # => [0, 1]
```
キーとオブジェクトの組みを取り出します．
キーが存在しない場合は`nil`を返します．

**計算量** `O(log(x.size))`
___

#### include?(key) -> bool
#### member?(key) -> bool
```ruby
x.include?(0) # => true
```
キーが存在するかどうかを返します．
`find`と等価です．

**計算量** `O(log(x.size))`
___

#### lower_bound(key) -> [key, val] | nil
#### >= key -> [key, val] | nil
```ruby
x = KumaLib::SoertedHash.new
(0..3).each {|i| x[i] = i * 10}
x.lower_bound(1) # => [1, 10]
```
引数のキー以上のキーとオブジェクトの組みを返します．
そのようなキーが存在しないい場合は`nil`を返します．

**計算量** `O(log(x.size))`
___

#### upper_bound(key) -> [key, val] | nil
#### > key -> [key, val] | nil
```ruby
x = KumaLib::SoertedHash.new
(0..3).each {|i| x[i] = i * 10}
x.upper_bound(1) # => [2, 20]
```
引数のキーよりも大きいキーとオブジェクトの組みを返します．
そのようなキーが存在しないい場合は`nil`を返します．

**計算量** `O(log(x.size))`

___

#### not_lower_bound(key) -> [key, val] | nil
#### < key -> [key, val] | nil

引数のキーよりも小さいキーとオブジェクトの組みを返します．
そのようなキーが存在しないい場合は`nil`を返します．

**計算量** `O(log(x.size))`
___

#### not_upper_bound(key) -> [key, val] | nil
#### <= key -> [key, val] | nil

引数のキー以上のキーとオブジェクトの組みを返します．
そのようなキーが存在しないい場合は`nil`を返します．

**計算量** `O(log(x.size))`

## リンク
[c++日本語リファレンス map](https://cpprefjp.github.io/reference/map/map.html)