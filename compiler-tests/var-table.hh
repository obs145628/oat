#ifndef VAR_TABLE_HH_
# define VAR_TABLE_HH_

#include <map>
#include <string>

class VarTable
{

public:
   bool contains(const std::string& key) const;
   std::string get(const std::string& key) const;
   void set(const std::string& key, const std::string& value);
   void put(const std::string& key, const std::string& value);
   void remove(const std::string& key);



private:
   std::map<std::string, std::string> _map;

};

#endif //!VAR_TABLE_HH_
