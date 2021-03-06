#include "ui/alert_ui.hpp"

using namespace std;
using namespace Rocket;

Sync::Signal<void (const string)> AlertUi::NewAlert;

AlertUi::AlertUi(WindowFramework* window, Core::Context* context, const string& message) : UiBase(window, context)
{
  _continue = true;
  root     = context->LoadDocument("data/alert.rml");
  if (root)
  {
    Core::Element* elem_message = root->GetElementById("message");

    elem_message->SetInnerRML(message.c_str());
    ToggleEventListener(true, "button-ok", "click", ButtonClicked);
    ButtonClicked.EventReceived.Connect([this](Core::Event&) { _continue = false; });
  }
}

AlertUi::~AlertUi()
{
}

bool AlertUi::Run(void)
{
  if (root)
  {
    root->PullToFront();
  }
  return (_continue);
}

