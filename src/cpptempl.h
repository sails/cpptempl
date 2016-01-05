// Copyright (C) 2015 sails Authors.
// All rights reserved.
//
// Official git repository and contact information can be found at
// https://github.com/sails/cpptempl and http://www.sailsxu.com/.
//
// Filename: cpptempl.hpp
// Description: This is a template engine for C++
//
// Author: sailsxu <sailsxu@gmail.com>
// Created: 2015-12-30 10:00:53


#ifndef CPPTEMPL_H_
#define CPPTEMPL_H_

#include <string.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <map>
#include <memory>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

namespace cpptempl {


void SplitString(const std::string& str,
                 const char* delim,
                 std::vector<std::string>* result) {
  char *cstr, *p;
  cstr = new char[str.size()+1];
  snprintf(cstr, str.size()+1, "%s", str.c_str());
  p = strtok(cstr, delim);  // NOLINT'
  while (p != NULL) {
    result->push_back(std::string(p));
    p = strtok(NULL, delim);  // NOLINT'
  }
  delete[] cstr;
}

class auto_data {
 public:
  ///////////////////////////
  // type enumeration //
  ///////////////////////////
  enum class data_type : uint8_t {
    null,
      string,
      boolean,
      number_integer,
      number_float,
      map,
      list
        };

  ///////////////////////////
  // value storage //
  ///////////////////////////
  union data_value {
    std::string* str;
    bool boolean;
    int64_t int_val;
    double f_val;

    data_value() {
    }
    data_value(std::string* v) {
      str = v;
    }
    data_value(bool boolean) : boolean(boolean) {}
    data_value(int64_t v) : int_val(v) {}
    data_value(double v) : f_val(v) {}
    data_value(data_type t) {
      switch (t) {
        case data_type::string: {
          str = NULL;
          break;
        }
        case data_type::boolean: {
          boolean = false;
          break;
        }
        case data_type::number_integer: {
          int_val = 0;
          break;
        }
        case data_type::number_float: {
          f_val = 0.0;
        }
        default:
          break;
      }
    }
  };

 public:
  // 从其它类型构造basic_data
  using string_t = std::string;
  auto_data() {
    type = data_type::null;
  }
  auto_data(const string_t& v)  // NOLINT
      : type(data_type::string) {
    std::string* str = new std::string(v);
    value.str = str;
  }
  auto_data(const char* v)  // NOLINT
      : type(data_type::string) {
    std::string* str = new std::string(v);
    value.str = str;
  }

  auto_data(bool v)  // NOLINT
      : type(data_type::boolean), value(v) {}
  auto_data(int64_t v)  // NOLINT
      : type(data_type::number_integer), value(v) {}
  auto_data(int v)  // NOLINT
      : type(data_type::number_integer), value((int64_t)v) {}
  auto_data(size_t v)  // NOLINT
      : type(data_type::number_integer), value((int64_t)v) {}
  auto_data(double v)  // NOLINT
      : type(data_type::number_float), value(v) {}
  auto_data(const auto_data& data) {
    type = data.type;
    if (data.type == data_type::string) {
      value.str = new std::string(*(data.value.str));
    } else if (data.type == data_type::map) {
      for (auto& item : data.map_data) {
        auto_data d = item.second;
        map_data[item.first] = d;
      }
    } else {
      value = data.value;
    }
  }


  ~auto_data() {
    if (type == data_type::string && value.str != NULL) {
      delete value.str;
      value.str = NULL;
    }
  }

  // map
  bool has(std::string key) const {
    if (type == data_type::map) {
      auto iter = map_data.find(key);
      if (iter != map_data.end()) {
        return true;
      }
    }
    return false;
  }
  // because of [] will insert data for key when not found, so
  // can't defined as auto_data& operator[](const std::stirng& key) const;
  // when param as "const auto_data&", call 'Get' method instead
  // besides can't return const auto_data&, bacause of it will be use
  // data["test"] = "test", this will change the result of reference
  auto_data& operator[](const std::string& key) {
    auto iter = map_data.find(key);
    if (iter == map_data.end()) {  // find
      map_data[key] = auto_data();
    }
    type = data_type::map;
    return map_data[key];
  }
  auto_data& operator[](const char* key) {
    auto iter = map_data.find(key);
    if (iter == map_data.end()) {  // find
      map_data[key] = auto_data();
    }
    type = data_type::map;
    return map_data[key];
  }

