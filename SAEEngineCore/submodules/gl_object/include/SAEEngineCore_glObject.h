#pragma once
#ifndef SAE_ENGINE_CORE_GL_OBJECT_H
#define SAE_ENGINE_CORE_GL_OBJECT_H

#include <SAEEngineCore_Environment.h>
#include <SAEEngineCore_Object.h>

#include <concepts>
#include <memory>
#include <optional>
#include <unordered_map>

namespace sae::engine::core {

template <typename T> struct UBind {
  const T *buff;

  UBind(const T *_buff) : buff{_buff} { this->buff->bind(); };
  UBind(const UBind &other) = delete;
  UBind &operator=(const UBind &other) = delete;

  UBind(UBind &&other) = delete;
  UBind &operator=(UBind &&other) = delete;

  ~UBind() { this->buff->unbind(); };
};

namespace gl {
enum class BUFFER_MODE : GLenum {
  STATIC_DRAW = GL_STATIC_DRAW,
  STREAM_DRAW = GL_STREAM_DRAW,
  DYNAMIC_DRAW = GL_DYNAMIC_DRAW
};

enum class TYPE : GLenum {
  FLOAT = GL_FLOAT,
  DOUBLE = GL_DOUBLE,

  INT = GL_INT,
  SHORT = GL_SHORT,
  BYTE = GL_BYTE,

  UNSIGNED = GL_UNSIGNED_INT,
  UNSIGNED_SHORT = GL_UNSIGNED_SHORT,
  UNSIGNED_BYTE = GL_UNSIGNED_BYTE

};

namespace impl {

template <typename T> struct gl_type;

template <> struct gl_type<float_t> {
  constexpr static inline TYPE _type = TYPE::FLOAT;
};
template <> struct gl_type<double_t> {
  constexpr static inline TYPE _type = TYPE::DOUBLE;
};

template <> struct gl_type<int32_t> {
  constexpr static inline TYPE type = TYPE::INT;
};
template <> struct gl_type<int16_t> {
  constexpr static inline TYPE type = TYPE::SHORT;
};
template <> struct gl_type<int8_t> {
  constexpr static inline TYPE type = TYPE::BYTE;
};

template <> struct gl_type<uint32_t> {
  constexpr static inline TYPE type = TYPE::UNSIGNED;
};
template <> struct gl_type<uint16_t> {
  constexpr static inline TYPE type = TYPE::UNSIGNED_SHORT;
};
template <> struct gl_type<uint8_t> {
  constexpr static inline TYPE type = TYPE::UNSIGNED_BYTE;
};

}; // namespace impl

template <typename T> concept cx_has_gl_type = requires(T a) {
  impl::gl_type<std::remove_const_t<T>>{};
};

template <cx_has_gl_type T>
struct gl_type : public impl::gl_type<std::remove_const_t<T>> {};

}; // namespace gl

struct deffered_init_t {};
constexpr static inline deffered_init_t deffered_init{};

/**
 * @brief Holds an integer quantity of bytes. This exists to try to prevent
 * errors when using a size in bytes or a size in elements
 */
struct Bytes {
public:
  using value_type = std::size_t;
  constexpr value_type count() const noexcept { return this->count_; };

  constexpr auto operator<=>(const Bytes &) const noexcept = default;

  constexpr Bytes operator+(const Bytes &_rhs) const noexcept {
    return Bytes{this->count() + _rhs.count()};
  };
  constexpr Bytes operator+(const value_type &_rhs) const noexcept {
    return Bytes{this->count() + _rhs};
  };
  Bytes &operator+=(const Bytes &_rhs) noexcept {
    this->count_ += _rhs.count();
    return *this;
  };
  Bytes &operator+=(const value_type &_rhs) noexcept {
    this->count_ += _rhs;
    return *this;
  };

  constexpr Bytes operator-(const Bytes &_rhs) const noexcept {
    return Bytes{this->count() - _rhs.count()};
  };
  constexpr Bytes operator-(const value_type &_rhs) const noexcept {
    return Bytes{(value_type)(this->count() - _rhs)};
  };
  Bytes &operator-=(const Bytes &_rhs) noexcept {
    this->count_ -= _rhs.count();
    return *this;
  };
  Bytes &operator-=(const value_type &_rhs) noexcept {
    this->count_ -= _rhs;
    return *this;
  };

