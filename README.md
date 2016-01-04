# cpptempl
a template engine for c++


<!-- ## Design goals： -->

## Syntax
### template string
* Variables
```cpp
{$variable}
```
* Loops：
```cpp
{% for person in people %}Name: {$person.name}{% endfor %}
```
* If：
```cpp
{% if person.name == "xu" %}Full name: xu{% endif %}
```

### usage
```cpp
cpptempl::auto_data data;
data["age"] = 10;
data["name"] = "xu";
std::string str = "name:{$name}, age:{$age}";
std::string ret = cpptempl::parse(str, data);  // ret will be "name:xu, age:10"
```

## Integration
The single required source, file json.hpp is in the src directory, All you need to do is add
```cpp
#include "cpptempl.hpp"

```

## Implicit conversions
The type of the variable is determined automatically by the expression to store. Likewise, the stored value is implicitly converted.
```cpp
// number
cpptempl::auto_data t = 1;
int i = t;

// string
std::string str = "test";
cpptempl::auto_data s = str;
std::string str2 = s;

// boolean
bool b = true;
cpptempl::auto_data cb = b;
bool b2 = cb;
```
