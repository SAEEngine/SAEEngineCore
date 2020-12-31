#include "SAEEngineCore_FileHandling.h"

#include <cassert>
#include <filesystem>
#include <fstream>
#include <unordered_map>

namespace sae::engine::core {
namespace {
// The map for string to FileType::FILE_TYPE_E conversions
const static inline std::unordered_map<std::string, FILE_TYPE::FILE_TYPE_E>
    FileType_STRING_TO_ENUM{{".png", FILE_TYPE::FILE_TYPE_E::F_PNG},
                            {".txt", FILE_TYPE::FILE_TYPE_E::F_TXT}};

// The map for FileType::FILE_TYPE_E to string conversions
const static inline std::unordered_map<FILE_TYPE::FILE_TYPE_E, std::string>
    FileType_ENUM_TO_STRING{{FILE_TYPE::FILE_TYPE_E::F_PNG, ".png"},
                            {FILE_TYPE::FILE_TYPE_E::F_TXT, ".txt"}

    };
}; // namespace

FILE_TYPE::FILE_TYPE_E FILE_TYPE::str_to_enum(const std::string &_str,
                                              no_abort_t) {
  auto _at = FileType_STRING_TO_ENUM.find(_str);
  if (_at != FileType_STRING_TO_ENUM.end()) {
    // Return the associated enum
    return _at->second;
  } else {
    return FILE_TYPE_E::BAD_VALUE;
  };
};
FILE_TYPE::FILE_TYPE_E FILE_TYPE::str_to_enum(const std::string &_str) {
  // Call the no_abort overload then handle an error
  auto _out = str_to_enum(_str, no_abort);
  if (_out != FILE_TYPE::FILE_TYPE_E::BAD_VALUE) {
    // Return the associated enum
    return _out;
  } else {
    return _out; // return BAD_VALUE otherwise
  };
};

std::optional<const char *> FILE_TYPE::enum_to_str(
    FILE_TYPE_E _e) // if the filetype is BAD_VALUE, this is a hard error.
{
  // Check that the FILE_TYPE_E value is valid
  if (_e != FILE_TYPE_E::BAD_VALUE) {
    assert(FileType_ENUM_TO_STRING.count(_e) !=
           0); // sanity check asserting that the enum conversion exists
    return FileType_ENUM_TO_STRING.at(_e)
        .c_str(); // return the associated string conversion
  } else {
#ifdef SAE_ENGINE_CORE_USE_EXCPETIONS
    throw std::invalid_argument{"Cannot converted FILE_TYPE::BAD_VALUE to a "
                                "file extension string"}; // throw exception if
                                                          // they are enabled
#endif
    return std::nullopt; // return nullopt otherwise
  };
};

std::optional<const char *> FILE_TYPE::to_cstring() const {
  return this->enum_to_str(this->type_);
};

FILE_TYPE &FILE_TYPE::operator=(FILE_TYPE_E _ft) noexcept {
  this->type_ = _ft;
  return *this;
};

FILE_TYPE::FILE_TYPE(const std::string &_str)
    : type_{FILE_TYPE::str_to_enum(_str)} {};
FILE_TYPE::FILE_TYPE(const std::string &_str, no_abort_t)
    : type_{str_to_enum(_str, no_abort)} {};

FILE_TYPE &FILE_TYPE::operator=(const std::string &_str) {
  this->type_ = FILE_TYPE::str_to_enum(_str);
  return *this;
};

} // namespace sae::engine::core

namespace sae::engine::core {

/**
 * @brief takes in a path and returns the data in that file
 * @brief returns an optional if there was an error opening the file
 *
 * @param _filename
 * @return std::optional<std::vector<unsigned char>>
 */
std::optional<std::vector<unsigned char>>
OpenFile(std::filesystem::path _filename) {

  std::ifstream _file(_filename.native());

  if (_file.is_open()) {

    std::vector<unsigned char> _out{};
    unsigned char _readbuffer[512]{};
    while (!_file.eof()) {

      _file.read((char *)_readbuffer, sizeof(_readbuffer));
      _out.insert(_out.end(), _readbuffer, _readbuffer + _file.gcount());
    }
    return _out;
  }

  return std::nullopt;
}

/**
 * @brief takes in a path and returns extension of that file
 * @brief returns an optional if the file extension was empty
 *
 * @param _filename
 * @return std::optional<std::string>
 */
std::optional<std::string> GetFileType(std::filesystem::path _filename) {

  if (_filename.extension() != "") {
    return _filename.extension();
  }

  return std::nullopt;
}

FileIO::FileIO(const char *_path) : path_(_path) {}

void FileIO::save_text_in_file(std::string _data, std::string _filename) {
  std::fstream s(_filename);
  if (!s.is_open()) {
    core::lout << "file: " << _filename << " could not be found\n";
    return;
  }
  s << _data;
}

void FileIO::create_file(std::string _filename) {
  std::fstream s(_filename);
  s.open(_filename);
}

} // namespace sae::engine::core
