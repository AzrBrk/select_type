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


## The `exp_function_binder` class

The `exp_function_binder` class is a function binder that uses `exp_node` as the function arguments stack and binds arguments to a function. It is defined in the `exp_bind` namespace.

Unlike `std::function`, the `exp_function_binder` keeps the arguments within itself and does not create an extra binding object:

```cpp
#include <iostream>
#include "exp_function_binder.hpp"

using namespace exp_bind;

int foo(int i) { return i; }

struct X
{
    int foo(int i) { return i; }
};

int main()
{
    {
        // Binding a function
        exp_function_binder efb{ foo };
        efb.bind(10);

        std::cout << efb() << '\n';

        // You can also change the bound argument in a function
        efb[0] = 3;
        std::cout << "Argument changed to 3: " << efb() << '\n';

        // You can also undo the binding operation using counts
        // Note that once the function is called, the counter for the binder is reset
        efb.bind(0);
        efb.rebind_back(1);
        std::cout << efb(4) << '\n';
    }

    {
        // Binding a lambda
        auto efb = exp_bind::bind([](int i) { return i; });
        std::cout << efb(11) << '\n';
    }

    {
        // Binding a member function
        // This takes a reference to an object because that's how it works in C++
        X x{};
        auto efb = exp_bind::bind(x, &X::foo);
        std::cout << efb(12) << '\n';
    }

    return 0;
}
```

In this code snippet, the `exp_function_binder.hpp` file provides the definition for the `exp_function_binder` class in the `exp_bind` namespace. It demonstrates various examples of binding functions, lambdas, and member functions.

The first example binds a regular function `foo` using `exp_function_binder`. The bound argument is set to `10`, and the result of calling the function is printed. The bound argument is then changed to `3`, and the function is called again to show the updated result. Lastly, the binding operation is undone using counts, and the function is called with a different argument (`4`).

The second example demonstrates binding a lambda function. The lambda takes an integer argument and returns it as the result. The lambda function is bound to an `exp_function_binder` object, and the result of calling the binder with an argument of `11` is printed.

The third example shows binding a member function. An instance of the `X` struct is created, and its member function `foo` is bound using `exp_function_binder`. The result of calling the binder with an argument of `12` is printed.


### Binding a lambda or functor template

To create a binder for a lambda or functor template, you need to specify the template arguments. You will need to use the `realize_meta` function from the `function_impl.hpp` file.

```cpp
#include <iostream>
#include "exp_function_binder.hpp"
#include "function_impl.hpp"

using namespace exp_bind;
using namespace function_impl;

int main()
{
    auto f = []<class ...Args>(Args ...args)
    {
        ((std::cout << args << ' '), ...);
    };

    // Note that `realize_meta` takes a type list, not raw types, as template arguments
    auto efb = exp_bind::bind(realize_meta<exp_list<int, double, char>>(f));

    efb(1, 2.33, 'c');

    return 0;
}
```

In this code snippet, a lambda function `f` is defined with a variadic template parameter `Args`. The lambda function prints all the arguments passed to it using fold expressions. 

To bind `f` using `exp_function_binder`, the `realize_meta` function is used to convert the template arguments into a type list (`exp_list<int, double, char>`). Then, the `bind` function is called with the realized type list and the lambda function `f`.

Finally, the `efb` binder is called with arguments `1`, `2.33`, and `'c'`, resulting in the values being printed to the console.


## Function Series

The `exp_function_series` class is capable of creating a series of functions, even if they are in completely different forms. Here's an example to demonstrate its usage:

```cpp
#include <iostream>
#include "exp_function_series.hpp"

using namespace exp_function_series;

// A factory function to create a function series
// Trust me, you don't want to write down its type yourself
using exp_function_series::link_f;

int main()
{
    auto my_series = link_f(
        [](int a, double b) { return b - a; },
        [](double c) { return std::to_string(c); },
        []() { std::cout << "end\n"; }
    );

    // Use `series_bind` function to bind arguments to the function series
    series_bind(my_series, 1, 1.33, 0.33);
    
    // Use `_continue()` to execute the whole series
    my_series._continue();

    // The function series returns a tuple that contains all the return values from the functions in the series
    // You can use structured binding to get the result. Note that if a function returns void, it will return `(void*)0`,
    // so you have to add a placeholder for that result.
    auto [first, second, v] = my_series._return();

    std::cout << first << " " << second << '\n';
}
```

In the above code snippet, the `exp_function_series` class is used to create a series of functions. The `link_f` function is used as a factory function to create the function series. Multiple lambda functions representing different steps of the series are passed as arguments to `link_f`.

The `series_bind` function is used to bind the necessary arguments to the function series. In this example, the arguments `1`, `1.33`, and `0.33` are bound.

The `_continue()` function is called to execute the entire series of functions.

The function series returns a tuple containing the return values from each function in the series. Structured binding is used to retrieve the individual results, and in the case of a function returning void, a placeholder is used.

Finally, the results are printed to the console.

### Connecting Functions in the Series

The previous example demonstrates how to create and use a function series to get its result, but it doesn't show the connection between the functions inside the series. Instead of using the member function `_continue()`, the `_bind_continue()` member function automatically binds the return value from the last function and proceeds with the execution. The `final_return()` function returns the result of the final function:

```cpp
#include <iostream>
#include "exp_function_series.hpp"

using namespace exp_function_series;

// A factory function to create a function series
// Trust me, you don't want to write down its type yourself
using exp_function_series::link_f;

int main()
{
    auto my_series = link_f(
        [](int a, double b) { return b - a; },
        [](double c) { return std::to_string(c); }
    );
    
    // This only binds arguments to the first function in the series
    my_series.bind(1, 1.33);

    // The return value of the first function will automatically be bound to the stack of the second function
    my_series._bind_continue();

    std::cout << my_series.final_return() << '\n'; // Output: 0.330000
}
```

