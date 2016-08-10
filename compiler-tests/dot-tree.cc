#include "dot-tree.hh"
#include <fstream>
#include <stdexcept>
#include <cstdlib>

DotTree::DotTree(const std::string& value,
                 const std::vector<DotTree*>& children)
   : _value(value), _children(children)
{

}

DotTree::~DotTree()
{
   for(DotTree* child: _children)
      delete child;
}

DotTree::DotTree(const DotTree& dt)
   : DotTree(dt._value)
{
   for(DotTree* child: dt._children)
      _children.push_back(new DotTree(*child));
}

void DotTree::saveText(const std::string& path)
{
   std::ofstream fs(path, std::ios::binary);
   if(!fs.is_open())
      throw std::runtime_error{"Unable to open file " + path};
   fs << *this;
}

void DotTree::saveImage(const std::string& path)
{
   std::string dotPath = "/tmp/oat_tree.dot";
   saveText(dotPath);

   std::string cmd = "dot -Tpng -o " + path + " " + dotPath;
   if(system(cmd.c_str()) != 0)
      throw std::runtime_error{"Unable to save image to " + path};
}


void DotTree::write(std::size_t& id, std::ostream& os) const
{
   std::size_t parentId = id;

   os << "  node" << std::to_string(id++) << " [label=\""
      << _value << "\"]" << std::endl;

   for(DotTree* child: _children)
   {
      std::size_t childId = id;
      child->write(id, os);
      os << "  node" << std::to_string(parentId) << " -> node"
         << std::to_string(childId) << std::endl;
   }
}


std::ostream& operator<<(std::ostream& os, const DotTree& dt)
{
   std::size_t id = 1;

   os << "digraph astgraph {" << std::endl;
   os << "  node [shape=none, fontsize=12, fontname=\"Courier\", height=.1];"
      << std::endl;
   os << "  ranksep=.3;" << std::endl;
   os << "  edge [arrowsize=.5]" << std::endl << std::endl;

   dt.write(id, os);
   os << "}" << std::endl;
   return os;
}


/*
digraph astgraph {
  node [shape=none, fontsize=12, fontname="Courier", height=.1];
  ranksep=.3;
  edge [arrowsize=.5]

  node1 [label="expr"]
  node2 [label="term"]
  node1 -> node2
  node3 [label="+"]
  node1 -> node3
  node4 [label="term"]
  node1 -> node4

*/