  // because of want return auto_data&, so when there is not data
  // for key, can't new one, here throw out_of_range exception
  const auto_data& Get(const std::string& key) const {
    auto iter = map_data.find(key);
    if (iter == map_data.end()) {  // find
      throw new std::out_of_range("out of range is Get method");;
    }
    return map_data.at(key);
  }
  

  // vector
  int size() const {
    return list_data.size();
  }
  auto_data operator[](int index) const {
    return list_data[index];
  }
  void push_back(const auto_data& data) {
    type = data_type::list;
    list_data.push_back(data);
  }

  bool operator ==(const auto_data& data) {
    if (this->type != data.type) {
      return false;
    }
    switch (type) {
      case data_type::string: {
        return *(this->value.str) == *(data.value.str);
      }
      case data_type::boolean: {
        return value.boolean == data.value.boolean;
      }
      case data_type::number_integer: {
        return value.int_val == data.value.int_val;
      }
      case data_type::number_float: {
        return value.f_val == data.value.f_val;
      }
      case data_type::map: {
        return false;
      }
      default:
        return false;
    }
    return false;
  }

  // assignment operator
  void operator =(const auto_data& data) {
    type = data.type;
    if (data.type == data_type::string) {
      value.str = new std::string(data.value.str->c_str());
    } else if (data.type == data_type::map) {
      map_data = data.map_data;
    } else {
      value = data.value;
    }
  }
  operator std::string() const {
    std::string str = "";
    switch (type) {
      case data_type::string: {
        str = std::string((*(value.str)));
        break;
      }
      default:
        break;
    }
    return str;
  }
  operator int() const {
    int64_t v = 0;
    switch (type) {
      case data_type::number_integer: {
        v = value.int_val;
        break;
      }
      default:
        break;
    }
    return v;
  }
  operator int64_t() const {
    int64_t v = 0;
    switch (type) {
      case data_type::number_integer: {
        v = value.int_val;
        break;
      }
      default:
        break;
    }
    return v;
  }
  operator double() const {
    double v = 0.0;
    switch (type) {
      case data_type::number_float: {
        v = value.f_val;
        break;
      }
      default:
        break;
    }
    return v;
  }
  operator bool() const {
    bool v = false;
    switch (type) {
      case data_type::boolean: {
        v = value.boolean;
        break;
      }
      default:
        break;
    }
    return v;
  }


 public:
  /// return the type as string
  std::string type_name() const {
    switch (type) {
      case (data_type::null): {
        return "null";
      }
      case (data_type::string): {
        return "string";
      }
      case (data_type::boolean): {
        return "boolean";
      }
      default: {
        return "number";
      }
    }
  }

  data_type Type() {
    return type;
  }

  bool empty() {
    return type == data_type::null;
  }

  bool is_true() {
    switch (type) {
      case data_type::null: {
        return false;
      }
      case data_type::boolean: {
        return value.boolean;
      }
      case data_type::number_integer: {
        return value.int_val != 0;
      }
      case data_type::number_float: {
        return value.f_val != 0;
      }
      default:
        return true;
        break;
    }
    return true;
  }

