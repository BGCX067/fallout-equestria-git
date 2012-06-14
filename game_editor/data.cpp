#include "datatree.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>

using namespace std;

void      Data::Output(unsigned char indent)
{
  Data::iterator it  = this->begin();
  Data::iterator end = this->end();

  for (unsigned char toIndent = indent ; toIndent ; --toIndent)
    std::cout << ' ';
  std::cout << "[" << _data->key << "] -> '" << _data->value << "'" << std::endl;
  for (; it != end ; ++it)
    (*it).Output(indent + 2);
}

static void WriteFile(ofstream& stream, const Data data, unsigned char indent)
{
  Data::const_iterator it  = data.const_begin();
  Data::const_iterator end = data.const_end();

  for (unsigned char toIndent = indent ; toIndent ; --toIndent)
    stream << ' ';
  stream << data.Key() << ": " << data.Value() << endl;
  for (; it != end ; ++it)
    (*it).Output(indent + 2);
}

bool DataTree::Save(const string& path)
{
  ofstream file;
  
  file.open(path.c_str(),ios::binary);
  if (file.is_open())
  {
    WriteFile(file, this, 0);
    return (true);
  }
  return (false);
}

Data::Data(DataTree* d) : _data(d)
{}

Data::Data(const std::string& key, DataBranch* father)
{
  _data = new DataBranch();
  _data->father   = father;
  _data->key      = key;
  _data->pointers = 1;
  _data->nil      = true;
  father->children.push_back(_data);
}

Data::~Data()
{
  if (_data)
  {
    _data->pointers--;
    if ((_data->nil || !_data->father) && _data->root == false && _data->pointers == 0)
    {
      delete _data;
      _data = 0;
    }
  }
}

Data Data::operator[](int cur)
{
  DataBranch::Children::iterator it  = _data->children.begin();
  DataBranch::Children::iterator end = _data->children.end();

  while (cur-- && it != end) ++it;
  if (it != end)
    return (Data((*it)));
  return (Data());
}

Data Data::operator[](const std::string& key)
{
  DataBranch::Children::iterator it  = _data->children.begin();
  DataBranch::Children::iterator end = _data->children.end();

  _data->nil = false;
  for (; it != end ; ++it)
  {
    if ((*it)->key == key)
      return (Data((*it)));
  }
  return (Data(key, _data));
}

const Data Data::operator[](const std::string& key) const
{
  DataBranch::Children::const_iterator it  = _data->children.begin();
  DataBranch::Children::const_iterator end = _data->children.end();

  _data->nil = false;
  for (; it != end ; ++it)
  {
    if ((*it)->key == key)
      return (Data((*it)));
  }
  return (Data(key, _data));
}

const Data& Data::operator=(const Data& var)
{
  if (_data)
  {
    _data->pointers--;
    if (Nil() && _data->pointers == 0)
      delete _data;
  }
  _data = var._data;
  if (_data)
    _data->pointers++;
  return (*this);
}

DataBranch::~DataBranch()
{
  Children::iterator it;
  Children::iterator end;

  if (father)
  {
    it  = father->children.begin();
    end = father->children.end();
    
    for (; it != end ; ++it)
    {
      if ((*it) == this)
      {
	father->children.erase(it);
	break ;
      }
    }
  }
  while (children.begin() != children.end())
  {
    DataBranch* child = *children.begin();

    if (child->nil == false)
      delete child;
    else
    {
      children.erase(children.begin());
      child->father = 0;
    }
  }
}

void        Data::MoveUp()
{
    Children&          list  = _data->father->children;
    Children::iterator it    = std::find(list.begin(), list.end(), _data);
    Children::iterator next  = it;

    if (it == list.end())
      return ;
    list.insert(--next, _data);
    list.erase(it);
}

void        Data::MoveDown()
{
    Children&          list  = _data->father->children;
    Children::iterator it    = std::find(list.begin(), list.end(), _data);
    Children::iterator next  = it;

    ++next;
    if (next == list.end())
      return ;
    ++next;
    list.insert(next, _data);
    list.erase(it);
}