In this example, the `exp_function_series` class is used to create a series of functions. The `link_f` function is again used as a factory function to create the function series. Two lambda functions representing different steps of the series are passed as arguments to `link_f`.

The `bind()` function is used to bind the necessary arguments to the first function in the series. In this example, the arguments `1` and `1.33` are bound.

The `_bind_continue()` function is called to execute the entire series of functions. It binds the return value from the first function to the second function and then proceeds with the execution.

Finally, the result of the final function is obtained using the `final_return()` function and printed to the console.

### The `link_object` Class

The `link_object` class is a specialized function series designed for member functions. It accepts a series of member functions from the same class. Once the series is created, it can be used to generate a sequence of callable functions by accepting a pointer to an instance of the class.

In the provided code snippet, the `link_object` class is used to perform delayed construction of three `student` objects.

```cpp 
#include <iostream>
#include "exp_function_series.hpp"

using namespace exp_function_series;
using namespace std::literals;

class student
{
public:
	explicit student() noexcept {}

	void set_name(std::string n) { name = n; }
	void set_score(int sc) { score = sc; }
	void set_id(unsigned int id) { ID = id; }

	std::string get_name() const { return name; }
	double get_score() const { return score; }
	unsigned int get_id() const { return ID; }

	static auto student_series()
	{
		return link_object{
			&student::set_name,
			&student::set_id,
			&student::set_score
		};
	}

private:
	std::string name{};
	double score{};
	unsigned int ID{};
};


int main()
{
	auto s_lo = student::student_series();
	student A{}, B{}, C{};
	s_lo(&A, "Tom"s, 10000, 88.7);
	s_lo(&B, "Sherry"s, 10001, 89.4);
	s_lo(&C, "Marry"s, 10002, 99.9);

	for (const auto& i : { A, B, C })
	{
		std::cout << "Student: " << i.get_name() << " ID: " << i.get_id() << " Score: " << i.get_score() << '\n';
	}
}
```


The `student` class represents a student with various member functions for setting and getting information.

The `student_series()` static member function of the `student` class returns a `link_object` instance, which is created by passing the member functions `set_name`, `set_id`, and `set_score` to the `link_object` constructor.

In the `main()` function, `s_lo` is an instance of `link_object` created using `student::student_series()`.

Three `student` objects, `A`, `B`, and `C`, are created. The `s_lo` object is then used to call the member functions on each student object, providing the corresponding arguments.

Finally, a loop is used to print out the information of each student using the `get_name()`, `get_id()`, and `get_score()` member functions.

#### The Reflex Map

The `link_object` class in the header file "exp_function_series.hpp" has more functionality than just `_continue()`, which executes all the functions in a series. It also provides full control over the order of function execution. The `reflex_map` class in the header file "exp_reflex.hpp" is a class that quickly reflexes a class as a console interactive program, based on the `link_object` class. By making slight modifications to our `student` class, we can utilize the `reflex_map` to write a console interactive program.

The following program reads your commands to create a `student` instance. By using the command 'save', you can store the instance in a `std::vector`. At the end of the program, it displays what you created.

```cpp
#include <iostream>
#include "exp_function_series.hpp"
#include "exp_reflex.hpp"
#include <vector>

using namespace exp_function_series;
using namespace exp_reflex;
using namespace std::literals;

class student {
public:
	explicit student() noexcept {}

	void set_name(std::string n) { name = n; }
	void set_score(int sc) { score = sc; }
	void set_id(unsigned int id) { ID = id; }
	void save() {
		stu_vec.push_back(*this);
	}

	std::string get_name() const { return name; }
	double get_score() const { return score; }
	unsigned int get_id() const { return ID; }

	static auto student_series() {
		return link_object{
			&student::set_name,
			&student::set_id,
			&student::set_score,
			&student::save
		};
	}

	std::vector<student> stu_vec{};
private:
	std::string name{};
	double score{};
	unsigned int ID{};
};

int main() {
	student x{};
	reflex_map student_console{ student::student_series(), x };

	// Define the commands of our student console program
	student_console.reflex_as("name"s, "id"s, "score"s, "save"s);

	// The rm_input function returns a lambda function that reads input from std::istream base class.
	// By default, it reads from std::cin, but you can also read from a file stream or other std stream.
	// It will automatically recognize your command and apply your argument to the correct corresponding function.
	auto ipt = rm_input(student_console);
	while (ipt()) {}

	for (const auto& i : x.stu_vec) {
		std::cout << "student: '" << i.get_name() << "' ID: " << i.get_id() << " score: " << i.get_score() << "\n";
	}
}
```

Here's the input/output of our small console program. The order in which you code your commands doesn't matter; the input lambda function will find the correct function based on the order of your reflexing.

```
name Tomcat
score 78.5
id 10008
save
id 10006
score 89.5
name Sherry Porter
save
exit
student: 'Tomcat' ID: 10008 score: 78.5
student: 'Sherry Porter' ID: 10006 score: 89.5
```

## The flex_string

The flex_string is a tuple-like container for text format output, it is based on std::format, use its control flow to format a string.
```cpp
#include"flex_string.hpp"
#include"operatable.hpp"

using namespace exp_operator;
using namespace flex_string;
using namespace flex_string::flex_string_space;

int main()
{
	fstring str{ std::string{"hello world,"}, int{2023} };
	for (; str.at(1) < 2031; str.at(1) += 1)
	{
		std::cout << str.to_string() << '\n';
	}
}
```

Each part of data in `fstring` is independent, allowing users to change them quickly by index, the example above shows the basic use of fstring.
But there's more, the `fstring` can change its output format by metaprogramming, this series of template transform static string to template char array.
Allowing users to malipulate them like normal typelist 

