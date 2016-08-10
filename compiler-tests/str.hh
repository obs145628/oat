#ifndef STR_HH_
# define STR_HH_

# include <string>

namespace str
{
   bool startsWith(const std::string& s, const std::string& sub);

   bool isSymbol(const std::string& str);

   long toInt(const std::string& str, bool* ok = nullptr);
   double toFloat(const std::string& str, bool* ok = nullptr);

}

#endif //!STR_HH_
