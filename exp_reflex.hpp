#include"exp_function_series.hpp"
#include<map>
#include<string>
#include<type_traits>


namespace exp_reflex
{
	using namespace exp_function_series;
	using std::map;
	using std::string;

	//支持此类的反射需要类中定义reflex静态函数
	//先来点template

	template<class _Link_Obj> 
	class reflex_map : public map<short, string>
	{
	public:
		explicit reflex_map(_Link_Obj const& lo) :fl(lo) noexcept {}
		void _bind_name(string const& fn_name);
		void _call_from_string(string const& fn_name);

		template<class _Strs, class En = std::enable_if_t<std::is_convertible_v<string, _Strs>>>
		void _bind_names(_Strs const& ...str) { (_bind_name(str),...); }


			
	private:
		_Link_Obj fl;
	};
}