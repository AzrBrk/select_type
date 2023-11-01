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
