#include <string>
#include <map>
#include <vector>

#include <fstream>

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
            auto sectionOpenBracket = std::find(fileLine.begin(), fileLine.end(), '[');
            auto sectionCloseBracket = std::find(fileLine.begin(), fileLine.end(), ']');
            if (sectionOpenBracket != fileLine.end() && sectionCloseBracket != fileLine.end())
            {
                std::map<std::string, std::string> sectionData;
                std::string sectionName;
                std::copy(sectionOpenBracket, sectionCloseBracket + 2, std::back_inserter(sectionName));

                while (!file.eof())
                {
                    std::getline(file, fileLine);
                    if (std::find(fileLine.begin(), fileLine.end(), '[') != fileLine.end())
                    {
                        _data.insert({sectionName, sectionData});
                        break;
                    }
                    auto equalSign = std::find(fileLine.begin(), fileLine.end(), '=');
                    if (equalSign == fileLine.end())
                    {
                        continue;
                    }
                    std::string key;
                    std::string value;
                    std::copy(fileLine.begin(), equalSign, std::back_inserter(key));
                    std::copy(equalSign + 1, fileLine.end(), std::back_inserter(value));
                    sectionData.insert({key, value});
                }
                _data.insert({sectionName, sectionData});
            }
        }
    }
	
	void save() const;
	
    /**
     * Считывает из секции section
     * значение ключа key
     * Если не удалось считать - возвращает default value
     * 
     * Определить для std::string, int, float, bool
     */
	template<typename T>
	T read(const std::string &section, const std::string &key, T defaultValue = T{}) const;
    /**
     * В ключ key из секции section 
     * записывает значение value
     *
     * Определить для std::string, int, float, bool
     */
	template<typename T>
	void write(const std::string &section, const std::string &key, T value);

    /**
     * Проверяет, существует ли секция section
     */
    bool sectionExists(const std::string &section) const;

    /**
     * Проверяет, существует ли ключ key в секции section
     */
    bool keyExists(const std::string &section, const std::string &key) const;

    /**
     * Возвращает существующие секции
     */
    std::vector<std::string> sections() const;

    /**
     * Возвращает существующие ключи в секции section
     */
    std::vector<std::string> keys(const std::string &section) const;

private:
    std::map<
            std::string,
            std::map<std::string, std::string>
    > _data;
    const std::string _path;
};

// true -  true, True, TRUE, on, On, ON, yes, Yes, YES, y, Y, 1
// false - остальные