 private:
  data_type type;
  data_value value = data_type::null;
  std::map<std::string, auto_data> map_data;
  std::vector<auto_data> list_data;
};


//////////////////////////////////////////////////////////////////////////
// parse_val
//////////////////////////////////////////////////////////////////////////
// will call auto_data copy constructor
auto_data parse_val(std::string key, const auto_data& data) {
  // quoted string
  if (key[0] == '\"') {
    size_t index = key.substr(1).find_last_of("\"");
    if (index != std::string::npos) {
      return key.substr(1, index);
    }
    return "";
  }
  size_t index = key.find(".");
  if (index == std::string::npos) {
    if (!data.has(key)) {
      return auto_data();
    }
    return data.Get(key);
  }

  std::string sub_key = key.substr(0, index);
  if (!data.has(sub_key)) {
    return auto_data();
  }
  const auto_data& item = data.Get(sub_key);
  return parse_val(key.substr(index+1), item);
}



// token classes
typedef enum  {
  TOKEN_TYPE_NONE,
  TOKEN_TYPE_TEXT,
  TOKEN_TYPE_VAR,
  TOKEN_TYPE_IF,
  TOKEN_TYPE_FOR,
  TOKEN_TYPE_ENDIF,
  TOKEN_TYPE_ENDFOR,
} TokenType;



class Token;
using token_vector = std::vector<std::shared_ptr<Token>>;
// Template tokens
// base class for all token types
class Token {
 public:
  virtual TokenType gettype() = 0;
  virtual void set_children(const token_vector&) {
    printf("this token can't set child\n");
  }
  virtual std::string get_text(const auto_data&) { return ""; }
};




// normal text
class TokenText : public Token {
 private:
  std::string m_text;
 public:
  explicit TokenText(std::string text) : m_text(text) {}
  TokenType gettype() { return TOKEN_TYPE_TEXT;}
  std::string get_text(const auto_data&) {
    return m_text;
  }
};

// variable
class TokenVar : public Token {
 private:
  std::string m_key;