  constexpr Bytes operator*(const Bytes &_rhs) const noexcept {
    return Bytes{this->count() * _rhs.count()};
  };
  constexpr Bytes operator*(const value_type &_rhs) const noexcept {
    return Bytes{(value_type)(this->count() * _rhs)};
  };

  constexpr Bytes operator/(const Bytes &_rhs) const noexcept {
    return Bytes{this->count() / _rhs.count()};
  };
  constexpr Bytes operator/(const value_type &_rhs) const noexcept {
    return Bytes{(value_type)(this->count() / _rhs)};
  };

  constexpr explicit Bytes(value_type _count) noexcept : count_{_count} {};
  Bytes &operator=(value_type _count) noexcept {
    this->count_ = _count;
    return *this;
  };

private:
  value_type count_ = 0;
};

/**
 * @brief Wrapping type for an opengl Vertex Buffer Object
 */
template <GLenum BUFFER_TARGET> class VBO {
public:
  GLuint id() const noexcept { return this->id_; };

  bool good() const noexcept { return this->id() != 0; };
  void init() noexcept { glGenBuffers(1, &this->id_); };

  void bind(GLenum _target) const noexcept {
    glBindBuffer(_target, this->id());
  };
  void bind() const noexcept { this->bind(BUFFER_TARGET); };

  void unbind(GLenum _target) const noexcept { glBindBuffer(_target, 0); };
  void unbind() const noexcept { this->unbind(BUFFER_TARGET); };

  void destroy() {
    glDeleteBuffers(1, &this->id_);
    this->id_ = 0;
  };

  Bytes size() const noexcept { return this->size_; };
  Bytes capacity() const noexcept { return this->capacity_; };

  void reserve(Bytes _bytes) {
    if (this->capacity() > Bytes{0}) {
      GLuint _cbuff = 0;

      glGenBuffers(1, &_cbuff);

      glBindBuffer(GL_COPY_WRITE_BUFFER, _cbuff);
      glBufferData(GL_COPY_WRITE_BUFFER, (GLsizeiptr)_bytes.count(), NULL,
                   GL_STATIC_DRAW);

      glBindBuffer(GL_COPY_READ_BUFFER, this->id());

      glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0,
                          std::min(_bytes, this->capacity()).count());

      glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
      glBindBuffer(GL_COPY_READ_BUFFER, 0);

      glDeleteBuffers(1, &this->id_);
      this->id_ = _cbuff;
    } else {
      this->bind();
      glBufferData(BUFFER_TARGET, _bytes.count(), NULL, GL_STATIC_DRAW);
    };

    if (_bytes < this->size()) {
      this->size_ = _bytes;
    };
    this->capacity_ = _bytes;
  };
  void resize(Bytes _bytes) {
    if (_bytes > this->capacity()) {
      this->reserve(_bytes);
    };
    this->size_ = _bytes;
  };

  /**
   * @brief Writes _bytes length of data from _dataIn into the buffer, starting
   * at _offset
   * @param _offset Offset from this buffer start in bytes
   * @param _dataIn Pointer to data to copy from
   * @param _bytes Length of data to copy in bytes
   * @param _target GL Buffer target, defaults to the template parameter
   */
  void overwrite(Bytes _offset, const void *_dataIn, Bytes _bytes,
                 GLenum _target = BUFFER_TARGET) {
    if (_offset + _bytes > this->capacity()) {
      this->resize(_offset + _bytes);
    };
    this->bind(_target);
    glBufferSubData(_target, _offset.count(), _bytes.count(), _dataIn);
    if (_offset + _bytes > this->size()) {
      this->size_ = _offset + _bytes;
    };
  };

  /**
   * @brief Assigns _bytes length of data from _data to the buffer
   * @param _data Pointer to data to copy from
   * @param _bytes Length of data to copy in bytes
   * @param _target GL Buffer target, defaults to the template parameter
   */
  void assign(const void *_data, Bytes _bytes, GLenum _target = BUFFER_TARGET) {
    this->overwrite(Bytes{0}, _data, _bytes, _target);
  };

