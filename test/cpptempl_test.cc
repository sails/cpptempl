// Copyright (C) 2015 sails Authors.
// All rights reserved.
//
// Official git repository and contact information can be found at
// https://github.com/sails/sails and http://www.sailsxu.com/.
//
// Filename: cpptempl_test.cc
//
// Author: sailsxu <sailsxu@gmail.com>
// Created: 2015-12-30 11:59:49



#define CATCH_CONFIG_MAIN

#include <time.h>
#include "catch.hpp"
#include "../src/cpptempl.hpp"

TEST_CASE("cpptempl1", "nomal object") {
  // test nomal obj
  cpptempl::auto_data data;
  data["age"] = 10;
  data["name"] = "xu";
  std::string str = "name:{$name}, age:{$age}";
  std::string ret = cpptempl::parse(str, data);
  REQUIRE(ret == "name:xu, age:10");
  printf("ret:%s\n", ret.c_str());
}


TEST_CASE("cpptempl2", "object") {
  // test nomal obj
  cpptempl::auto_data pers;
  cpptempl::auto_data p1;
  p1["age"] = 11;
  p1["name"] = "x";
  cpptempl::auto_data p2;
  p2["age"] = 12;
  p2["name"] = "s";
  pers["one"] = p1;
  pers["two"] = p2;
  std::string str = "p1(name:{$one.name}, age:{$one.age});"
                    "p2(name:{$two.name}, age:{$two.age})";
  std::string ret = cpptempl::parse(str, pers);
  REQUIRE(ret == "p1(name:x, age:11);p2(name:s, age:12)");
  printf("ret:%s\n", ret.c_str());
}


TEST_CASE("cpptempl3", "nomal object") {
  // 差不多1秒10w次
  uint32_t start = time(NULL);
  for (int i = 0; i < 100000; i++) {
    cpptempl::auto_data data;
    data["age"] = 10;
    data["name"] = "xu";
    std::string str = "name:{$name}, age:{$age}";
    cpptempl::parse(str, data);
  }
  uint32_t end = time(NULL);
  printf("speed:%us for 10w \n", (end-start));
}

TEST_CASE("cpptempl4", "if block") {
  // test int value
  cpptempl::auto_data data1;
  data1["test"] = 10;
  std::string str = "{%if test %}ok{% endif %}";
  std::string ret = cpptempl::parse(str, data1);
  REQUIRE(ret == "ok");
  printf("%s\n", ret.c_str());

  // test bool value
  cpptempl::auto_data data2;
  data2["test"] = true;
  str = "{%if test %}test1"
        "{%if test%}test2"
        "{%if not test%}test3{%endif%}"
        "{%endif%}"
        "{% endif %}";
  ret = cpptempl::parse(str, data2);
  REQUIRE(ret == "test1test2");
  printf("%s\n", ret.c_str());

  // null value
  cpptempl::auto_data data3;
  str = "{%if test %}test{% endif %}";
  ret = cpptempl::parse(str, data3);
  REQUIRE(ret == "");

  // test nested obj
  cpptempl::auto_data temp;
  temp["test"] = true;
  cpptempl::auto_data data4;
  data4["test"] = temp;
  str = "{%if test.test %}test{% endif %}";
  ret = cpptempl::parse(str, data4);
  REQUIRE(ret == "test");

  // test == operator
  cpptempl::auto_data data5;
  data5["test"] = 10;
  str = "{%if test == test %}test{% endif %}";
  ret = cpptempl::parse(str, data5);
  REQUIRE(ret == "test");
}



TEST_CASE("cpptempl5", "for block") {
  cpptempl::auto_data data;
  data["test"].push_back(1);
  data["test"].push_back(2);
  std::string str = "{%for d in test%}test:{$d}{% endfor %}";
  std::string ret = cpptempl::parse(str, data);
  printf("for test:%s\n", ret.c_str());
  // REQUIRE(ret == "test");

  cpptempl::auto_data data2;
  cpptempl::auto_data p1;
  p1["name"] = "xu";
  cpptempl::auto_data p2;
  p2["name"] = "car";
  data2["persons"].push_back(p1);
  data2["persons"].push_back(p2);
  str = "{%for person in persons%}name:{$person.name} {% endfor %}";
  ret = cpptempl::parse(str, data2);
  printf("for test:%s\n", ret.c_str());
}
