#pragma once
#ifndef SAE_ENGINE_CORE_GL_OBJECT_H
#define SAE_ENGINE_CORE_GL_OBJECT_H

#include <SAEEngineCore_Environment.h>
#include <SAEEngineCore_Object.h>

#include <concepts>
#include <memory>
#include <optional>
#include <unordered_map>

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

			template <> 
			struct gl_type<float_t>
			{ 
				constexpr static inline TYPE _type = TYPE::FLOAT; 
			};
			template <> 
			struct gl_type<double_t> 
			{
				constexpr static inline TYPE _type = TYPE::DOUBLE; 
			};

			template <> 
			struct gl_type<int32_t> 
			{ 
				constexpr static inline TYPE type = TYPE::INT; 
			};
			template <> 
			struct gl_type<int16_t> 
			{ 
				constexpr static inline TYPE type = TYPE::SHORT; 
			};
			template <> 
			struct gl_type<int8_t> 
			{ 
				constexpr static inline TYPE type = TYPE::BYTE; 
			};

			template <> 
			struct gl_type<uint32_t> 
			{
				constexpr static inline TYPE type = TYPE::UNSIGNED; 
			};
			template <> 
			struct gl_type<uint16_t> 
			{ 
				constexpr static inline TYPE type = TYPE::UNSIGNED_SHORT; 
			};
			template <> 
			struct gl_type<uint8_t>
			{ 
				constexpr static inline TYPE type = TYPE::UNSIGNED_BYTE; 
			};

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

	/**
	 * @brief Holds an integer quantity of bytes. This exists to try to prevent errors when using a size in bytes or a size in elements
	*/
	struct Bytes
	{
	public:
		using value_type = std::size_t;
		constexpr value_type count() const noexcept { return this->count_; };
		constexpr auto operator<=>(const Bytes&) const noexcept = default;
		
		constexpr Bytes operator+(const Bytes& _rhs) const noexcept 
		{
			return Bytes{ this->count() + _rhs.count() };
		};
		Bytes& operator+=(const Bytes& _rhs) noexcept
		{
			this->count_ += _rhs.count();
			return *this;
		};
				
		constexpr Bytes operator-(const Bytes& _rhs) const noexcept
		{
			return Bytes{ this->count() - _rhs.count() };
		};
		Bytes& operator-=(const Bytes& _rhs) noexcept
		{
			this->count_ -= _rhs.count();
			return *this;
		};
		
		constexpr Bytes operator*(const Bytes& _rhs) const noexcept
		{
			return Bytes{ this->count() * _rhs.count() };
		};
		constexpr Bytes operator/(const Bytes& _rhs) const noexcept
		{
			return Bytes{ this->count() / _rhs.count() };
		};


		constexpr explicit Bytes(value_type _count) noexcept :
			count_{ _count } 
		{};

	private:
		value_type count_ = 0;
	};

	/**
	 * @brief Wrapping type for an opengl Vertex Buffer Object
	*/
	template <GLenum BUFFER_TARGET>
	class VBO
	{
	public:

		GLuint id() const noexcept { return this->id_; };

		bool good() const noexcept
		{
			return this->id() != 0;
		};
		void init() noexcept
		{
			glGenBuffers(1, &this->id_);
		};

		void bind(GLenum _target) const noexcept
		{
			glBindBuffer(_target, this->id());
		};
		void bind() const noexcept
		{
			this->bind(BUFFER_TARGET);
		};
		
		void unbind(GLenum _target) const noexcept
		{
			glBindBuffer(_target, 0);
		};
		void unbind() const noexcept
		{
			this->unbind(BUFFER_TARGET);
		};

		void destroy()
		{
			glDeleteBuffers(1, &this->id_);
			this->id_ = 0;
		};

		Bytes size() const noexcept
		{
			return this->size_;
		};
		Bytes capacity() const noexcept
		{
			return this->capacity_;
		};

		void reserve(Bytes _bytes)
		{
			if (this->capacity() > Bytes{ 0 })
			{
				GLuint _cbuff = 0;

				glGenBuffers(1, &_cbuff);

				glBindBuffer(GL_COPY_WRITE_BUFFER, _cbuff);
				glBufferData(GL_COPY_WRITE_BUFFER, (GLsizeiptr)_bytes.count(), NULL, GL_STATIC_DRAW);

				glBindBuffer(GL_COPY_READ_BUFFER, this->id());

				glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, std::min(_bytes, this->capacity()).count());

				glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
				glBindBuffer(GL_COPY_READ_BUFFER, 0);

				glDeleteBuffers(1, &this->id_);
				this->id_ = _cbuff;
			}
			else
			{
				this->bind();
				glBufferData(BUFFER_TARGET, _bytes.count(), NULL, GL_STATIC_DRAW);
				this->unbind();
			};

			if (_bytes < this->size())
			{
				this->size_ = _bytes;
			};
			this->capacity_ = _bytes;

		};
		void resize(Bytes _bytes)
		{
			if (_bytes > this->capacity())
			{
				this->reserve(_bytes);
			};
			this->size_ = _bytes;
		};

		/**
		 * @brief Writes _bytes length of data from _dataIn into the buffer, starting at _offset
		 * @param _offset Offset from this buffer start in bytes
		 * @param _dataIn Pointer to data to copy from
		 * @param _bytes Length of data to copy in bytes
		 * @param _target GL Buffer target, defaults to the template parameter
		*/
		void overwrite(Bytes _offset, const void* _dataIn, Bytes _bytes, GLenum _target = BUFFER_TARGET)
		{
			if (_offset + _bytes > this->capacity())
			{
				this->resize(_offset + _bytes);
			};
			this->bind(_target);
			glBufferSubData(_target, _offset.count(), _bytes.count(), _dataIn);
			this->unbind(_target);
		};

		/**
		 * @brief Assigns _bytes length of data from _data to the buffer
		 * @param _data Pointer to data to copy from
		 * @param _bytes Length of data to copy in bytes
		 * @param _target GL Buffer target, defaults to the template parameter
		*/
		void assign(const void* _data, Bytes _bytes, GLenum _target = BUFFER_TARGET)
		{
			this->overwrite(Bytes{ 0 }, _data, _bytes, _target);
		};

		/**
		 * @brief Removes _bytes number of bytes from the buffer after the offset _offset. This is done by overwriting the old values
		 * @param _offset Offset from start of buffer in bytes
		 * @param _bytes Length to erase in bytes
		*/
		void erase(Bytes _offset, Bytes _bytes)
		{
			this->bind(GL_COPY_READ_BUFFER);
			this->bind(GL_COPY_WRITE_BUFFER);

			auto _rangeEnd = _offset + _bytes;
			assert(_rangeEnd < this->size());
			glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, _rangeEnd.count(), _offset.count(), (this->size() - _rangeEnd).count());
			this->size_ = this->size() - _bytes;

			this->unbind(GL_COPY_READ_BUFFER);
			this->unbind(GL_COPY_WRITE_BUFFER);
		};

		void append(const void* _dataIn, Bytes _bytes, GLenum _target = BUFFER_TARGET)
		{
			this->overwrite(this->size(), _dataIn, _bytes, _target);
		};

		VBO() :
			id_{ 0 }
		{
			this->init();
		};

		VBO(const VBO& other) = delete;
		VBO& operator=(const VBO& other) = delete;

		VBO(VBO&& other) noexcept :
			id_{ std::exchange(other.id_, 0) },
			size_{ other.size_ }, capacity_{ other.capacity_ }
		{};
		VBO& operator=(VBO&& other) noexcept
		{
			this->destroy();
			this->id_ = std::exchange(other.id_, 0);
			this->size_ = other.size();
			this->capacity_ = other.capacity();
			return *this;
		};

		~VBO()
		{
			this->destroy();
		};

	private:
		GLuint id_ = 0;
		Bytes size_{ 0 };
		Bytes capacity_{ 0 };
	};

	/**
	 * WARNING : type is soon to be removed
	 * @brief Wrapping type for an opengl Vertex Buffer Object that mainly uses GL_ARRAY_BUFFER, best used with a single data type
	 * @tparam T 
	*/
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
			this->bind();
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

	/**
	 * @brief Wrapper for an opengl Vertex Array Object
	*/
	class VAO
	{
	public:
		const GLuint& id() const noexcept { return this->id_; };
		
		bool good() const noexcept { return this->id() != 0; };

		void init() 
		{
			glGenVertexArrays(1, &this->id_);
			this->bind();
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


	




	

	template <typename T>
	class Artist
	{
	public:
		using art_type = T;

		using value_type = art_type;
		using pointer = value_type*;

		using size_type = uint32_t;

	public:
		void insert(pointer _ptr)
		{

		};
		void remove(pointer _ptr)
		{

		};

		Artist();

	private:
		ShaderProgram* shader_ = nullptr;
		VAO vao_;
		uint32_t vertice_count_ = 0;
		uint32_t art_count_ = 0;
		std::unordered_map<uint32_t, Bytes> offset_map_{};

	};



	class glDrawable
	{
	public:
		bool good() const;
		bool initialize();
		void refresh();
		void destroy();

	};

	struct glWidget
	{
		ColorRGBA_8 color_;

	};

	struct glObject
	{

	};







	class glWindow : public GFXWindow
	{
	public:
		bool good() const noexcept { return this->keep_running_; };
		explicit operator bool() const noexcept { return this->good(); };

		void draw() override
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			GFXWindow::draw();

			glfwPollEvents();
		};

		HANDLE_EVENT_RETURN handle_event(const Event& _event) override
		{
			using EVENT = Event::EVENT_TYPE;
			auto _out = GFXWindow::handle_event(_event);
			if (_out == HANDLE_EVENT_RETURN::IGNORED)
			{
				switch (_event.index())
				{
				case EVENT::WINDOW_CLOSE:
					this->keep_running_ = false;
					_out = HANDLED;
					break;
				default:
					break;
				};
			};
			return _out;
		};

		using GFXWindow::GFXWindow;

	private:
		bool keep_running_ = true;
	};







}

#endif