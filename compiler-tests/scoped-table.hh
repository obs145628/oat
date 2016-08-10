#ifndef SCOPED_TABLE_HH_
# define SCOPED_TABLE_HH_

# include <string>
# include <map>

class ScopedTable
{
public:
   ScopedTable(ScopedTable* parent = nullptr);

   void define(const std::string& key, long value);
   void set(const std::string& key, long value);
   long get(const std::string& key) const;

private:
   std::map<std::string, long> _table;
   ScopedTable* _parent;

};

#endif //!SCOPED_TABLE_HH_