### The delim_list

Right now, let's not disturbed by needing to write comma, space in a output string, it makes your output operation ugly. The delim_list is in namespace 
`flex_string::meta_string_stream::delimitor`, you can simply using namespace flex_string_space to using these utilities templates, here we created a IP type of string

```cpp
#include"flex_string.hpp"
#include"operatable.hpp"

using namespace exp_operator;
using namespace flex_string;
using namespace flex_string::flex_string_space;

int main()
{
	using my_ipv4_type = do_repeat<4, short>::to<fstring>;

	my_ipv4_type ip{ 192,168,0,1 };

	using ip_control = decltype(ip.control_str())::to<delim_list>::apply<exp_char<'.'>>;

	std::cout << ip.exp_to_string<fs_final<ip_control>>();
}
```
#### The typelist transforming chain
in the above codes, you can see a nested template to, it's inherited from exp_list
```cpp
template<class ...TS>
struct exp_list
{
	static constexpr size_t length = sizeof...(TS);
	template<template<class...> class TL>
	using to = TL<TS...>;
}


```
most of the typelist in `flex_string::meta_string_stream` are inherited from exp_list, or their final product is an exp_list, so users can easily swith between them.

You can also use member function transformed_string to control the output format, by this way, you will need to given a template alias

```cpp
#include"flex_string.hpp"
#include"operatable.hpp"

using namespace exp_operator;
using namespace flex_string;
using namespace flex_string::flex_string_space;

template<class sstr> using ip_control = fs_final<
	typename sstr::template to<delim_list>::template apply<exp_char<'.'>>
>;

int main()
{
	using my_ipv4_type = do_repeat<4, short>::to<fstring>;

	my_ipv4_type ip{ 192,168,0,1 };

	std::cout << ip.transformed_string<ip_control>();
}
```

You can also directly delim a std::array, or a std::tuple, the function `array_delim` is based on fstring.
```cpp
#include"flex_string.hpp"
#include<array>

using namespace flex_string;
using namespace flex_string::flex_string_space;

int main()
{
	std::array ip{ 192, 168, 0, 1 };
	std::cout << array_delim(ip, exp_char<'.'>{});
}
```

### The `wrap_list`
The wrap_list is aimed at wrapping specified elements, or, all elements, with a set of character in an output string,
It's recommand to do the wrapping operation before the delimitation, also it's one of the transforming chain typelist, which means
you can directly use the nest template `to` to transform any `exp_list` based typelist to it. 
```cpp
#include"flex_string.hpp"
#include<array>

using namespace flex_string;
using namespace flex_string::flex_string_space;

template<class sstr> using wrap_and_delim = fs_final<
	typename sstr
::template to<wrap_list>::template apply<chars_wrapper <'[', ']'>>
::template to<delim_list>::template apply<exp_char<','>>
>;

int main()
{
	fstring fstr{ 1,2,3,4,5 };
	std::cout << fstr.transformed_string<wrap_and_delim>();
}
//output: [1],[2],[3],[4],[5]
```
The `delim_list` will perform a pefect delim, there will be no extra spaces, commas or any other side effects of outputing, 
for example, if you try to print an array by a for loop
```cpp

int main()
{
	std::array arr{ 1,2,3,4,5 };
	for (const auto i : arr)
	{
		std::cout << '[' << i << ']' << ',';
	}
}
//output: [1],[2],[3],[4],[5],
```
It's very difficult to achieve a perfect delimitation of an output of elements like what the fstring can do.

To select what you need to wrap, for example, if you want to highlight only numbers in a fstring
```cpp
#include"flex_string.hpp"
#include<array>
#include<string>

using namespace std::literals;

using namespace flex_string;
using namespace flex_string::flex_string_space;

template<class T> struct select_int 
{
	static constexpr bool value = std::is_same_v<int, T>;
};

template<class sstr, class fstr_type>
struct wrap_numbers_and_delim
{
	//get a meta_array with indices of integer type in a static format string
	using get_numbers =typename select_if_list<select_int, typename to_exp_list<fstr_type>::type::template to<tag_list>>::cv_typelist;
	//wrap_list: with_indices will only apply wrapping to indices provided
	using wrap_ctrl = typename sstr::template to<wrap_list>::template with_indices<get_numbers, chars_wrapper <'[', ']'>>::wrap;
	using delim_ctrl = typename wrap_ctrl::template to<delim_list>::template apply<exp_char<' '>>;
	//fs_final will automatically try to convert the static string to which a fstring accept
	//if it fail, the std::format will throw an exception while formating the string
	using type = fs_final<delim_ctrl>;
};

template<class sstr, class fstr_type> using wrap_delim = typename wrap_numbers_and_delim<sstr, fstr_type>::type;

int main()
{
	fstring Michaelstr{ "Michael is a grade"s, int{7}, "student."s, "he heights at:"s, int{173}, "and weights at:"s, int{66}};

	std::cout << Michaelstr.transformed_string<wrap_delim>();
}
//output: Michael is a grade [7] student. he heights at: [173] and weights at: [66]
```
## Meta Programming 
### Meta Object
the meta data is not changable, while meta object can perform a changable-like behavior in a meta looper, in meta looper, the meta object recursivly change itself, the recursion is automatically done by looper, programmer doesn't need to do it manually.
All the users has to do is to provide an object and a meta-function to describe how the changes is happened, further more, there are some common meta_object generators in namespace meta_traits::common_object, which helps you to quickly generate meta_objects.

