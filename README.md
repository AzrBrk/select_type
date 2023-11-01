# select_type
a meta struct lib to operate meta type list

In select_type.hpp: 
There's a lot of meta programming utility, and a basic struct called element_, it uses a node like structure to stored various type of data,
you can use the following codes to create a element node:

~~~cpp
#include"select_type.hpp"
using node = exp_node<std::shared_ptr, int ,double, char>;
~~~

The first template argument here dedicated that you're using std::shared_ptr as a basic store struct, it has an affect on how the .next is constructed. Followed by 
is a series of type you want to stored in a element_ node. you can use the following codes to add elements to that list:

~~~cpp
node my_node{};
node my_node2{0};
::push_back(my_node, 2.33, shared_constructor());
~~~

The std::shared_ptr is the most class I use to create this meta programming library, so there's quick alias to create such node:

~~~cpp
exp_shared_node<int, double, char> my_node{};
~~~

Also there is a factory function to create such a node:

~~~cpp
auto my_node = make_element_node(shared_constructor(), 1, 2.3, 'c');
~~~

This way create a node like the structure upon
