#ifndef DOT_TREE_HH_
# define DOT_TREE_HH_

# include <string>
# include <vector>
# include <iostream>

class DotTree
{

public:
   DotTree(const std::string& value,
           const std::vector<DotTree*>& children = {});
   ~DotTree();
   DotTree(const DotTree& dt);

   void saveText(const std::string& path);
   void saveImage(const std::string& path);

private:
   std::string _value;
   std::vector<DotTree*> _children;

   void write(std::size_t& id, std::ostream& os) const;


   friend std::ostream& operator<<(std::ostream& os, const DotTree& dt);


};

#endif //!DOT_TREE_HH_