so far, there are 3 types of meta object in namespace meta_traits:
```cpp
/*A meta obj is a bind of a meta_function and an obj, each time it is invoked, it update itself to a new type,
use ::type to get the inner obj*/
template<class MMO(Obj), class F/*Define how to Update an obj*/>
struct meta_object
{
	using type = MMO(Obj);
	template<class ...Arg>
	using apply = meta_object<meta_invoke<F, MMO(Obj), Arg...>, F>;

	template<class Outer_Obj>
	using meta_set = meta_object<Outer_Obj, F>;
};

//a meta object that returns a type in each loop progress
template<class MMO(Obj), class F, class Ret>
struct meta_ret_object 
{
	using ret = meta_invoke<Ret, MMO(Obj)>;
	using type = MMO(Obj);
	template<class ...Arg>
	using apply = meta_ret_object<meta_invoke<F, MMO(Obj), Arg...>, F, Ret>;

	template<class Outer_Obj>
	using meta_set = meta_ret_object<Outer_Obj, F, Ret>;
};

template<size_t times, class MMO(Obj), class F, class break_f = meta_always_continue>
struct meta_timer_object
{
	using timer =meta_invoke<meta_break_if<break_f, quick_value_i_greater<times, 0>>, MMO(Obj)>;

	using type = MMO(Obj);
	template<class ...Arg>
	using apply = meta_timer_object<times - 1, meta_invoke<F, MMO(Obj), Arg...>, F, break_f>;

	template<class Outer_Obj>
	using meta_set = meta_timer_object<times, Outer_Obj, F, break_f>;

	template<size_t reset_time>
	using reset = meta_timer_object<reset_time, MMO(Obj), F, break_f>;
};
```
The meta object mostly use in the following looper structure:
```cpp

	//Note: looper returns a meta object, not the context itself
	//Note: with the macro MMO, it requires the template class to be a meta_object
	template<bool, class MMO(Condition), class MMO(Obj), class MMO(Generator) = meta_object<meta_empty, meta_empty_fn>> struct meta_looper
	{
			
		
		template<class ...Args> struct apply {

			//transfer current obj to  condition_obj to judge
			//transfer different context based on types of meta_object
			using _continue_t = typename meta_invoke<meta_transfer_object<MMO(Obj), MMO(Condition)>>::type;
			static const bool _continue_ = _continue_t::value;

			//invoke generator object if condition is true
			using generator_stage_o = typename invoke_meta_function_if<_continue_>::template apply<MMO(Generator), Args...>;

			//invoke Obj object if condition is true
			using result_stage_o = typename invoke_object_if<_continue_>::template apply<MMO(Obj), generator_stage_o>;

			//for debug
			using next_stage = meta_looper<_continue_, MMO(Condition), result_stage_o, generator_stage_o>;

			//recursively loop for result
			using track_apply_t = meta_invoke<invoke_meta_function_if<_continue_>, meta_looper<
				_continue_, 
				MMO(Condition),
				result_stage_o,
				generator_stage_o
			>, Args...>;
			using type = typename track_apply_t::type;
		};
	};


	template<class Cond, class MO, class Generator> struct meta_looper<false, Cond, MO, Generator>
	{
		static constexpr bool _continue_ = false;
		using type = MO;
	};
```
here is the example of using meta object to realize a type selection
```cpp
//example of using meta object to realize type select in a typelist
#include "meta_object_traits.hpp"

#define meta_while meta_looper_t

using namespace meta_traits;
//use meta object common generators
using namespace common_object;

template<size_t N, class TL> using meta_select = typename meta_while<
	//use common meta object generator to generate meta objects
	meta_timer_cnd_o,
	meta_replace_to<N>,
	meta_ret_decreasible_o<TL>
>::type;

int main()
{
	using l = exp_list<int, double, char>;

	std::cout << typeid(meta_select<1, l>).name();
}
```
in common_object namespace, there several meta objects generators
- with suffix _o, it means a common meta object generator
- with suffix _cnd_o, or _co, it means a condition meta object generator
- with suffix _ret_o, it meta_ret_object generator
- with suffix _go, it means a generator type of meta object generators
- with suffix _to, it means a meta timer object generator
you can use them to generate different meta objects.
### meta_stream

A `meta_stream` is a meta object that can be used to generate a stream object.

```cpp
template<class MMO(To), class MMO(From)> struct meta_stream
{
	using from = MMO(From);
	using to = MMO(To);
	using cache = typename MMO(From)::ret;
	using update = meta_stream<
		typename invoke_object_if<(exp_size<typename MMO(From)::type> > 0)>
		::template apply<MMO(To), MMO(From)>, meta_invoke<MMO(From)>
	>;
	template<class F> using apply = meta_invoke<F, MMO(To), MMO(From)>;
};
struct meta_stream_f
{
	template<class mo_stream, class...>
	using apply = typename mo_stream::update;
};

template<class TL>
using op_stream = meta_appendable_o<TL>;
template<class TL>
using ip_stream = meta_ret_decreasible_o<TL>;
...

template<size_t Transfer_Length, class MMO(To), class MMO(From)> 
	requires (size_of_type_list<typename MMO(From)::type>::value >= Transfer_Length)
	using meta_stream_o = meta_timer_object<
	Transfer_Length,
	meta_stream< MMO(To), MMO(From)>,
	meta_stream_f
>;

template<class MMO(To), class MMO(From)>
using meta_all_transfer = typename meta_timer_looper_t<
	meta_stream_o<size_of_type_list<typename MMO(From)::type>::value, MMO(To), MMO(From)>
>::type;
```

The purpose of the `meta_stream` is to facilitate transmission between typelists. By utilizing the `flex_string` library, a static string can be transformed into a typelist. The `meta_stream` allows for manipulation of static strings as if they were regular typelists. It enables the creation of a stream for a static string, similar to `std::cin` and `std::cout`.

The `meta_stream` consists of two meta objects: the input source and the output destination. By defining the transfer process within a `meta_stream`, it becomes possible to control static strings effectively.

