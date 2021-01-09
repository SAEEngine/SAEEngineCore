#pragma once
#ifndef SAE_ENGINE_CORE_ARTIST_H
#define SAE_ENGINE_CORE_ARTIST_H

#include <SAEEngineCore_Event.h>

#include <concepts>
#include <type_traits>
#include <unordered_set>
#include <vector>
#include <optional>

namespace sae::engine::core
{
	class GFXObject;

	using SpecID = uint32_t;

	namespace impl
	{
		class SpecIDFactory
		{
		public:
			constexpr SpecID make() noexcept
			{
				return this->count_++;
			};

		private:
			SpecID count_ = 0;
		};
		extern std::optional<SpecIDFactory> SPEC_ID_FACTORY;
	};
	SpecID new_spec_id() noexcept;

	class VertexData
	{
	private:
		using container_type = std::vector<std::byte>;

	public:
		using size_type = typename container_type::size_type;

		using iterator = typename container_type::iterator;
		using const_iterator = typename container_type::const_iterator;

		auto begin() noexcept { return this->data_.begin(); };
		auto begin() const noexcept { return this->data_.cbegin(); };
		auto cbegin() const noexcept { return this->data_.cbegin(); };

		auto end() noexcept { return this->data_.end(); };
		auto end() const noexcept { return this->data_.cend(); };
		auto cend() const noexcept { return this->data_.cend(); };

		auto* data() noexcept { return this->data_.data(); };
		const auto* data() const noexcept { return this->data_.data(); };

		auto spec() const noexcept { return this->spec_; };

		/**
		 * @brief Returns the number of vertices currently stored 
		*/
		auto count() const noexcept { return this->vertices_; };

		/**
		 * @brief Returns the total size of the vertex data in bytes
		*/
		auto size() const noexcept { return this->data_.size(); };

	protected:
		/**
		 * @brief Sets the vertex count value to n
		*/
		void set_count(size_type n) noexcept;

		template <typename IterT>
		auto insert(const_iterator _at, IterT _srcBegin, const IterT _srcEnd)
		{
			return this->data_.insert(_at, _srcBegin, _srcEnd);
		};

		iterator erase(const_iterator _begin, const_iterator _end);

	public:

		void append(const VertexData& _vdata);

		/**
		 * @brief Erases vertices from the container
		 * @param _offset Offset in vertices
		 * @param _vertSize Size of a single vertex in bytes
		 * @param _count Number of vertices to erase
		*/
		void erase(size_type _offset, size_type _vertSize, size_type _count);

		/**
		 * @brief Erases one vertex from the container
		 * @param _offset Offset in vertices
		 * @param _vertSize Size of a single vertex in bytes
		*/
		void erase(size_type _offset, size_type _vertSize);






		explicit VertexData(SpecID _spec, size_type _vertices, size_type _vertexSize) :
			spec_{ _spec }, vertices_{ _vertices }, data_( _vertices * _vertexSize )
		{};
		explicit VertexData(SpecID _spec) :
			VertexData{ _spec, 0, 0 }
		{};
		
	private:
		SpecID spec_ = 0;
		size_type vertices_ = 0;
		std::vector<std::byte> data_{};
	};

	class Artist
	{
	private:
		struct Art
		{
			friend inline constexpr bool operator==(const Art& _lhs, const GFXObject* _ptr) noexcept
			{
				return _lhs.ptr == _ptr;
			};
			friend inline constexpr bool operator!=(const Art& _lhs, const GFXObject* _ptr) noexcept
			{
				return !(_lhs == _ptr);
			};

			GFXObject* ptr;
			VertexData::size_type offset;
			VertexData::size_type vertices;
		};

		
	protected:
		auto& vertex_data() noexcept { return this->data_; };
		const auto& vertex_data() const noexcept { return this->data_; };
		
		auto vertex_size() const noexcept { return this->vertex_size_; };

	public:

		virtual void draw();
		virtual void refresh() {};

		virtual void handle_event(Event& _event) {};



		bool can_insert(const VertexData& _verts) const noexcept;
		void insert(GFXObject* _obj, const VertexData& _verts);
		void remove(GFXObject* _obj);
		
		auto find(GFXObject* _obj)
		{
			return std::find(this->art_.begin(), this->art_.end(), _obj);
		}; 
		auto find(GFXObject* _obj) const
		{
			return std::find(this->art_.begin(), this->art_.end(), _obj);
		};

		bool contains(GFXObject* _obj) const;

		SpecID spec() const noexcept { return this->spec_; };



		Artist(const SpecID _spec, const VertexData::size_type _vertexSize) :
			spec_{ _spec }, data_{ _spec }, vertex_size_{ _vertexSize }
		{};
		virtual ~Artist() = default;

	private:
		const SpecID spec_ = 0;
		const VertexData::size_type vertex_size_ = 0;

		bool needs_refresh_ = false;

		VertexData data_;
		std::vector<Art> art_{};

	};

}

#endif