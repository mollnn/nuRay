#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <bits/stdc++.h>
#include "../math/vec3.h"

class Config
{
    std::map<std::string, std::string> mp;

public:
    Config();
    Config(const std::string &str);
    void fromString(const std::string &str);
    std::string getValueStr(const std::string &key, const std::string &default_value);
    int getValueInt(const std::string &key, int default_value);
    float getValueFloat(const std::string &key, float default_value);
    vec3 getValueVec3(const std::string &key, const vec3 &default_value);
    void setValueStr(const std::string &key, const std::string &value);
};

#endif