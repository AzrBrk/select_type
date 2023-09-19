#pragma once
#include<string>
#include<vector>

using std::string;
using std::vector;
struct vs_auto_iterator
{
	typedef vector<string>::iterator vs_iter;
	vs_iter start_iter;
	vs_iter end_iter;
	vs_iter cur_iter;
	bool reaches_end = false;
	bool at_start = false;
	vs_auto_iterator(vector<string>& vs_out_op)
		:
		start_iter(vs_out_op.begin()),
		end_iter(vs_out_op.end()),
		cur_iter(vs_out_op.begin())
	{
		at_start = true;
	}
	string str()
	{
		if (reaches_end)
		{
			return "";
		}
		else
			return *(cur_iter);
	}

	vs_auto_iterator operator++()
	{
		if (cur_iter == end_iter)
		{
			reaches_end = true;
		}
		else
		{
			if (cur_iter == start_iter)
			{
				at_start = false;
			}
			++cur_iter;
			if (cur_iter == end_iter)
			{
				reaches_end = true;
			}
		}
		return *this;
	}
	vs_auto_iterator operator--()
	{
		if (cur_iter == start_iter)
		{
			at_start = true;
			
		}
		else
		{
			if (cur_iter == end_iter)
			{
				reaches_end = false;
			}
			--cur_iter;
			if (cur_iter == start_iter)
			{
				at_start = true;

			}
			return *this;
		}
	}
	
};

