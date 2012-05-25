#ifndef  DIALOG_HPP
# define DIALOG_HPP

# include "rocket_extension.hpp"
# include "scriptengine.hpp"
#include "datatree.hpp"
# include <list>
# include <string>
# include <algorithm>

struct DialogAnswers
{
  typedef std::pair<std::string, std::string> KeyValue;
  typedef std::list<KeyValue>                 AnswerList;

  AnswerList answers;
};

class DialogModel
{
public:
  DialogModel(const std::string& dialogId)
  {
    _tree = DataTree::Factory::JSON("data/dialogs/" + dialogId + ".json");
    if (_tree)
      _data = Data(_tree);
  }

  ~DialogModel()
  {
    if (_tree) delete _tree;
  }

  void               SetCurrentNpcLine(const std::string& id);  
  const std::string  GetHookAvailable(const std::string& answerId)
  {
    return (_data[_currentNpcLine][answerId]["HookAvailable"].Value());
  }

  const std::string  GetExecuteMethod(const std::string& answerId)
  {
    return (_data[_currentNpcLine][answerId]["HookExecute"].Value());
  }

  const std::string  GetDefaultNextLine(const std::string& answerId)
  {
    return (_data[_currentNpcLine][answerId]["DefaultAnswer"].Value());
  }

  const std::string GetNpcLine(void)
  {
    return (_data[_currentNpcLine].Key()); // TODO replace this with L18n traduction of the key
  }

  DialogAnswers     GetDialogAnswers(void);

private:
  DataTree*          _tree;
  Data               _data;
  std::string        _currentNpcLine;
};

class DialogView : public UiBase
{
public:
  void Destroy(void);
protected:
  DialogView(WindowFramework* window, Rocket::Core::Context* context);
  ~DialogView();

  void UpdateView(const std::string& npcLine, const DialogAnswers& answers);
  void CleanView(const DialogAnswers& answers);

  RocketListener AnswerSelected;

private:
  Rocket::Core::Element* _containerNpcLine;
  Rocket::Core::Element* _containerAnswers;
};

class DialogController : public DialogView
{
public:
  DialogController(WindowFramework* window, Rocket::Core::Context* context, const std::string& dialogId);
  ~DialogController()
  {
    _context->Release();
  }

  Observatory::Signal<void ()> DialogEnded;

private:
  void             ExecuteAnswer(Rocket::Core::Event& event);
  void             SetCurrentNode(const std::string& nodeName);

  asIScriptContext* _context;
  asIScriptModule*  _module;
  DialogModel       _model;
};


#endif