In the provided example, the `ignore_space` meta function is used to filter out space characters from a static string. The `meta_all_transfer` type trait is employed to apply this transformation to the input static string, resulting in a transformed string without spaces. Finally, the transformed string is printed to the standard output stream.

This powerful mechanism offered by `meta_stream` allows for sophisticated manipulation and processing of static strings.
```cpp
//example of using meta stream to control static string
#include"flex_string.hpp"

using namespace flex_string;
using namespace flex_string_space;

//a meta function the ignored space character
struct ignore_space
{
	template<class T> struct ignore_impl :std::true_type{};

	template<char c> struct ignore_impl<exp_char<c>>
	{
		static constexpr bool value = (c == ' ');
	};
	template<class this_type, class T> using apply = ignore_impl<T>;
};

int main()
{
	static constexpr char str[]{ "hello meta stream!" };
	using str_tl = EXP_STATIC_STR(str);

	//delete all space characters in a static string
	using transform_str = meta_all_transfer<
		meta_appendable_fliter_o<chars<>, ignore_space>, 
		ip_stream<str_tl>
	>::to::type;

	std::cout << EXP_STATIC_TO_STR(transform_str);
}
```
### The fstring control string

The fstring provides a basic control string that formats its inside elements. However, the basic control string is not capable of formatting a fstring string directly. To format an fstring, the control string must be a character array with no other types inside. The template alias `fs_final` is used to convert a typelist into that format. 

Here is an example code snippet demonstrating the difference between a basic control string and the final control string:

```cpp
#include "flex_string.hpp"
#include "exp_print.hpp"

using namespace flex_string;
using namespace flex_string_space;
using exp_print::meta_print;

int main()
{
    fstring fstr{ int{}, double{}, char{} };

    using cstr = decltype(fstr.control_str());

    meta_print<cstr>{}();
    std::cout << '\n';
    meta_print<fs_final<cstr>>{}();
}
```
Output:
```
template<template<{,}>,template<{,}>,template<{,}>>
template<{,},{,},{,}>
```

The basic control string is designed to manipulate a pair of curly braces as a single type because `std::format` uses these braces to control the formatting. You don't need to worry about wrapping these formats too much. The `fs_final` will recursively unwrap all these braces. Since fstring is based on `std::format`, all the elements inside must be formattable, which means you have to provide specialization for `std::formatter` for your custom class. The fstring will check if the type is formattable using C++20 concepts.

To customize the control string, you can use the `to` function of `decltype(fstr.control_str())` and apply the `chars_wrapper` to wrap the desired characters.
```cpp
fstring fstr{ int{}, double{}, char{} };

using cstr = decltype(fstr.control_str())
    ::to<wrap_list>::apply<chars_wrapper<'[', ']'>>
    ::to<wrap_list>::apply<chars_wrapper<'[', ']'>>
    ::to<wrap_list>::apply<chars_wrapper<'[', ']'>>;

meta_print<cstr>{}();
std::cout << '\n';
meta_print<fs_final<cstr>>{}();
```
Output:
```
template<template<[,template<[,template<[,template<{,}>,]>,]>,]>,template<[,template<[,template<[,template<{,}>,]>,]>,]>,template<[,template<[,template<[,template<{,}>,]>,]>,]>>
template<[,[,[,{,},],],],[,[,[,{,},],],],[,[,[,{,},],],]>
```
The basic control string of `fstring` contains no type information about the elements inside. Therefore, the `fstring::transformed_string()` function allows you to use a template alias with two template arguments. The first template argument is the basic control string, and the second template argument is the type of the elements inside.

The previous example of selecting numbers showed that:

```cpp
template<class sstr, class fstr_type>
struct wrap_numbers_and_delim
{
    // Get a meta_array with indices of integer type in a static format string
    using get_numbers = typename select_if_list<select_int, typename to_exp_list<fstr_type>::type::template to<tag_list>>::cv_typelist;
    
    // wrap_list: with_indices will only apply wrapping to indices provided
    using wrap_ctrl = typename sstr::template to<wrap_list>::template with_indices<get_numbers, chars_wrapper <'[', ']'>>::wrap;
    using delim_ctrl = typename wrap_ctrl::template to<delim_list>::template apply<exp_char<' '>>;
    
    using type = fs_final<delim_ctrl>;
};
```

In this snippet, the `select_if_list` requires a tagged typelist with indices to perform more accurate selection. You can generate the tagged list simply by transforming the typelist to `tag_list::cv_typelist`. It provides a meta integer array with the indices of the selection type in the typelist. The `wrap_list` uses index information to perform the wrapping.


---
#### The Partially Transform

The partially typelist provides a slice of the typelist that references the original typelist. By applying a meta function to the slice, the changes will also apply to the original typelist.

```cpp
#include"flex_string.hpp"

using namespace flex_string;
using namespace flex_string_space;

struct partially_wrapped
{
	template<class TL> 
	using apply = meta_invoke<chars_wrapper<'[', ']'>,
		typename TL::template to<delim_list>
		::template apply<exp_char<','>>>;
	
};

int main()
{
	fstring fstr{ int{10}, double{23.78}, char{'k'}};

	using cstr = decltype(fstr.control_str())
		::to<partially>::at<1, 2>::transform<partially_wrapped>;

	std::cout << fstr.exp_to_string<fs_final<cstr>>();
}

```

In this example, the partially typelist creates a slice reference to the original typelist that contains references to the `double` and `char` types. It then applies `partially_wrapped` to that slice.

Output:
```
10[23.78,k]
```

