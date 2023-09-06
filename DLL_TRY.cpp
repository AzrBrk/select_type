#pragma once
#include<iostream>
#include<boost/config.hpp>
#include "DLL_TRY.h"

namespace my_namespace
{
	class my_plugin_plus :public my_plugin_api
	{
	public:
		std::string name()const
		{
			return "my_plugin_plus";
		}
		float calculate(float x, float y) { return x + y; }
		my_plugin_plus()
		{
			std::cout << "construct of my_plugin_plus" << std::endl;
		}
		~my_plugin_plus()
		{
			std::cout << "destruct of my_plugin_plus" << std::endl;
		}
	};
	extern "C" BOOST_SYMBOL_EXPORT my_plugin_plus plugin;
	my_plugin_plus plugin;
};
