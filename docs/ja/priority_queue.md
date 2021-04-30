# Priority_Queue

優先度付きキューです．

___
## クラスメソッド

**new {|val| ...} -> Priority_Queue**
```ruby
x = Kumalib::Priority_Queue.new {|val| val}
y = Kumalib::Priority_Queue.new {|val| val.to_i}
```
Priority_Queueのインスタンスを作成します．
ブロックによってオブジェクトを評価し，その評価値によってオブジェクト並べます．
そのため，ブロックは必ず64ビット整数を返すようにしてください．
___
## インスタンスメソッド

**empty? -> bool**
```ruby
x = Kumalib::Priority_Queue.new {|val| val}
x.empty? # => true
```
キューが空かどうかを返します．

**計算量** `O(1)`
___

**size -> Integer**
```ruby
x = Kumalib::Priority_Queue.new {|val| val}
x.size # => 0
```
キューのサイズを返します．

**計算量** `O(1)`
___

**top -> Object | nil**
```ruby
x = Kumalib::Priority_Queue.new {|val| val}
x << 1
x.top # => 1
```

キューに含まれる評価値最大のオブジェクトを返します．
キューが空の場合は`nil`を返します．
この時キューからの削除は行われません．

**計算量** `O(1)`
___

**<< Object -> self**

**push(Object, ...) -> nil**
```ruby
x = Kumalib::Priority_Queue.new {|val| val}
x << 1 << 2
x.push(2, 3, 4)
x.size # => 5
```

キューにオブジェクトを追加します．

**計算量** 1つ追加するのに`O(log n)` (`n`はキューのサイズ)
___

**pop -> nil**
```ruby
x = Kumalib::Priority_Queue.new {|val| val}
x.push(1, 2, 3)
x.pop
x.size # => 2
```

キューに含まれる評価値最大のオブジェクトを削除します．

**計算量** `O(1)` (`n`はキューのサイズ)
___

**shift -> Object | nil**
```ruby
x = Kumalib::Priority_Queue.new {|val| val}
x.push(1, 2, 3)
x.shift # => 3
x.size # => 2
```

キューに含まれる評価値最大のオブジェクトを削除して返します．
キューが空の場合は`nil`を返します．
つまり，`top`と`pop`を同時に行います．

**計算量** `O(log n)` (`n`はキューのサイズ)
___

**clear -> nil**
```ruby
x = Kumalib::Priority_Queue.new {|val| val}
x.push(*(0...100))
x.clear
x.empty? # => true
```

キューを空にします．

**計算量** `O(n)` (`n`はキューのサイズ)