#### compile time loop
The typeid operator of C++ may perform well in most situation, but it won't play well with `meta_object`, thus, the `exp_print` is for the `meta_object` users, the `exp_print::meta_print` is template class that act as the typeid operator,
except it specializes some cases of provided type:
- a typelist, output template<...>, it ignore the typelist name since in a meta_stream, a typelist is regarded as only  a container
- a type with a char type as template argument, output directly the char type content,
- a type with a size_t type as template argment, output I(I is a size_t variable)
- `meta_object` -see below
- `meta_timer_object` -see below
- `meta_istream`, `meta_ostream` -see below
The definition of meta_print something is as below
```cpp
template<class T> struct meta_print
{
	std::ostream& operator()(std::ostream& os = std::cout){...}
}
```

The meta_looper has a function version of specializing, the `while_constexpr`, in the following demo, you will see how to use `while_constexpr` and `meta_print`, to track a traverse of a tuple:
```cpp
#include"flex_string.hpp"
#include"exp_print.hpp"
#define delim "--------------------"

using namespace flex_string;
using namespace flex_string_space;
using exp_print::meta_print;

//a meta function to highlight which elemet is chose in a tuple
//using fstring format output
template<size_t I> struct wrap_at
{
    template<class sstr> using apply = fs_final<
        typename to_selectable_t<sstr>
        ::template invoke<I>
        ::template transform_to<chars_wrapper<'[', ']'>>
        ::template to<delim_list>
        ::template apply<exp_char<','>>>;
};


int main()
{
    std::tuple tp{1, "2", 3.f, 4.87};

    //track selected
    while_constexpr< // loop while the stream counter is available
        meta_stream_o<exp_size<decltype(tp)>, 
        meta_replace_o, 
        meta_istream<meta_itoa<exp_size<decltype(tp)> - 1>>>
    >{}.recursively_invoke([]<class idx_o>(auto mtp){
        std::cout << delim << '\n';
        meta_print<idx_o>{}() << ":";
        meta_print<
            exp_select<idx_o::to::type::value, decltype(mtp)>
        >{}()
            << ":\n" << tuple_format<wrap_at<idx_o::to::type::value>::template apply>(mtp) << '\n';
        std::cout << delim << '\n';
       
    }, tp);

   
 }
```
In the meta function `wrap_at`, it firstly tranform the basic control string to a `selectable_list`, the `meta_typelist::selectable_list` is an iterator for a typelist, it references each element in the typelist, 
so in each selection of index, we wrap the element with square brackets [] to highlight the selected result, and then delim all elements with ',' to output the tuple.

The `while_constexpr` will recursivly invoke the a template functor until the meta looper return false, and it will try using its result type of each stage to realize the template functor,
the above demo use a `meta_stream_object` which complex the codes for only to demonstrate the looping process, in the above demo, the `while_constexpr` fetch a value from a meta array that contains all indices
for each elements from the tuple `tp` once a time, it transfer the `meta_stream_object` to the lambda function just to show the transforming of the looping stage, the looping times is set the amounts of the
elements in `tp`.

it use the function `tuple_format`, which is from the library of `flex_string.hpp` to show the select result. it firstly transform the target tuple to a fstring type of string, and use a meta template alias as format control
in this case, it uses the meta function `wrap_at`. You can apply this function with any tuple, it'll get the same effect, no matter how many elements there are in the tuple.

Likely output(difference compiler will display different context, for example, in gcc, int will display as i):
```
--------------------
template<MO{
 object = <0>
 function = struct meta_traits::common_object::replace_transform
},SOURCE{template<<1>,<2>,<3>>}>:int:
[1],2,3,4.87
--------------------
--------------------
template<MO{
 object = <1>
 function = struct meta_traits::common_object::replace_transform
},SOURCE{template<<2>,<3>>}>:char const * __ptr64:
1,[2],3,4.87
--------------------
--------------------
template<MO{
 object = <2>
 function = struct meta_traits::common_object::replace_transform
},SOURCE{template<<3>>}>:float:
1,2,[3],4.87
--------------------
--------------------
template<MO{
 object = <3>
 function = struct meta_traits::common_object::replace_transform
},SOURCE{template<>}>:double:
1,2,3,[4.87]
--------------------
```
The `meta_print` will recognize a meta_object and display what the user cares about, it's object and function like the way `MO{object = o function = F}`, it will recognize an `ip_stream` and display it as a source.

also as I have mentioned in the very first of this tutorial the `exp_node` is the very basic data struct of all these utilities I've created in these library, you can easily use the while_constexpr to operate the `exp_node` link list.
```cpp

//use while_constexpr to iterate over the exp_vh_shared_node
#include"meta_selectable_list.hpp"
#include"exp_vh_node.hpp"


using namespace meta_typelist;//provide meta programming utilities for while_constexpr

//the exp_vh_node_node, unlike tuple, which is a structure, 
//it is a link list that contains the different types of data
struct node_forward_f
{
    //if you'd like to exam if a node is available, use this_type::next_pointer_type instead
    //by that way you will iterate the node by the pointer, instead the element itself
    template<class this_type, class ...> using apply = typename this_type::next_type;
};

template<class Node> using node_forward_o = meta_object<Node, node_forward_f>;

struct node_forward_condition
{
    template<class this_type, class ...> struct apply
    {
        static constexpr bool value = this_type::has_next;
    };
};

using node_forward_condition_o = meta_condition_c_o<node_forward_condition>;

template<class Node> constexpr bool node_has_next = Node::has_next;

int main()
{
    auto n = VH_NODE::make_vh_shared_nodes(1, 2.33, 4, 0.1f);
    //the while_constexpr<...>::recursivly_transform_invoke ask you to provide 2 functions
    //first is the main operation function
    //second is how the data to be transformed
    while_constexpr<node_forward_condition_o, node_forward_o<decltype(n)>>{}.recursively_transform_invoke(
        []<typename node>(node && nd)
    {
        std::cout << nd.value_ref() << ' ';
    }, []<typename node>(node && nd) {
        if constexpr (node_has_next<std::decay_t<node>>) { return nd.next_element(); }
        else return nd;
    }, std::move(n.next_element()/*the exp_vh_share_node uses an empty struct as first element to make sure the 
    exp_vh_shared_node is always constructible with empty constructor even if there's reference type in the link list, so you have to ignore the first element*/)
        );
}
```
Note that `node_forward_condition_o`, `node_forward_o` has already in the library, if you'd like to use the them.
# The `possibility`

