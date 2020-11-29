#pragma once
#ifndef SAE_ENGINE_CORE_GL_OBJECT_H
#define SAE_ENGINE_CORE_GL_OBJECT_H

#include <SAEEngineCore_Environment.h>

#include <concepts>
#include <memory>
#include <optional>

namespace sae::engine::core
{

	template <typename T>
	struct UBind
	{
		const T* buff;

		UBind(const T* _buff) :
			buff{ _buff }
		{
			this->buff->bind();
		};
		UBind(const UBind& other) = delete;
		UBind& operator=(const UBind& other) = delete;

		UBind(UBind&& other) = delete;
		UBind& operator=(UBind&& other) = delete;

		~UBind()
		{
			this->buff->unbind();
		};

	};


	namespace gl
	{
		enum class BUFFER_MODE : GLenum
		{
			STATIC_DRAW = GL_STATIC_DRAW,
			STREAM_DRAW = GL_STREAM_DRAW,
			DYNAMIC_DRAW = GL_DYNAMIC_DRAW
		};

		enum class TYPE : GLenum
		{
			FLOAT = GL_FLOAT,
			DOUBLE = GL_DOUBLE,

			INT = GL_INT,
			SHORT = GL_SHORT,
			BYTE = GL_BYTE,

			UNSIGNED = GL_UNSIGNED_INT,
			UNSIGNED_SHORT = GL_UNSIGNED_SHORT,
			UNSIGNED_BYTE = GL_UNSIGNED_BYTE

		};

		namespace impl
		{

			template <typename T>
			struct gl_type;

			template <> struct gl_type<float_t> { constexpr static inline TYPE _type = TYPE::FLOAT; };
			template <> struct gl_type<double_t> { constexpr static inline TYPE _type = TYPE::DOUBLE; };

			template <> struct gl_type<int32_t> { constexpr static inline TYPE type = TYPE::INT; };
			template <> struct gl_type<int16_t> { constexpr static inline TYPE type = TYPE::SHORT; };
			template <> struct gl_type<int8_t> { constexpr static inline TYPE type = TYPE::BYTE; };

			template <> struct gl_type<uint32_t> { constexpr static inline TYPE type = TYPE::UNSIGNED; };
			template <> struct gl_type<uint16_t> { constexpr static inline TYPE type = TYPE::UNSIGNED_SHORT; };
			template <> struct gl_type<uint8_t> { constexpr static inline TYPE type = TYPE::UNSIGNED_BYTE; };

		};

		template <typename T>
		concept cx_has_gl_type = requires (T a)
		{
			impl::gl_type<std::remove_const_t<T>>{};
		};

		template <cx_has_gl_type T>
		struct gl_type : public impl::gl_type<std::remove_const_t<T>> {};

	};

	struct deffered_init_t {};
	constexpr static inline deffered_init_t deffered_init{};

	template <typename T>
	class glArrayBuffer
	{
	public:
		using value_type = T;
		using pointer = value_type*;
		using reference = value_type&;
		using const_pointer = const value_type*;
		using const_reference = const value_type&;
		using size_type = std::size_t;

		GLuint id() const noexcept 
		{ 
			return this->id_;
		};

		bool good() const noexcept 
		{ 
			return this->id() != 0;
		};
		
		void init() noexcept
		{
			glGenBuffers(1, &this->id_);
		};
		void bind() const noexcept
		{
			glBindBuffer(GL_ARRAY_BUFFER, this->id());
		};
		void unbind() const noexcept
		{
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		};
		void destroy()
		{
			glDeleteBuffers(1, &this->id_);
			this->id_ = 0;
		};

		size_type size() const noexcept
		{
			return this->size_;
		};
		size_type capacity() const noexcept
		{
			return this->capacity_;
		};

