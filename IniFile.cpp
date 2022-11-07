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
	
	void save() const;
	
    /**
     * Считывает из секции section
     * значение ключа key
     * Если не удалось считать - возвращает default value
     * 
     * Определить для std::string, int, float, bool
     */
	template<typename T>
	T read(const std::string &section, const std::string &key, T defaultValue = T{}) const
    {
       auto sectionIt = _data.find(section);
       if (sectionIt == _data.end())
       {
           throw std::runtime_error("No section " + section + " in file.");
       }
//       (*sectionIt).second.find(key);
    }
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