 public:
  explicit TokenVar(std::string key) : m_key(key) {}
  TokenType gettype() { return TOKEN_TYPE_VAR;}
  std::string get_text(const auto_data& data) {
    auto_data ret = parse_val(m_key, data);
    std::string str = "";
    switch (ret.Type()) {
      case auto_data::data_type::string: {
        return ret;
      }
      case auto_data::data_type::boolean: {
        bool b = ret;
        str = b ? "true":"false";
        break;
      }
      case auto_data::data_type::number_integer: {
        char temp[10] = {'\0'};
        int64_t t = ret;
        snprintf(temp, sizeof(temp), "%" PRIu64,  t);
        str = std::string(temp);
        break;
      }
      case auto_data::data_type::number_float: {
        char temp[10] = {'\0'};
        double t = ret;
        snprintf(temp, sizeof(temp), "%f",  t);
        str = std::string(temp);
        break;
      }
      case auto_data::data_type::null: {
        str = "";
        break;
      }
      default:
        break;
    }
    return str;
  }
};

// for block
class TokenFor : public Token {
 public:
  std::string m_key;
  std::string m_val;
  token_vector m_children;
  // 拆分出来
  explicit TokenFor(std::string expr) {
    std::vector<std::string> elements;
    char split[] = " ";
    SplitString(expr, split, &elements);
    if (elements.size() != 4) {
      perror("cpp template string have error syntax 'for'");
      exit(0);
    }
    m_val = elements[1];
    m_key = elements[3];
  }
  TokenType gettype() { return TOKEN_TYPE_FOR;}
  void set_children(const token_vector &children) {
    m_children.assign(children.begin(), children.end());
  }
  token_vector &get_children() {
    return m_children;
  }
  std::string get_text(const auto_data& data) {
    if (!data.has(m_key)) {
      printf("has no key:%s\n", m_key.c_str());
      return "";
    }
    const auto_data& l = data.Get(m_key);
    int listSize = l.size();
    std::string str = "";
    for (int i = 0; i < listSize; i++) {
      auto_data d;
      d[m_val] = l[i];;  // this will call operator=, and will create new object
      for (size_t j = 0; j < m_children.size(); ++j) {
        str += m_children[j]->get_text(d);
      }
    }
    return str;
  }
};

// if block
class TokenIf : public Token {
 public:
  std::string m_expr;
  token_vector m_children;
  explicit TokenIf(std::string expr) : m_expr(expr) {}
  TokenType gettype() { return TOKEN_TYPE_IF;}
  void set_children(const token_vector &children) {
    m_children.assign(children.begin(), children.end());
  }
  token_vector &get_children() { return m_children;}
  std::string get_text(const auto_data& data) {
    std::string str = "";
    if (is_true(data)) {
      for (size_t j = 0; j < m_children.size(); ++j) {
        str += m_children[j]->get_text(data);
      }
    } else {
      // printf("is not true:%s\n", m_expr.c_str());
    }
    return str;
  }
  bool is_true(const auto_data& data) {
    std::vector<std::string> elements;
    char split[] = " ";
    SplitString(m_expr, split, &elements);
    if (elements.size() == 1) {
      return false;
    } else if (elements.size() == 2) {
      return parse_val(elements[1], data).is_true();
    } else if (elements.size() == 3) {
      if (elements[1] == "not") {
        return !parse_val(elements[2], data).is_true();
      }
    } else if (elements.size() == 4) {
      if (elements[2] == "==") {
        auto_data lhs = parse_val(elements[1], data);
        auto_data rhs = parse_val(elements[3], data);
        return lhs == rhs;
      }
    }
    return false;
  }
};

// end of block
// end of control block
class TokenEnd : public Token {
 private:
  std::string m_type;
 public:
  explicit TokenEnd(std::string text) : m_type(text) {}
  TokenType gettype() {
    return m_type == "endfor" ? TOKEN_TYPE_ENDFOR : TOKEN_TYPE_ENDIF;
  }
};




//////////////////////////////////////////////////////////////////////////
// tokenize
// parses a template into tokens (text, for, if, variable)
//////////////////////////////////////////////////////////////////////////
token_vector tokenize(std::string text) {
  token_vector tokens;
  while (!text.empty()) {
    size_t pos = text.find("{");
    if (pos == std::string::npos) {
      if (!text.empty()) {
        tokens.push_back(std::shared_ptr<Token>(new TokenText(text)));
      }
      return tokens;
    }
    std::string pre_text = text.substr(0, pos);
    if (!pre_text.empty()) {
      tokens.push_back(std::shared_ptr<Token>(new TokenText(pre_text)));
    }
    text = text.substr(pos+1);
    if (text.empty()) {
      tokens.push_back(std::shared_ptr<Token>(new TokenText("{")));
      return tokens;
    }
    // variable
    if (text[0] == '$') {
      pos = text.find("}");
      if (pos != std::string::npos) {
        tokens.push_back(
            std::shared_ptr<Token>(new TokenVar(text.substr(1, pos-1))));
        text = text.substr(pos+1);
      }
    } else if (text[0] == '%') {  // control statement
      pos = text.find("}");
      if (pos != std::string::npos) {
        std::string expression = text.substr(1, pos-2);
        // strim
        size_t spos = expression.find_first_not_of(" ");
        if (spos != 0 && spos != std::string::npos) {
          expression = expression.substr(spos);
        }
        size_t epos = expression.find_last_not_of(" ");
        if (epos != expression.length() && epos != std::string::npos) {
          expression = expression.substr(0, epos+1);
        }
        text = text.substr(pos+1);
        if (expression.find_first_of("for") == 0) {
          tokens.push_back(std::shared_ptr<Token>(new TokenFor(expression)));
        } else if (expression.find_first_of("if") == 0) {
          tokens.push_back(std::shared_ptr<Token>(new TokenIf(expression)));
        } else {
          tokens.push_back(std::shared_ptr<Token>(
              new TokenEnd(expression)));
        }
      }
    } else {
      tokens.push_back(std::shared_ptr<Token>(new TokenText("{")));
    }
  }
  return tokens;
}

//////////////////////////////////////////////////////////////////////////
// parse_tree
// recursively parses list of tokens into a tree
//////////////////////////////////////////////////////////////////////////
void parse_tree(token_vector* tokens,
                token_vector* tree,
                TokenType until = TOKEN_TYPE_NONE) {
  while (!tokens->empty()) {
    std::shared_ptr<Token> token = tokens->at(0);
    tokens->erase(tokens->begin());
    if (token->gettype() == TOKEN_TYPE_FOR) {
      token_vector children;
      parse_tree(tokens, &children, TOKEN_TYPE_ENDFOR);
      token->set_children(children);
    } else if (token->gettype() == TOKEN_TYPE_IF) {
      token_vector children;
      parse_tree(tokens, &children, TOKEN_TYPE_ENDIF);
      token->set_children(children);
    } else if (token->gettype() == until) {
      return;
    }
    tree->push_back(token);
  }
}


std::string parse(std::string templ_text, const auto_data& data) {
  token_vector tokens;
  tokens = tokenize(templ_text);
  token_vector tree;
  parse_tree(&tokens, &tree);

  std::string str = "";
  for (size_t i = 0 ; i < tree.size() ; ++i) {
    str =  str + tree[i]->get_text(data);
  }
  return str;
}



}  // namespace cpptempl

#endif  // CPPTEMPL_H_