## Introduction

The `possibility` utility is a meta-programming gem that aids in deducing the structure of aggregate types. This tutorial explores how to use `possibility` to extract member types from various aggregate structures.

## Getting Started

### Installation

`possibility` is a header-only library, making integration into your projects a breeze. Simply include the `possibility.hpp` header file to start utilizing its capabilities.

```cpp
#include "possibility.hpp"
```

## The Basics of `possibility`

`possibility` excels at deducing member types within aggregate structures. By providing a type list representing all possible types, `possibility` generates a type list containing the member types of the specified structure.

Let's dive into a practical example to understand its usage better.

```cpp
#include "possibility.hpp"
#include <iostream>
#include <string>

using namespace possibilities;

struct S {
    std::string str;
    std::size_t index;
    int aaa;
    char c;
    char d;
    int sg;
    double dd;
};

struct A {
    int a;
    int b;
};

struct B {
    std::size_t idx;
    std::string str;
};

int main() {
    using possi = possibilities::tl<std::string, std::size_t, char, int, double>;

    std::cout << typeid(possibility<S, possi>).name() << std::endl;
    std::cout << typeid(possibility<A, possi>).name() << std::endl;
    std::cout << typeid(possibility<B, possi>).name() << std::endl;
}
```

### Output:

```
struct possibilities::tl<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >,unsigned __int64,int,char,char,int,double>
struct possibilities::tl<int,int>
struct possibilities::tl<unsigned __int64,class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> > >
```

In this example, the `possibility` utility deduces the member types of structures `S`, `A`, and `B` based on the provided type list (`possi`).

## Important Notes

- **Flexibility**: The provided type list doesn't have to precisely match the member types or their occurrences in the structures. Just list them once, and let `possibility` do the work.

- **Compile-Time Error Handling**: If `possibility` encounters issues deducing the structure, it will trigger a compile-time error, providing early feedback on potential problems.

- **Header-Only**: Integration is simplified with the header-only nature of `possibility.hpp`. However, note that longer type lists may increase compilation times due to the nature of meta-programming.

Sure, let's elaborate on how combining the `possibility` utility with `align_offset_ptr` can aid in executing compile-time iteration through an aggregate structure: 

---
## Journeying Through Aggregate Structures with `possibility` and `align_offset_ptr`

Combining the capabilities of `possibility` and `align_offset_ptr`, we can achieve compile-time iteration through aggregate structures in C++.

`align_offset_ptr` is a versatile tool that creates a pointer, allowing access to any type of data in a structure by aligning to any address. When combined with `possibility`, it allows for the analysis and mapping of complex aggregate structures, providing an avenue for metaprogramming optimisations.

Consider the following sample to understand how these concepts work together:

```cpp
#include"meta_selectable_list.hpp"
#include"offset_poiner.hpp"
#include"possiblity.hpp"
#include<iostream>

using namespace meta_typelist;
using namespace offset_pointer;
using namespace possibilities;

/* In this meta-function, we specify how to operate an align_offset_ptr. 
We design the logic such that after obtaining an element, the pointer 
advances beyond the member. mso::cache is used to retrieve the cached 
element from the meta-istream. Therefore, during every transfer process, 
we use the cache of meta-istream. Following this, in the next transfer of 
type information, the pointer will advance over it. Thus, the first element 
from the meta-istream is ignored by adding an empty structure at the beginning 
of the typelist. */
struct offset_advance_f
{
    template<class this_offset, class T> struct advance_impl { using type = typename this_offset::template advance<sizeof(T)>; };
    template<class this_offset> struct advance_impl<this_offset, meta_empty>{ using type = this_offset; };
    template<class this_offset, class T> using apply = typename advance_impl<this_offset, T>::type;
};

using possi = possibilities::tl<int, double, char>;

// Here, a meta-stream is defined that continuously transfers member types, based on a provided type.
template<class T> using offset_stream = meta_all_transfer_o<
    meta_object<align_offset_ptr<alignof(T), 0, 0>, offset_advance_f>,
    meta_istream<decrease_transform<possibility<T, possi>>>
>;

struct X
{
    char a;
    double b;
};

int main()
{
    X x{ 'a', 2.33 };

    auto f = [&x]<typename offset_mso>
    {
        using ptr = typename offset_mso::to::type;
        using seek_type = typename offset_mso::cache;

        std::cout << ptr::get<X, seek_type>(&x);
        std::cout << ' ';
    };

    while_constexpr<offset_stream<X>>{}.recursively_invoke(f);    
} // Output: a 2.33
```

The `while_constexpr` transfers the entire `meta_stream` object to this lambda at compile-time only, so it does not affect runtime efficiency. This finely crafted amalgamation of `possibility` and `align_offset_ptr` simplifies the task of iterating over complex aggregate structures and enhances compile-time processing capabilities, effectively taking metaprogramming in C++ to a whole new level.

Certainly, we can refine the contents to provide a clearer understanding of the `align_offset_ptr`. Below is a refined version of your tutorial:

---

## Deep Dive into `align_offset_ptr`

The `align_offset_ptr` serves as a compile-time pointer simulating data layout in specific alignment. Its uniquely designed structure is independent of any other types or structures, enabling it to advance indefinitely. While this feature can be risky without guided type information, `align_offset_ptr` is designed as a template structure with meta types for safer use.

Let's understand how the `align_offset_ptr` is declared:

