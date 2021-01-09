#include "SAEEngineCore_Artist.h"

#include <cassert>

namespace sae::engine::core
{
	namespace impl
	{
		

		std::optional<SpecIDFactory> SPEC_ID_FACTORY{ std::nullopt };

	};

	SpecID new_spec_id() noexcept
	{
		if (impl::SPEC_ID_FACTORY.has_value())
		{
			return impl::SPEC_ID_FACTORY->make();
		}
		else
		{
			impl::SPEC_ID_FACTORY.emplace();
			return impl::SPEC_ID_FACTORY->make();
		};
	};

}

namespace sae::engine::core
{
	void VertexData::set_count(size_type n) noexcept
	{ 
		this->vertices_ = n;
	};

	VertexData::iterator VertexData::erase(const_iterator _begin, const_iterator _end)
	{
		return this->data_.erase(_begin, _end);
	};



	void VertexData::append(const VertexData& _vdata)
	{
		assert(_vdata.spec() == this->spec());
		this->insert(this->end(), _vdata.begin(), _vdata.end());
		this->set_count(_vdata.count() + this->count());
	};

	void VertexData::erase(size_type _offset, size_type _vertSize, size_type _count)
	{
		auto _begin = this->begin() + (_offset * _vertSize);
		this->erase(_begin, _begin + (_count * _vertSize));
		this->set_count((size_type)(this->count() - _count));
	};
	void VertexData::erase(size_type _offset, size_type _vertSize)
	{
		this->erase(_offset, _vertSize, 1);
	};





}

namespace sae::engine::core
{

	bool Artist::can_insert(const VertexData& _verts) const noexcept
	{
		return _verts.spec() == this->spec();
	};
	void Artist::insert(GFXObject* _obj, const VertexData& _verts)
	{
		assert(this->can_insert(_verts) && "Cannot insert vertex data with SpecID not matching the artist's SpecID");

		this->art_.push_back(Art{ .ptr = _obj, .offset = this->vertex_data().count(), .vertices = _verts.count() });
		this->vertex_data().append(_verts);

		this->needs_refresh_ = true;

	};
	void Artist::remove(GFXObject* _obj)
	{
		auto _it = this->find(_obj);
		if (_it != this->art_.end())
		{
			auto _offset = _it->offset;
			auto _vcount = _it->vertices;
			this->vertex_data().erase(_it->offset, this->vertex_size(), _vcount);
			this->art_.erase(_it);
			for (auto& a : this->art_)
			{
				if (a.offset > _offset)
				{
					a.offset -= _vcount;
				};
			};

			this->needs_refresh_ = true;

		};
	};
	bool Artist::contains(GFXObject* _obj) const
	{
		return this->find(_obj) != this->art_.end();
	};

	void Artist::draw()
	{
		if (this->needs_refresh_)
		{
			this->refresh();
			this->needs_refresh_ = false;
		};
	};







}