  /**
   * @brief Removes _bytes number of bytes from the buffer after the offset
   * _offset. This is done by overwriting the old values
   * @param _offset Offset from start of buffer in bytes
   * @param _bytes Length to erase in bytes
   */
  void erase(Bytes _offset, Bytes _bytes) {
    auto _rangeEnd = _offset + _bytes;
    assert(_rangeEnd <= this->size());

    GLuint _bid = 0;
    glGenBuffers(1, &_bid);
    glBindBuffer(GL_COPY_WRITE_BUFFER, _bid);
    glBufferData(GL_COPY_WRITE_BUFFER, this->size().count(), NULL,
                 GL_STATIC_DRAW);
    this->bind(GL_COPY_READ_BUFFER);

    glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0,
                        this->size().count());
    glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER,
                        _rangeEnd.count(), _offset.count(),
                        (this->size() - _rangeEnd).count());
    this->size_ = this->size() - _bytes;

    glBindBuffer(GL_COPY_READ_BUFFER, _bid);
    this->bind(GL_COPY_WRITE_BUFFER);
    glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0,
                        this->size().count());

    glDeleteBuffers(1, &_bid);

    this->unbind(GL_COPY_WRITE_BUFFER);
  };

  void append(const void *_dataIn, Bytes _bytes,
              GLenum _target = BUFFER_TARGET) {
    this->overwrite(this->size(), _dataIn, _bytes, _target);
  };

  explicit VBO(Bytes _reserveCount) : id_{0} {
    this->init();
    this->reserve(_reserveCount);
  };

  VBO() : VBO{Bytes{0}} {};

  VBO(const VBO &other) = delete;
  VBO &operator=(const VBO &other) = delete;

  VBO(VBO &&other)
  noexcept
      : id_{std::exchange(other.id_, 0)}, size_{other.size_},
        capacity_{other.capacity_} {};
  VBO &operator=(VBO &&other) noexcept {
    this->destroy();
    this->id_ = std::exchange(other.id_, 0);
    this->size_ = other.size();
    this->capacity_ = other.capacity();
    return *this;
  };

  ~VBO() { this->destroy(); };

private:
  GLuint id_ = 0;
  Bytes size_{0};
  Bytes capacity_{0};
};

/**
 * WARNING : type is soon to be removed
 * @brief Wrapping type for an opengl Vertex Buffer Object that mainly uses
 * GL_ARRAY_BUFFER, best used with a single data type
 * @tparam T
 */
template <typename T> class glArrayBuffer {
public:
  using value_type = T;
  using pointer = value_type *;
  using reference = value_type &;
  using const_pointer = const value_type *;
  using const_reference = const value_type &;
  using size_type = std::size_t;

  GLuint id() const noexcept { return this->id_; };

  bool good() const noexcept { return this->id() != 0; };

  void init() noexcept {
    glGenBuffers(1, &this->id_);
    this->bind();
  };
  void bind() const noexcept { glBindBuffer(GL_ARRAY_BUFFER, this->id()); };
  void unbind() const noexcept { glBindBuffer(GL_ARRAY_BUFFER, 0); };
  void destroy() {
    glDeleteBuffers(1, &this->id_);
    this->id_ = 0;
  };

  size_type size() const noexcept { return this->size_; };
  size_type capacity() const noexcept { return this->capacity_; };

  void reserve(size_type _count) {
    GLuint _cbuff = 0;

    glGenBuffers(1, &_cbuff);

    glBindBuffer(GL_COPY_WRITE_BUFFER, _cbuff);
    glBufferData(GL_COPY_WRITE_BUFFER, _count * sizeof(value_type), NULL,
                 GL_STATIC_DRAW);

    glBindBuffer(GL_COPY_READ_BUFFER, this->id());

    glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0,
                        std::min(_count * sizeof(value_type),
                                 this->capacity() * sizeof(value_type)));

    glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
    glBindBuffer(GL_COPY_READ_BUFFER, 0);

    glDeleteBuffers(1, &this->id_);

    this->id_ = _cbuff;
    if (_count < this->size()) {
      this->size_ = _count;
    };
    this->capacity_ = _count;
  };
  void resize(size_type _count) {
    if (_count > this->capacity()) {
      this->reserve(_count);
    };
    this->size_ = _count;
  };

  void overwrite(size_type _offset, const_pointer _dataIn, size_type _count) {
    this->overwrite(_offset, _dataIn, _dataIn + _count);
  };

  template <typename _RndIter>
  void overwrite(size_type _offset, _RndIter _begin, _RndIter _end) {
    auto _len = _end - _begin;
    if (_len + _offset > this->size()) {
      this->resize(_len + _offset);
    };
    this->bind();
    glBufferSubData(GL_ARRAY_BUFFER, _offset * sizeof(value_type),
                    _len * sizeof(value_type), &_begin[0]);
    this->unbind();
  };

  void assign(const_pointer _data, size_type _count) {
    this->overwrite(0, _data, _count);
  };

  template <typename _RndIter> void assign(_RndIter _begin, _RndIter _end) {
    this->overwrite(0, _begin, _end);
  };

  glArrayBuffer(deffered_init_t){};

  glArrayBuffer(const_pointer _data, size_type _count) {
    this->init();
    this->assign(_data, _count);
  };
  glArrayBuffer(size_type _capacity) {
    this->init();
    this->reserve(_capacity);
  };

  glArrayBuffer() { this->init(); };

  glArrayBuffer(const glArrayBuffer &other) = delete;
  glArrayBuffer &operator=(const glArrayBuffer &other) = delete;

  glArrayBuffer(glArrayBuffer &&other) noexcept
      : id_{std::exchange(other.id_, 0)}, size_{other.size()},
        capacity_{other.capacity()} {};

  glArrayBuffer &operator=(glArrayBuffer &&other) noexcept {
    this->destroy();
    this->id_ = std::exchange(other.id_, 0);
    this->size_ = other.size();
    this->capacity_ = other.capacity();
    return *this;
  };
  ~glArrayBuffer() { this->destroy(); };