		void reserve(size_type _count)
		{
			GLuint _cbuff = 0;
			
			glGenBuffers(1, &_cbuff);
			
			glBindBuffer(GL_COPY_WRITE_BUFFER, _cbuff);
			glBufferData(GL_COPY_WRITE_BUFFER, _count * sizeof(value_type), NULL, GL_STATIC_DRAW);
			
			glBindBuffer(GL_COPY_READ_BUFFER, this->id());
			
			glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, std::min(_count * sizeof(value_type), this->capacity() * sizeof(value_type)));
			
			glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
			glBindBuffer(GL_COPY_READ_BUFFER, 0);
			
			glDeleteBuffers(1, &this->id_);

			this->id_ = _cbuff;
			if (_count < this->size())
			{
				this->size_ = _count;
			};
			this->capacity_ = _count;

		};
		void resize(size_type _count)
		{
			if (_count > this->capacity())
			{
				this->reserve(_count);
			};
			this->size_ = _count;
		};

		void overwrite(size_type _offset, const_pointer _dataIn, size_type _count)
		{
			this->overwrite(_offset, _dataIn, _dataIn + _count);
		};

		template <typename _RndIter>
		void overwrite(size_type _offset, _RndIter _begin, _RndIter _end)
		{
			auto _len = _end - _begin;
			if (_len + _offset > this->size())
			{
				this->resize(_len + _offset);
			};
			this->bind();
			glBufferSubData(GL_ARRAY_BUFFER, _offset * sizeof(value_type), _len * sizeof(value_type), &_begin[0]);
			this->unbind();
		};

		void assign(const_pointer _data, size_type _count)
		{
			this->overwrite(0, _data, _count);
		};

		template <typename _RndIter>
		void assign(_RndIter _begin, _RndIter _end)
		{
			this->overwrite(0, _begin, _end);
		};

		glArrayBuffer(deffered_init_t) {};

		glArrayBuffer(const_pointer _data, size_type _count)
		{
			this->init();
			this->assign(_data, _count);
		};
		glArrayBuffer(size_type _capacity)
		{
			this->init();
			this->reserve(_capacity);
		};

		glArrayBuffer()
		{
			this->init();
		};

		glArrayBuffer(const glArrayBuffer& other) = delete;
		glArrayBuffer& operator=(const glArrayBuffer& other) = delete;

		glArrayBuffer(glArrayBuffer&& other) noexcept : 
			id_{ std::exchange(other.id_, 0) },
			size_{ other.size() }, capacity_{ other.capacity() }
		{};

		glArrayBuffer& operator=(glArrayBuffer&& other) noexcept
		{
			this->destroy();
			this->id_ = std::exchange(other.id_, 0);
			this->size_ = other.size();
			this->capacity_ = other.capacity();
			return *this;
		};
		~glArrayBuffer()
		{
			this->destroy();
		};

	private:
		GLuint id_{};
		size_type size_ = 0;
		size_type capacity_ = 0;

	};

	class VAO
	{
	public:
		const GLuint& id() const noexcept { return this->id_; };
		
		bool good() const noexcept { return this->id() != 0; };

		void init() 
		{
			glGenVertexArrays(1, &this->id_);
		};
		void bind() const
		{
			glBindVertexArray(this->id());
		};
		void unbind() const
		{
			glBindVertexArray(0);
		};
		void destroy()
		{
			glDeleteVertexArrays(1, &this->id_);
			this->id_ = 0;
		};

		VAO()
		{
			this->init();
		};
		VAO(deffered_init_t)
		{};

		VAO(const VAO& other) = delete;
		VAO& operator=(const VAO& other) = delete;

		VAO(VAO&& other) noexcept :
			id_{ std::exchange(other.id_, 0) }
		{};
		VAO& operator=(VAO&& other) noexcept
		{
			this->destroy();
			this->id_ = std::exchange(other.id_, 0);
			return *this;
		};

		~VAO()
		{
			this->destroy();
		};

	private:
		GLuint id_ = 0;
	};











}

#endif