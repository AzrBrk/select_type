# select_type

A meta struct library for operating on meta type lists.

## exp_node

In the `select_type.hpp` file, there are various meta programming utilities available, including a basic struct called `exp_node`. This struct uses a node-like structure to store various types of data. You can create an element node using the following code:

```cpp
#include "select_type.hpp"
using node = exp_node<std::shared_ptr, int, double, char>;
```

The first template argument, `std::shared_ptr`, specifies the use of `std::shared_ptr` as the basic storage struct. It affects how the `.next` is constructed. After that, you can list the types of data you want to store in the `exp_node`. To add elements to the list, you can use the following code:

```cpp
node my_node{};
node my_node2{0};
::push_back(my_node, 2.33, shared_constructor());
```

Since `std::shared_ptr` is the most commonly used class in this meta programming library, there is a quick alias to create such a node:

```cpp
exp_shared_node<int, double, char> my_node{};
```

Additionally, there is a factory function to create a node with multiple elements:

```cpp
auto my_node = make_element_node(shared_constructor(), 1, 2.3, 'c');
```

This way, a node-like structure can be created.

## exp_iterator

The `exp_iterator` class is designed to help you traverse the `exp_node` structure. It takes a reference to an `exp_node` type and supports the C++ standard "for" semantics. It also has a deduction constructor, allowing you to use it as follows:

```cpp
int main()
{
	auto my_node = make_element_node(shared_constructor(), 1, 2.3, 'c');
	exp_iterator iter{ my_node };

	for (auto i : iter)
	{
		std::cout << i << " ";
	}
}
```

By using the `exp_iterator`, you can easily iterate through the elements stored in the `exp_node` and print them out.

```cpp
#include <iostream>
#include "select_type.hpp"

int main()
{
    auto my_node = make_element_node(shared_constructor(), 1, 2.3, 'c');
    exp_iterator iter{ my_node };

    iter[0] = 10;
    iter[1] = 2 / 6;
    iter[2] = 'k';

    for (size_t i = 0; i < ::size(my_node); ++i)
    {
        std::cout << iter[i] << " ";
    }

    return 0;
}
```

In `exp_node`, unlike `std::tuple`, the data stored in `exp_node` can be accessed by a non-compile-time `size_t` type of index, using the `operator[]` provided by the `exp_iterator` class.

In this updated code snippet, I've added a loop that iterates through all the elements stored in `my_node` and prints them out using the `operator[]` provided by the `exp_iterator` class.


## Tuple_iterator

The `tuple_iterator` class uses the base of `exp_node` and stores a reference to each element in a tuple to manipulate elements in the tuple. It has the same behavior as `exp_iterator`.

```cpp
#include <iostream>
#include <tuple>
#include "select_type.hpp"

int main()
{
    std::tuple<int, double, char> tp{ 1, 2.3, 'c' };

    tuple_iterator<std::tuple<int, double, char>> tp_it{ tp };

    auto it = tp_it.iterator();

    it[0] = 10;

    for (auto i : it)
    {
        std::cout << i << " ";
    }

    return 0;
}
```

In this code snippet, the `std::tuple` class is used to create a tuple with elements `1`, `2.3`, and `'c'`. A `tuple_iterator` is created using the `std::tuple` type. The iterator is then used to access and modify the elements in the tuple. In this case, it modifies the first element of the tuple to `10`. Finally, it iterates over the elements and prints them using `std::cout`.

## Operator

The operators for `exp_iterator` are defined in the file `operatable.hpp`, within the `exp_operators` namespace. Currently, it supports the following operators: `+`, `-`, `*`, `/`, `+=`, `-=`, `>`, and `<`.

```cpp
#include <iostream>
#include "select_type.hpp"
#include "operatable.hpp"

using namespace exp_operators;

int main()
{
    auto node = make_element_node(shared_constructor(), 1, 2.3, 'c');
    
    exp_iterator it{ node };

    while (it[0] < 10)
    {
        it[0] += 1;
        for (auto i : it)
        {
            std::cout << i << " ";
        }
        std::cout << '\n';
    }

    return 0;
}
```

In this code snippet, the `operatable.hpp` file provides the operators required for manipulating the `exp_iterator`. The `exp_operators` namespace contains the definitions for the operators `+`, `-`, `*`, `/`, `+=`, `-=`, `>`, and `<`.

The code creates an `exp_iterator` using an element node created with `make_element_node` and initialized with values `1`, `2.3`, and `'c'`. It then enters a `while` loop, incrementing the value at index `0` until it reaches `10`. Inside the loop, it prints out the elements of the iterator.


