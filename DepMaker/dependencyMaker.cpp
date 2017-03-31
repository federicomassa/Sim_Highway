#include <string>
#include <vector>
#include <regex>
#include <iostream>
#include <fstream>
#include <sstream>
#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>

using namespace std;
using namespace boost::filesystem;

namespace dep
{
  void writeDependency(ofstream&, const std::string&);
  bool insertUnique(std::vector<std::string>&, const std::string&);
  vector<std::string> lookForDependencies(const std::string&);
}



int main()
{

  stringstream ss;
  ofstream output("dep.mk");
  
  // === LOOK FOR cpp FILES in . directory === //
  
  /* regular expression to look for cpp files */
  regex cpp("\"(?:\\./)([[:alnum:]./]+)(?:\\.cpp)\"");

  /*   path to the directory where the files of which we want to track the dependencies are */
  path p(".");
   
  for (auto& entry : boost::make_iterator_range(directory_iterator(p), {}))
    {
      ss << entry;
      string entryStr;

      ss >> entryStr;
      ss.clear();
      
      cmatch cm;
      if (!regex_match(entryStr.c_str(), cm, cpp))
	continue;

      
      if (cm.size() != 2)
	{
	  std::cout << "Invalid regular expression match!" << std::endl;
	  exit(1);
	}
      
      dep::writeDependency(output, cm[1]);
    }
  
  return 0;
}

void dep::writeDependency(ofstream& output, const std::string& cppName)
{
  std::cout << "Writing rules for " << cppName << ".o ..." << std::endl;
  output << cppName << ".o: ";

  ifstream testIn;

  vector<std::string> dependencies;
  dependencies.push_back(cppName + ".cpp");

  /* try if .h exists */
  testIn.open((cppName + ".h").c_str());
  if (testIn.good())
    dependencies.push_back(cppName + ".h");

  testIn.close();
  
  /* now iterate on sub files */
  for (int i = 0; i < dependencies.size(); i++)
    {
      vector<std::string> subDeps = dep::lookForDependencies(dependencies[i]);

      for (int j = 0; j < subDeps.size(); j++)
	{
	  dep::insertUnique(dependencies, subDeps[j]);
	  
	}
            
    }
  
  
  for (std::vector<std::string>::iterator itr = dependencies.begin(); itr != dependencies.end(); itr++)
    {
      output << *itr << ' ';
    }
  
  output << std::endl;
  
}

/* insert an element into the vector, if not already present */
bool dep::insertUnique(vector<std::string>& v, const std::string& element)
{
  bool alreadyPresent = false;
  
  for (std::vector<std::string>::iterator itr = v.begin(); itr != v.end(); itr++)
    {
      if ((*itr) != element)
	continue;
      else
	{
	  alreadyPresent = true;
	  break;
	}
    }
  
  if (!alreadyPresent)
    v.push_back(element);

  return !alreadyPresent;
}

vector<std::string> dep::lookForDependencies(const std::string& fName)
{
  /* regular expression to look for included files */
  regex include("(?:#include)(?:[[:space:]]+)(?:\")([[:alnum:]./]+)(?:\")(?:[[:space:]]*)");

  ifstream in(fName.c_str());
  string tmpS;

  if (!in.good())
    {
      std::cout << "Problem with file " << fName << std::endl;
      exit(1);
    }
  
  vector<std::string> deps;
  
  while (!in.eof())
    {
      getline(in, tmpS);
      
      cmatch cm;
      if (!regex_match(tmpS.c_str(), cm, include))
	continue;
      
      if (cm.size() != 2)
	{
	  std::cout << "Invalid regular expression match!" << std::endl;
	  exit(1);
	}

      deps.push_back(cm[1]);

    }
  

  return deps;
}
