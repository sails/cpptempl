# cpptempl
a template engine for c++


## 目标：
### 一：用户只用引入一个头文件cpptempl.hpp

### 二：在c++层面上可以直接支持boolean, string, num, vector, list赋值.

  * cpptempl t = 1;
  * cpptempl t = "value";
  * cpptempl t = std::vector<std::string>();
  * cpptempl t; t["11"] = 1;t["22"] = 2;
  * cpptempl t; t[0]["1"] = 1; t[1]["1"] = 2;


### 三：在模板层面上可以实现：
  * 变量：{$variable}
  * 循环：{% for person in people %}Name: {$person.name}{% endfor %}
  * 条件：{% if person.name == "xu" %}Full name: xu{% endif %}


