#pragma once

#include <stdexcept>
#include <unordered_map>
#include <fstream>

/**
 * @brief Class for parsing config file.
 */
class Config {
  private:

  std::unordered_map<std::string, std::string> fields;

  /**
   * @brief Trims leading whitespaces.
   * 
   * @param input string 
   */
  static void leftTrim(std::string &str) {
    str.erase(
      str.begin(), 
      std::find_if(
        str.begin(), 
        str.end(), 
        [](unsigned char ch) { return !std::isspace(ch); }
      )
    );
  }

  /**
   * @brief Trims following whitespaces.
   * 
   * @param input string 
   */
  static void rightTrim(std::string &str) {
    str.erase(
      std::find_if(
        str.rbegin(), 
        str.rend(), 
        [](unsigned char ch) { return !std::isspace(ch); }
      ).base(),
      str.end()
    );
  }

  /**
   * @brief Trims whitespaces from both ends.
   * 
   * @param input string 
   */
  static void trim(std::string &str) {
    leftTrim(str);
    rightTrim(str);
  }

  /**
   * @brief Trims comments.
   * 
   * @param input string 
   */
  static void trimComment(std::string &str) {
    std::size_t commentPosition = str.find('#');
    if(commentPosition == std::string::npos) return;
    str.erase(commentPosition, std::string::npos);
  }

  public:

  /**
   * @brief Constructs a new Config object and loads key/value pairs.
   * 
   * @param config file path 
   */
  Config(const char *path) {
    std::ifstream file(path);
    if(file.is_open()) {
      std::string line;
      while(std::getline(file, line)) {
        trimComment(line);
        
        std::size_t keyValueSeparator = line.find('=');
        if(keyValueSeparator == std::string::npos) continue;

        std::string key = line.substr(0, keyValueSeparator);
        std::string value = line.substr(keyValueSeparator + 1);

        trim(key);
        trim(value);

        fields.insert({ key, value });
      }

      file.close();
    } else {
      throw std::runtime_error("Could not open config file");
    }
  }

  /**
   * @brief Gets the value at the given key.
   * 
   * @param key 
   * @return value
   */
  std::string getValue(std::string key) {
    auto pair = fields.find(key);
    if(pair != fields.end()) {
      return pair->second;
    } else throw std::runtime_error("Could not get field with given key");
  }
};