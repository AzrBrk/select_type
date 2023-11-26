#include"exp_function_series.hpp"
#include"operatable.hpp"
#include<map>
#include<optional>
#include<string>
#include<type_traits>


namespace exp_reflex
{
	using namespace exp_function_series;
	using namespace exp_operator;


	using std::map;
	using std::string;
	using std::optional;

	
	//LO: a series of multiple member functions from one class
	//Note: all member functions must be from one class
	template<class LO> 
	class reflex_map : public map<string, size_t>
	{
	public:
		using class_type = typename LO::class_type;
		using lo_type = LO;
	public:
		explicit reflex_map(LO const& lo, class_type& c) :rlo(lo), cref(c) {}

		//reflex functions by strings in the order of when a member function is bind in a series
		template<class ...strs> requires std::conjunction_v<
			std::is_same<std::decay_t<strs>, string>...
		> void reflex_as(strs ...function_names)
		{
			size_t pos{ 0 };
			(this->insert({ function_names, pos++ }), ...);
		}

		auto get_fl()
		{
			return rlo.create_series(&cref);
		}
			
	private:
		LO rlo;
		class_type &cref;
	};

	template<class LO>
	reflex_map(LO const& lo, typename LO::class_type&) -> reflex_map<LO>;


	template<class LO> struct LO_to_FL
	{
		using class_t = typename LO::class_type;
		using class_ptr_t = std::add_pointer_t<class_t>;
		using type = decltype(std::declval<LO>().create_series(std::declval<class_ptr_t>()));
	};

	template<class LO>
	using lo_to_fl_t = typename LO_to_FL<LO>::type;

	template<class RM>
	using rm_to_fl_t = lo_to_fl_t<typename RM::lo_type>;

	template<class RM> optional<rm_to_fl_t<RM>> str_to_rm(RM& rm, string const& f)
	{
		if (rm.find(f) == rm.end()) return std::nullopt;

		auto fl = rm.get_fl();
		fl[rm.at(f)];
		return fl;
	}

	//create a input function automatically get input from input stream
	//based on counts of arguments of a function in a function series
	//note: each function only takes inputs of it's needed
	//if your inputs are over-stack, the overflow inputs will target to the next function in series
	template<class RM> auto rm_input(RM& rm)
	{
		auto rm_ip_binder = [&rm](string const& f, string const& args)
			{
				auto rmf = str_to_rm(rm, f);
				if (!rmf) return false;

				std::istringstream iss{ args };
				auto fl = rmf.value();
				iss >> fl;
				fl.execute();
				return true;
			};
		auto rm_efb = exp_bind::bind(rm_ip_binder);
		//支持标准输入流的派生类 sstream, fstream, 默认为std::cin
		auto inputter = [rm_efb](std::istream& is = std::cin)
			{
				auto efb = rm_efb;
				std::string args;
				is >> efb;
				std::getline(is, args);
				efb.bind(args);
				return efb.apply_func();
			};
		return inputter;
	}


}