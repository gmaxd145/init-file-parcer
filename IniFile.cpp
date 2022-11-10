#include <string>
#include <map>
#include <vector>

#include <fstream>
#include <sstream>

class IniFile {
public:
	IniFile(const std::string& path) : _path(path)
    {
        std::ifstream file(_path);
        if (!file.is_open())
        {
            throw std::runtime_error("Open file error");
        }
        std::string fileLine;
        std::getline(file, fileLine);
        while(!file.eof())
        {
            auto sectionOpenBracket = fileLine.find('[');
            auto sectionCloseBracket = fileLine.find(']');
            if (sectionOpenBracket != std::string::npos && sectionCloseBracket != std::string::npos)
            {
                std::string sectionName = fileLine.substr(sectionOpenBracket + 1, sectionCloseBracket - 1);
                std::map<std::string, std::string> sectionData;
                while (!file.eof())
                {
                    std::getline(file, fileLine);
                    if (fileLine.find('[') != std::string::npos)
                    {
                        _data.insert({sectionName, sectionData});
                        break;
                    }
                    auto equalSign = fileLine.find('=');
                    if (equalSign == std::string::npos)
                    {
                        continue;
                    }
                    std::string key = fileLine.substr(0, equalSign);
                    std::string value = fileLine.substr(equalSign + 1, fileLine.length());
                    sectionData.insert({key, value});
                }
                _data.insert({sectionName, sectionData});
            }
        }
    }

    ~IniFile()
    {
        save();
    }

	void save() const
    {
        std::ofstream file(_path);
        if (!file.is_open())
        {
            throw std::runtime_error("Open file error");
        }
        for (const auto& [section, sectionData] : _data)
        {
            file << "[" + section + "]" << std::endl;
            for (const auto& [key, value] : _data.at(section))
            {
                file << key + "=" + value << std::endl;
            }
        }
    }

    /*
     * Считывает из секции section
     * значение ключа key
     * Если не удалось считать - возвращает default value
     *
     * Определить для std::string, int, float, bool
     */
    template<typename T>
    T read(const std::string& section, const std::string& key, const T defaultValue = T{}) const
    {
        if (!keyExists(section, key))
        {
            return defaultValue;
        }
        std::stringstream ss(_data.at(section).at(key));
        T returnValue;
        ss >> returnValue;
        return returnValue;
    }

    /**
     * В ключ key из секции section 
     * записывает значение ss
     *
     * Определить для std::string, int, float, bool
     */
	template<typename T>
	void write(const std::string &section, const std::string &key, T value)
    {
        std::stringstream ss;
        ss << value;
        if (keyExists(section, key))
        {
            _data.at(section).at(key) = ss.str();
        }
        else if (sectionExists(section))
        {
            _data.at(section).insert({key, ss.str()});
        }
        else
        {
            std::map<std::string, std::string> sectionData;
            sectionData.insert({key, ss.str()});
            _data.insert({section, sectionData});
        }
    }

    /**
     * Проверяет, существует ли секция section
     */
    bool sectionExists(const std::string &section) const
    {
        return _data.find(section) != _data.end();
    }

    /**
     * Проверяет, существует ли ключ key в секции section
     */
    bool keyExists(const std::string &section, const std::string &key) const
    {
        return (sectionExists(section) && _data.at(section).find(key) != _data.at(section).end());
    }

    /**
     * Возвращает существующие секции
     */
    std::vector<std::string> sections() const
    {
        std::vector<std::string> result{}; // {} ?
        for (const auto& [section, sectionData] : _data)
        {
            result.push_back(section);
        }
        return result;
    }

    /**
     * Возвращает существующие ключи в секции section
     */
    std::vector<std::string> keys(const std::string &section) const
    {
        std::vector<std::string> result{}; // {} ?
        if (sectionExists(section))
        {
            for (const auto& [key, value] : _data.at(section))
            {
                result.push_back(key);
            }
        }
        return result;
    }

private:
    std::vector<std::string> boolPossibleValues= {"true", "True", "TRUE", "on", "On", "ON", "yes", "Yes", "YES", "y",
                                                  "Y", "1"};
    std::map<
            std::string,
            std::map<std::string, std::string>
    > _data;
    const std::string _path;
};

template<>
bool IniFile::read(const std::string &section, const std::string &key, const bool defaultValue) const
{
    return keyExists(section, key) && (std::find(boolPossibleValues.begin(), boolPossibleValues.end(),
                                                    _data.at(section).at(key)) != boolPossibleValues.end());
}

template<>
std::string IniFile::read(const std::string &section, const std::string &key, const std::string defaultValue) const
{
    return keyExists(section, key) ? _data.at(section).at(key) : defaultValue;
}

template<>
void IniFile::write(const std::string &section, const std::string &key, bool value)
{
    std::stringstream ss;
    ss << value;
    std::string buf = ss.str() == "1" ? "true" : "false";
    if (keyExists(section, key))
    {
        _data.at(section).at(key) = buf;
    }
    else if (sectionExists(section))
    {
        _data.at(section).insert({key, buf });
    }
    else
    {
        std::map<std::string, std::string> sectionData;
        sectionData.insert({key, buf});
        _data.insert({section, sectionData});
    }
}