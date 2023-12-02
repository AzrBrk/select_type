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
## Meta Programming For fstring
### Meta Object
the meta data is not changable, while meta object can perform a changable-like behavior in a meta looper, in meta looper, the meta object recursivly change itself, the recursion is automatically done by looper, programmer doesn't need to do it manually.

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

//use times as condition, when times is 0, the looper with end the loop progress
template<size_t times, class MMO(Obj), class F>
struct meta_timer_object
{
	template<size_t ts> struct timer_
	{
		static const bool value = (ts > 0);
	};
	using timer = timer_<times>;
	using type = MMO(Obj);
	template<class ...Arg>
	using apply = meta_timer_object<times - 1, meta_invoke<F, MMO(Obj), Arg...>, F>;

	template<class Outer_Obj>
	using meta_set = meta_timer_object<times, Outer_Obj, F>;

	template<size_t reset_time>
	using reset = meta_timer_object<reset_time, MMO(Obj), F>;
};
```
The meta object mostly use in the following looper structure:
```cpp

	//Note: looper returns a meta object, not the context itself
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
			using track_apply_t =typename meta_looper<
				_continue_, 
				MMO(Condition),
				result_stage_o,
				generator_stage_o
			>::template apply<Args...>;
			using type = typename track_apply_t::type;
		};
	};

	template<class Cond, class MO, class Generator> struct meta_looper<false, Cond, MO, Generator>
	{
		template<class ...Args> struct apply
		{
			using type = MO;
		};
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