```cpp
template<std::size_t pack_size, std::size_t layer_offset, std::size_t offset>
struct align_offset_ptr{...};
```

In the `align_offset_ptr` structure, memory alignment is metaphorically seen as "layers." The data within memory is organized within these layers. The parameters host the alignment size (`pack_size`), the offset of the current layer (`layer_offset`), and the offset within the current layer (`offset`). Consequently, the final offset value is:

- A base unsigned `char` pointer + layer_offset * pack_size + offset.


The `align_offset_ptr` can be perceived as two parts: 

- The Advance Pointer: Moves to the end of where a certain offset should locate. The advance operation `align_offset_ptr<...>::advance<std::size_t size>` calculates the exact place.

- The Seeker Pointer: Finds the beginning of where a certain offset should locate. The seek operation `align_offset_ptr<...>::seek<std::size_t size>` performs this task.


To retrieve a certain type of data, use the static member function `get`:

```cpp
template<class C, class type> type& get(C *base_ptr);
```
Below is an example illustrating how to use `align_offset_ptr`:

```cpp
#include"offset_poiner.hpp"
#include<iostream>
#include"select_type.hpp"

using namespace offset_pointer;

int main()
{
  struct X{ int a{ 1 }; double b{ 2.33 }; double bb{ 9.79 }; char c{ 'l' }; char cc{ 'k' }; int aa{ 10 }; } x{};
   
  //a
 using xptr = align_offset_ptr<alignof(X), 0, 0>;
 std::cout << typeid(xptr::seek<sizeof(int)>).name();
 std::cout << xptr::get<X, int>(&x);
 std::cout << '\n';

 //b
 using xptr2 = xptr::advance<sizeof(int)>;
 std::cout << typeid(xptr2::seek<sizeof(double)>).name()<< "offset = " << xptr2::seek<sizeof(double)>::value + xptr2::seek<sizeof(double)>::layer * alignof(X) << ": value =";
 std::cout << xptr2::get<X, double>(&x);
 std::cout << '\n';
 //bb
 using xptr3 = xptr2::advance<sizeof(double)>;
 std::cout << typeid(xptr3::seek<sizeof(double)>).name() << "offset = " << xptr3::seek<sizeof(double)>::value + xptr3::seek<sizeof(double)>::layer * alignof(X) << ": value =";
 std::cout << xptr3::get<X, double>(&x);
 std::cout << '\n';
 //c
 using xptr4 = xptr3::advance<sizeof(double)>;
 std::cout << typeid(xptr4::seek<sizeof(char)>).name() << "offset = " << xptr4::seek<sizeof(char)>::value + xptr4::seek<sizeof(char)>::layer * alignof(X) << ": value =";
 std::cout << xptr4::get<X, char>(&x);
 std::cout << '\n';
 //cc
 using xptr5 = xptr4::advance<sizeof(char)>;
 std::cout << typeid(xptr5::seek<sizeof(char)>).name() << "offset = " << xptr5::seek<sizeof(char)>::value + xptr5::seek<sizeof(char)>::layer * alignof(X) << ": value =";
 std::cout << xptr5::get<X, char>(&x);
 std::cout << '\n'; 
 //aa
 using xptr6 = xptr5::advance<sizeof(char)>;
 std::cout << typeid(xptr6::seek<sizeof(int)>).name() << "offset = " << xptr6::seek<sizeof(int)>::value + xptr6::seek<sizeof(int)>::layer * alignof(X) << ": value =";
 std::cout << xptr6::get<X, int>(&x);
 std::cout << '\n';

 //Beware that you can still advance the pointer more, but an undefined behavior (UB) can take place.
 using xptr7 = xptr6::advance<sizeof(int)>;
    
    return 0;
    
}
```
output:
```
struct offset_pointer::align_offset_ptr<8,0,0>1
struct offset_pointer::align_offset_ptr<8,1,0>offset = 8: value =2.33
struct offset_pointer::align_offset_ptr<8,2,0>offset = 16: value =9.79
struct offset_pointer::align_offset_ptr<8,3,0>offset = 24: value =l
struct offset_pointer::align_offset_ptr<8,3,1>offset = 25: value =k
struct offset_pointer::align_offset_ptr<8,3,4>offset = 28: value =10
```
This example demonstrates how the `align_offset_ptr` is used to access and operate on various data types. Once you've reached the last member in the structure, be cautious about advancing the pointer further—doing so could lead to undefined behavior.
The `align_offset_ptr` is designed to emulate how data is arranged in memory, particularly in structures where memory alignment matters. 

Structure fields in memory are not necessarily continuous — there may be "padding bytes" inserted by compilers to ensure data is located at an address suitable for its type. That is, some types might need to be aligned to even addresses, or addresses that are multiples of 4, 8, etc. This arrangement maximizes the system's reading and writing efficiency.

`align_offset_ptr`, using its parameters `pack_size`, `layer_offset`, and `offset`, provides a scheme to simulate this placement of structure fields in layers of memory spaces, factoring in the alignment requirements.

- `pack_size` represents the size of the alignment, which corresponds to the width of each layer in memory.
- `layer_offset` is the offset or distance traversed within these layers, demonstrating the vertical shift in our layered memory model.
- `offset` is the horizontal displacement within a layer, which represents the actual shift within the boundaries of the same alignment.

In this way, `align_offset_ptr` simulates the memory layout as layers, comparing the whole memory space to a multi-level parking lot where data or cars are parked orderly within each level.

By advancing or seeking through this overall space according to the size of each field (i.e., through `align_offset_ptr<...>::advance<size>` or `align_offset_ptr<...>::seek<size>`), `align_offset_ptr` mimics the movement from one field to another, taking into account both their sizes and alignment requirements. 

This mechanism aids in visualizing the precise memory alignment and the layout of data fields in structures, particularly aiding in complex compile-time operations.

---

