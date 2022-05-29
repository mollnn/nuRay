#include "config.h"

Config::Config()
{
}

Config::Config(const std::string &str)
{
    fromString(str);
}

void Config::fromString(const std::string &str)
{
    std::stringstream ss(str);
    std::string kv;
    while (ss >> kv)
    {
        addItem(kv);
    }
}

std::string Config::getValueStr(const std::string &key, const std::string &default_value)
{
    if (mp.find(key) == mp.end())
    {
        std::cerr << "Fail to get config item " << key << std::endl;
        return "";
    }
    return mp[key];
}

void Config::setValueStr(const std::string &key, const std::string &value)
{
    mp[key] = value;
}

int Config::getValueInt(const std::string &key, int default_value)
{
    if (mp.find(key) == mp.end())
    {
        return default_value;
    }
    std::stringstream ss(mp[key]);
    int x;
    if (!(ss >> x))
    {
        return default_value;
    }
    return x;
}

float Config::getValueFloat(const std::string &key, float default_value)
{
    if (mp.find(key) == mp.end())
    {
        return default_value;
    }
    std::stringstream ss(mp[key]);
    float x;
    if (!(ss >> x))
    {
        return default_value;
    }
    return x;
}

vec3 Config::getValueVec3(const std::string &key, const vec3 &default_value)
{
    if (mp.find(key) == mp.end())
    {
        return default_value;
    }
    std::string str = mp[key];
    for (auto &x : str)
    {
        if (x == ',')
        {
            x = ' ';
        }
    }
    std::stringstream ss(str);
    float x, y, z;
    if (!(ss >> x >> y >> z))
    {
        return default_value;
    }
    return vec3(x, y, z);
}

vec3 Config::getValueVec3RGB(const std::string &key, const vec3 &default_value)
{
    float x = getValueFloat(key + ".r", default_value[0]);
    float y = getValueFloat(key + ".g", default_value[1]);
    float z = getValueFloat(key + ".b", default_value[2]);
    return vec3(x, y, z);
}

vec3 Config::getValueVec3XYZ(const std::string &key, const vec3 &default_value)
{
    float x = getValueFloat(key + ".x", default_value[0]);
    float y = getValueFloat(key + ".y", default_value[1]);
    float z = getValueFloat(key + ".z", default_value[2]);
    return vec3(x, y, z);
}

void Config::addItem(const std::string &kv)
{
    std::string key, value;
    auto id = kv.find('=');
    key = kv.substr(0, id);
    value = kv.substr(id + 1);
    mp[key] = value;
}

void Config::print()
{
    for (auto &[x, y] : mp)
    {
        std::cout << x << ": " << y << std::endl;
    }
}

void Config::fromFile(const std::string &filename)
{
    mp.clear();
    std::ifstream ifs(filename);
    std::string str_line;
    while (getline(ifs, str_line))
    {
        std::stringstream ss(str_line);
        std::string key, op, value;
        ss >> key >> op >> value;
        if (op == "=")
        {
            mp[key] = value;
        }
        else if (op == "+=")
        {
            mp[key] += value;
        }
    }
}