private:
  GLuint id_{};
  size_type size_ = 0;
  size_type capacity_ = 0;
};

/**
 * @brief Wrapper for an opengl Vertex Array Object
 */
class VAO {
public:
  const GLuint &id() const noexcept { return this->id_; };

  bool good() const noexcept { return this->id() != 0; };

  void init() {
    glGenVertexArrays(1, &this->id_);
    this->bind();
  };
  void bind() const { glBindVertexArray(this->id()); };
  void unbind() const { glBindVertexArray(0); };
  void destroy() {
    glDeleteVertexArrays(1, &this->id_);
    this->id_ = 0;
  };

  VAO() { this->init(); };
  VAO(deffered_init_t){};

  VAO(const VAO &other) = delete;
  VAO &operator=(const VAO &other) = delete;

  VAO(VAO &&other) noexcept : id_{std::exchange(other.id_, 0)} {};
  VAO &operator=(VAO &&other) noexcept {
    this->destroy();
    this->id_ = std::exchange(other.id_, 0);
    return *this;
  };

  ~VAO() { this->destroy(); };

private:
  GLuint id_ = 0;
};

#if false

	class glWidgetArtist;

	class glWidget : public UIObject
	{
	public:
		using color_type = ColorRGBA_8;
		using artist_type = glWidgetArtist;

	private:
		artist_type* get_artist() const noexcept
		{
			return this->artist_;
		};

		bool on_initialize();
		void on_destroy();

	public:
		bool initialize() override
		{
			return this->on_initialize();
		};

		void refresh() override;

		void destroy() override
		{
			this->on_destroy();
		};

		color_type get_color() const noexcept 
		{
			return this->color_; 
		};

		void set_color(color_type _col) noexcept 
		{
			this->color_ = _col; 
		};

		glWidget(artist_type* _artist, UIRect _r, color_type _col) : 
			UIObject{ _r }, artist_{ _artist }, color_{ _col }
		{
			assert(_artist);
			auto _res = this->on_initialize();
			assert(_res);
		};
		glWidget(artist_type* _artist, UIRect _r) : 
			glWidget{ _artist, _r, {} }
		{};
		~glWidget()
		{
			this->on_destroy();
		};

	private:
		friend artist_type;
		artist_type* artist_ = nullptr;
		std::optional<uint16_t> art_id_ = std::nullopt;
		color_type color_;

	};

	class glWidgetArtist : public GFXObject
	{
	public:
		using art_type = glWidget;
		using color_type = ColorRGBA_8;
		using shader_type = ShaderProgram;

	private:
		bool on_initialize()
		{
			if (!this->vao_.good())
			{
				this->vao_.init();
			};
			this->vao_.bind();

			if (!this->vbo_.good())
			{
				this->vbo_.init();
			};
			this->vbo_.bind();
		};
		void on_destroy()
		{
			this->vbo_.destroy();
			this->vao_.destroy();
		};

		void write_vertex(uint16_t _vert, float_t _x, float_t _y, float_t _z, color_type _color)
		{
			float_t _pos[3]{ _x, _y, _z };
			this->vbo_.overwrite(Bytes{ (size_t)(_vert * 16) }, _pos, Bytes{ sizeof(_pos) });
			this->vbo_.overwrite(Bytes{ (size_t)((_vert * 16) + 12) }, _color.col, Bytes{ sizeof(_color) });
		};
		void write_art(uint16_t _vertOffset, art_type* _art)
		{
			auto _l = (float_t)_art->bounds().left();
			auto _t = (float_t)_art->bounds().top();
			auto _r = (float_t)_art->bounds().right();
			auto _b = (float_t)_art->bounds().bottom();
			auto _z = (float_t)_art->zlayer();
			auto _col = _art->get_color();

			this->write_vertex(_vertOffset++, _l, _b, _z, _col);
			this->write_vertex(_vertOffset++, _l, _t, _z, _col);
			this->write_vertex(_vertOffset++, _r, _t, _z, _col);

			this->write_vertex(_vertOffset++, _l, _b, _z, _col);
			this->write_vertex(_vertOffset++, _r, _t, _z, _col);
			this->write_vertex(_vertOffset++, _r, _b, _z, _col);
		};

	protected:
		void draw_arrays()
		{
			this->vao_.bind();
			glDrawArrays(GL_TRIANGLES, 0, this->vcount_);
			this->vao_.unbind();
		};

	public:

		shader_type* get_shader() const noexcept { return this->shader_; };
		void set_shader(shader_type* _shader) noexcept { this->shader_ = _shader; };

		bool initialize() override
		{
			return this->on_initialize();
		};

		virtual void insert(art_type* _art)
		{
			assert(_art);
			assert(!_art->art_id_);
			assert(_art->get_artist() == this);

			_art->art_id_ = this->art_count_;
			this->offsets_.insert_or_assign(this->art_count_, this->vcount_);
			this->write_art(this->vcount_, _art);

			this->vcount_ += 6;
			++this->art_count_;
		};

		virtual void remove(art_type* _art)
		{
			assert(_art);
			assert(_art->art_id_);
			assert(_art->get_artist() == this);
			assert(this->offsets_.contains(*_art->art_id_));

			auto _vpos = this->offsets_.at(*_art->art_id_);
			this->vbo_.erase(Bytes{ (size_t)(_vpos * 16) }, Bytes{ 16 });
			for (auto& o : this->offsets_)
			{
				if (o.second > _vpos)
				{
					o.second -= 6;
				};
			};
			this->vcount_ -= 6;
			_art->art_id_ = std::nullopt;
		};

		virtual void refresh(art_type* _art)
		{
			assert(_art);
			assert(_art->art_id_);
			assert(_art->get_artist() == this);
			assert(this->offsets_.contains(*_art->art_id_));
			auto _of = this->offsets_.at(*_art->art_id_);
			this->write_art(_of, _art);

		};

		void draw_self() override
		{
			this->context()->bind_shader(this->get_shader());
			this->draw_arrays();
		};

		void destroy() override
		{
			this->on_destroy();
		};

		glWidgetArtist(ShaderProgram* _shader, Bytes _reserveBytes) :
			shader_{ _shader }
		{
			auto _res = this->on_initialize();
			assert(_res);
			this->vbo_.reserve(_reserveBytes);
		};

		glWidgetArtist(const glWidgetArtist& other) = delete;
		glWidgetArtist& operator=(const glWidgetArtist& other) = delete;

		glWidgetArtist(glWidgetArtist&& other) noexcept = delete;
		glWidgetArtist& operator=(glWidgetArtist&& other) noexcept = delete;

		~glWidgetArtist()
		{
			this->on_destroy();
		};

	private:
		ShaderProgram* shader_ = nullptr;
		VAO vao_;
		VBO<GL_ARRAY_BUFFER> vbo_;
		size_t vcount_ = 0;
		uint16_t art_count_ = 0;
		std::unordered_map<uint16_t, uint16_t> offsets_{};

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

#endif

} // namespace sae::engine::core

#endif