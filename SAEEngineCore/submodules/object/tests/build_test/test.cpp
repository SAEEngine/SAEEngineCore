/*
	Return GOOD_TEST (0) if the test was passed.
	Return anything other than GOOD_TEST (0) if the test was failed.
*/

// Common standard library headers

#include <cassert>

/**
 * @brief Return this from main if the test was passsed.
*/
constexpr static inline int GOOD_TEST = 0;

// Include the headers you need for testing here

#include <SAEEngineCore_Object.h>
#include <SAEEngineCore_Environment.h>
#include <SAEEngineCore_glObject.h>

#include <chrono>
#include <thread>
#include <fstream>
#include <sstream>
#include <string>
#include <array>
#include <iostream>
#include <cmath>


using namespace sae::engine::core;

class ElementList : public GFXView
{
public:
	void refresh() override
	{
		if (this->child_count() == 0)
			return;

		auto _x = this->bounds().left();
		auto _w = this->bounds().width();

		auto _eachWidth = (_w - ((this->child_count() - 1) * this->margin_)) / (this->child_count());

		for (auto& o : this->children())
		{
			auto& _b = o->bounds();
			_b.left() = _x;
			_b.right() = _b.left() + _eachWidth;
			_b.top() = this->bounds().top();
			_b.bottom() = this->bounds().bottom();
			_x += _eachWidth + this->margin_;
		};
	};

	ElementList(GFXContext* _context, Rect _r, pixels_t _margin = 5_px) : 
		GFXView{ _context, _r }, margin_{ _margin }
	{};

private:
	pixels_t margin_ = 0;

};


int main(int argc, char* argv[], char* envp[])
{

	GFXContext _context{ nullptr, Rect{{ 0_px, 0_px}, { 1600_px, 900_px } } };

	auto _listB = _context.bounds();

	auto _listPtr = new ElementList{ &_context, _listB, 0_px };
	_listPtr->emplace(new GFXObject{ {} });
	_listPtr->emplace(new GFXObject{ {} });
	_listPtr->emplace(new GFXObject{ {} });
	_listPtr->emplace(new GFXObject{ {} });
	_context.emplace(_listPtr);
	_context.refresh();

	_listB.right() = _listB.left() + 600_px;
	auto _list2Ptr = new ElementList{ &_context, _listB, 0_px };
	_list2Ptr->emplace(new GFXObject{ {} });
	_list2Ptr->emplace(new GFXObject{ {} });
	_context.emplace(_list2Ptr);
	_context.refresh();

	return GOOD_TEST;

};