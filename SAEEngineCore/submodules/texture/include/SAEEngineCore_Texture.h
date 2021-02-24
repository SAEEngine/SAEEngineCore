#pragma once
#ifndef SAE_ENGINE_CORE_TEXTURE_H
#define SAE_ENGINE_CORE_TEXTURE_H

#include <cstdint>
#include <memory>
#include <cassert>
#include <initializer_list>

namespace sae::engine::core
{

	template <typename PixelT, typename AllocT = std::allocator<PixelT>>
	class Texture
	{
	public:
		using value_type = PixelT;
		using pointer = value_type*;
		using reference = value_type&;
		using const_pointer = value_type*;
		using const_reference = value_type&;

		using size_type = size_t;
		using allocator_type = AllocT;

		constexpr size_type width() const noexcept { return this->width_; };
		constexpr size_type height() const noexcept { return this->height_; };
		constexpr size_type size() const noexcept { return this->width() * this->height(); };

		constexpr pointer data() noexcept { return this->data_; };
		constexpr const_pointer data() const noexcept { return this->data_; };

	private:
		void deallocate_data()
		{
			this->alloc_.destroy(this->data());
			this->alloc_.deallocate(this->data(), this->size());
			this->data_ = nullptr;
		};
		pointer allocate_size(size_type _count) noexcept(noexcept(std::declval<allocator_type>().allocate(std::declval<size_type>())))
		{
			this->alloc_.allocate(_count);
		};
		void resize_data(size_type _count)
		{
			auto _newData = this->allocate_size(_count);
			std::move(this->data(), this->data() + this->size(), _newData);
			this->deallocate_data();
			this->data_ = _newData;
		};

	public:
		void resize(size_type _width, size_type _height) noexcept(noexcept(resize_data(std::declval<size_type>())))
		{
			this->resize_data(_width * _height);
			this->width_ = _width;
			this->height_ = _height;
		};

		constexpr reference at(size_type _n) noexcept
		{
			assert(_n < this->size());
			return *(this->data() + _n);
		};
		constexpr const_reference at(size_type _n) const noexcept
		{
			assert(_n < this->size());
			return *(this->data() + _n);
		};

		constexpr reference operator[](size_type _n) noexcept
		{
			return this->at(_n);
		};
		constexpr const_reference operator[](size_type _n) const noexcept
		{
			return this->at(_n);
		};

		constexpr reference at(size_type _x, size_type _y) noexcept
		{
			return this->at((_x * this->height()) + _y);
		};
		constexpr const_reference at(size_type _x, size_type _y) const noexcept
		{
			return this->at((_x * this->height()) + _y);
		};

		constexpr reference front() noexcept { return this->at(0); };
		constexpr const_reference front() const noexcept { return this->at(0); };

		constexpr reference back() noexcept { return this->at((size_type)(this->size() - 1)); };
		constexpr const_reference back() const noexcept { return this->at((size_type)(this->size() - 1)); };

		constexpr Texture() noexcept = default;
		explicit Texture(size_type _width, size_type _height) noexcept(noexcept(resize(std::declval<size_type>(), std::declval<size_type>())))
		{
			this->resize(_width, _height);
			assert(this->width() == _width);
			assert(this->height() == _height);
			assert(this->size() == (_width * _height));
			assert(this->size() == this->width() * this->height());
		};

		template <typename IterT>
		Texture(size_type _width, size_type _height, IterT _begin, IterT _end) :
			Texture{ _width, _height }
		{
			std::copy(_begin, _end, this->data());
		};
		Texture(size_type _width, size_type _height, const_pointer _data) :
			Texture{ _width, _height, _data, _data + (_width * _height) }
		{};
		Texture(size_type _width, size_type _height, std::initializer_list<value_type> _pixels) :
			Texture{ _width, _height, _pixels.begin(), _pixels.end() }
		{};

		Texture(const Texture& other) :
			width_{ other.width() }, height_{ other.height() },
			alloc_{ other.alloc_ }, data_{ this->alloc_.allocate(other.size()) }
		{
			std::copy(other.data(), other.data() + other.size(), this->data());
		};

		Texture& operator=(const Texture& other)
		{
			this->deallocate_data();
			this->width_ = other.width();
			this->height_ = other.height();
			this->alloc_ = other.alloc_;
			this->data_ = this->allocate_size(this->size());
			std::copy(other.data(), other.data() + other.size(), this->data());
			return *this;
		};

		constexpr Texture(Texture&& other) noexcept :
			width_{ other.width() }, height_{ other.height() },
			alloc_{ std::move(other.alloc_) }, data_{ std::exchange(other.data_, nullptr) }
		{};
		Texture& operator=(Texture&& other) noexcept
		{
			this->deallocate_data();
			this->width_ = other.width();
			this->height_ = other.height();
			this->alloc_ = std::move(other.alloc_);
			this->data_ = std::exchange(other.data_, nullptr);
			return *this;
		};

		~Texture()
		{
			this->deallocate_data();
		};

	private:
		allocator_type alloc_{};
		size_type width_ = 0;
		size_type height_ = 0;
		pointer data_ = nullptr;

	};

}

#endif