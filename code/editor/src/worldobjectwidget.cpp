#include "worldobjectwidget.h"
#include "ui_worldobjectwidget.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QKeySequence>
#include <QInputDialog>
#include "selectableresource.h"

WorldObjectWidget::WorldObjectWidget(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::WorldObjectWidget)
{
  ui->setupUi(this);
  selection.object = 0;
  selection_type   = 0;
  UnsetSelection();

  ui->actionsButton->setMenu(&action_menu);

  connect(ui->objectName, SIGNAL(textChanged(QString)), this, SLOT(UpdateName(QString)));

  // Geometry
  connect(ui->objectPosX,        SIGNAL(valueChanged(double)), this, SLOT(UpdateGeometry()));
  connect(ui->objectPosY,        SIGNAL(valueChanged(double)), this, SLOT(UpdateGeometry()));
  connect(ui->objectPosZ,        SIGNAL(valueChanged(double)), this, SLOT(UpdateGeometry()));
  connect(ui->objectRotationX,   SIGNAL(valueChanged(double)), this, SLOT(UpdateGeometry()));
  connect(ui->objectRotationY,   SIGNAL(valueChanged(double)), this, SLOT(UpdateGeometry()));
  connect(ui->objectRotationZ,   SIGNAL(valueChanged(double)), this, SLOT(UpdateGeometry()));
  connect(ui->objectScaleX,      SIGNAL(valueChanged(double)), this, SLOT(UpdateGeometry()));
  connect(ui->objectScaleY,      SIGNAL(valueChanged(double)), this, SLOT(UpdateGeometry()));
  connect(ui->objectScaleZ,      SIGNAL(valueChanged(double)), this, SLOT(UpdateGeometry()));
  connect(ui->objectFloor,       SIGNAL(valueChanged(int)),    this, SLOT(UpdateFloor()));
  connect(ui->inheritsFloor,     SIGNAL(toggled(bool)),        this, SLOT(UpdateFloor()));

  // Collider
  connect(ui->displayColliders,  SIGNAL(toggled(bool)),            this, SLOT(UpdateColliderDisplay()));
  connect(ui->collider_type,     SIGNAL(currentIndexChanged(int)), this, SLOT(UpdateColliderType()));
  connect(ui->collider_pos_x,    SIGNAL(valueChanged(double)),     this, SLOT(UpdateColliderGeometry()));
  connect(ui->collider_pos_y,    SIGNAL(valueChanged(double)),     this, SLOT(UpdateColliderGeometry()));
  connect(ui->collider_pos_z,    SIGNAL(valueChanged(double)),     this, SLOT(UpdateColliderGeometry()));
  connect(ui->collider_hpr_x,    SIGNAL(valueChanged(double)),     this, SLOT(UpdateColliderGeometry()));
  connect(ui->collider_hpr_y,    SIGNAL(valueChanged(double)),     this, SLOT(UpdateColliderGeometry()));
  connect(ui->collider_hpr_z,    SIGNAL(valueChanged(double)),     this, SLOT(UpdateColliderGeometry()));
  connect(ui->collider_scale_x,  SIGNAL(valueChanged(double)),     this, SLOT(UpdateColliderGeometry()));
  connect(ui->collider_scale_y,  SIGNAL(valueChanged(double)),     this, SLOT(UpdateColliderGeometry()));
  connect(ui->collider_scale_z,  SIGNAL(valueChanged(double)),     this, SLOT(UpdateColliderGeometry()));

  // Light
  connect(ui->lightSetEnabled,   SIGNAL(toggled(bool)),         this, SLOT(LightSetEnabled(bool)));
  connect(ui->lightSetDisabled,  SIGNAL(toggled(bool)),         this, SLOT(LightSetDisabled(bool)));
  connect(ui->lightColorR,       SIGNAL(valueChanged(double)),  this, SLOT(UpdateLightColor()));
  connect(ui->lightColorG,       SIGNAL(valueChanged(double)),  this, SLOT(UpdateLightColor()));
  connect(ui->lightColorB,       SIGNAL(valueChanged(double)),  this, SLOT(UpdateLightColor()));
  connect(ui->lightAttenuationA, SIGNAL(valueChanged(double)),  this, SLOT(UpdateLightAttenuation()));
  connect(ui->lightAttenuationB, SIGNAL(valueChanged(double)),  this, SLOT(UpdateLightAttenuation()));
  connect(ui->lightAttenuationC, SIGNAL(valueChanged(double)),  this, SLOT(UpdateLightAttenuation()));
  connect(ui->lightTypesList,    SIGNAL(currentIndexChanged(int)), this, SLOT(UpdateLightType()));
  connect(ui->lightCompile,      SIGNAL(clicked()),             this, SLOT(LightCompile()));
  connect(ui->lightPriority,     SIGNAL(valueChanged(int)),     this, SLOT(UpdateLightPriority()));
  connect(ui->addLightTarget,    SIGNAL(clicked()),             this, SLOT(AddEnlightenedObject()));
  connect(ui->deleteLightTarget, SIGNAL(clicked()),             this, SLOT(DeleteEnlightenedObject()));
  connect(ui->showFrustum,       SIGNAL(toggled(bool)),         this, SLOT(LightShowFrustum(bool)));
  connect(ui->lightTargets,      SIGNAL(updatedPriority()),     this, SLOT(UpdateEnlightenedObject()));
  connect(ui->lightTargets,      SIGNAL(updatedPropagation()),  this, SLOT(UpdateEnlightenedObject()));

  // Light -> Shadow caster
  connect(ui->shadowFilmSize,    SIGNAL(valueChanged(int)),     this, SLOT(UpdateShadowCaster()));
  connect(ui->shadowNear,        SIGNAL(valueChanged(int)),     this, SLOT(UpdateShadowCaster()));
  connect(ui->shadowFar,         SIGNAL(valueChanged(int)),     this, SLOT(UpdateShadowCaster()));
  connect(ui->shadowBufferSizeX, SIGNAL(valueChanged(int)),     this, SLOT(UpdateShadowCaster()));
  connect(ui->shadowBufferSizeY, SIGNAL(valueChanged(int)),     this, SLOT(UpdateShadowCaster()));

  // Render
  connect(ui->selectModel,   SIGNAL(clicked()), this, SLOT(PickModel()));
  connect(ui->selectTexture, SIGNAL(clicked()), this, SLOT(PickTexture()));
  connect(ui->objectModel,   SIGNAL(textChanged(QString)), this, SLOT(UpdateRender()));
  connect(ui->objectTexture, SIGNAL(textChanged(QString)), this, SLOT(UpdateRender()));
  connect(ui->objectFocus,   SIGNAL(clicked()), this, SLOT(FocusCurrentObject()));
  connect(ui->useTexture,    SIGNAL(toggled(bool)), this, SLOT(UpdateRender()));
  connect(ui->useColor,      SIGNAL(toggled(bool)), this, SLOT(UpdateRender()));
  connect(ui->useOpacity,    SIGNAL(toggled(bool)), this, SLOT(UpdateRender()));
  connect(ui->colorRed,      SIGNAL(valueChanged(double)), SLOT(UpdateRender()));
  connect(ui->colorGreen,    SIGNAL(valueChanged(double)), SLOT(UpdateRender()));
  connect(ui->colorBlue,     SIGNAL(valueChanged(double)), SLOT(UpdateRender()));
  connect(ui->opacity,       SIGNAL(valueChanged(double)), SLOT(UpdateRender()));
  connect(ui->objectToggleVisibility, SIGNAL(clicked()), this, SLOT(ToogleCurrentObject()));

  // Waypoint
  connect(ui->setCurrentWaypoint, SIGNAL(clicked()), this, SLOT(SetCurrentWaypoint()));
  connect(ui->selectCurrentWaypoint, SIGNAL(clicked()), this, SLOT(SelectCurrentWaypoint()));

  // Behaviour
  connect(ui->objectTypeList,       SIGNAL(currentIndexChanged(int)), this, SLOT(UpdateBehaviour()));
  connect(ui->character,            SIGNAL(textChanged(QString)),     this, SLOT(UpdateBehaviour()));
  connect(ui->dialog,               SIGNAL(textChanged(QString)),     this, SLOT(UpdateBehaviour()));
  connect(ui->script,               SIGNAL(textChanged(QString)),     this, SLOT(UpdateBehaviour()));
  connect(ui->doorLocked,           SIGNAL(toggled(bool)),            this, SLOT(UpdateBehaviour()));
  connect(ui->key,                  SIGNAL(textChanged(QString)),     this, SLOT(UpdateBehaviour()));
  connect(ui->interactionUse,       SIGNAL(toggled(bool)),            this, SLOT(UpdateBehaviour()));
  connect(ui->interactionUseSkill,  SIGNAL(toggled(bool)),            this, SLOT(UpdateBehaviour()));
  connect(ui->interactionUseSpell,  SIGNAL(toggled(bool)),            this, SLOT(UpdateBehaviour()));
  connect(ui->interactionUseObject, SIGNAL(toggled(bool)),            this, SLOT(UpdateBehaviour()));
  connect(ui->interactionLookAt,    SIGNAL(toggled(bool)),            this, SLOT(UpdateBehaviour()));
  connect(ui->interactionTalkTo,    SIGNAL(toggled(bool)),            this, SLOT(UpdateBehaviour()));
  connect(ui->selectCharacter,      SIGNAL(clicked()),                this, SLOT(SelectCharacter()));
  connect(ui->selectItem,           SIGNAL(clicked()),                this, SLOT(SelectItem()));
  connect(ui->selectKey,            SIGNAL(clicked()),                this, SLOT(SelectKey()));
  connect(ui->selectScript,         SIGNAL(clicked()),                this, SLOT(SelectScript()));
  connect(ui->selectDialog,         SIGNAL(clicked()),                this, SLOT(SelectDialog()));

  // Render Particles
  connect(ui->particleEffectName,   SIGNAL(textChanged(QString)), this, SLOT(UpdateParticleEffect()));
  connect(ui->showParticleEffect,   SIGNAL(clicked()),            this, SLOT(RestartParticleEffect()));
  connect(ui->selectParticleEffect, SIGNAL(clicked()),            this, SLOT(SelectParticleEffect()));

  ui->actionsButton->setMenu(&action_menu);
  action_menu.addAction("Copy",  this, SIGNAL(CopyRequested()),  QKeySequence::Copy);
  action_menu.addAction("Paste", this, SIGNAL(PasteRequested()), QKeySequence::Paste);
}

WorldObjectWidget::~WorldObjectWidget()
{
  delete ui;
}

void WorldObjectWidget::LightCompile(void)
{
  if (selection_type == 3)
    world->CompileLight(selection.light, ColMask::Object | ColMask::DynObject);
}

void WorldObjectWidget::AddEnlightenedObject(void)
{
  if (selection_type == 3)
  {
    std::string name = QInputDialog::getText(this, "Chose an object", "Name").toStdString();
    auto        it = std::find(selection.light->enlightened_index.begin(), selection.light->enlightened_index.end(), name);

    if (it == selection.light->enlightened_index.end())
    {
      MapObject* object = world->GetObjectFromName(name);

      if (object)
      {
        ui->lightTargets->AddEnlightenedObject(name.c_str(), selection.light->priority, false);
        selection.light->enlightened_index.push_back(WorldLight::EnlightenedObjectSettings(name, selection.light->priority, false));
        LightCompile();
      }
    }
  }
}

void WorldObjectWidget::UpdateEnlightenedObject(void)
{
  if (selection_type == 3)
  {
    int            current_row        = ui->lightTargets->currentRow();
    unsigned short priority           = ui->lightTargets->GetPriority(current_row);
    bool           propagates         = ui->lightTargets->PropagatesToChildren(current_row);
    QString        name               = ui->lightTargets->Name(current_row);
    auto           enlightened_object = std::find(selection.light->enlightened_index.begin(),
                                                  selection.light->enlightened_index.end(),
                                                  name.toStdString());

    if (enlightened_object != selection.light->enlightened_index.end())
    {
      enlightened_object->priority           = priority;
      enlightened_object->inherited_property = propagates;
      LightCompile();
    }
    else
      cout << "No enlightened object by the name of " << name.toStdString() << endl;
  }
}

void WorldObjectWidget::DeleteEnlightenedObject(void)
{
  if (selection_type == 3)
  {
    QString name = ui->lightTargets->Name(ui->lightTargets->currentRow());
    auto           enlightened_object = std::find(selection.light->enlightened_index.begin(),
                                                  selection.light->enlightened_index.end(),
                                                  name.toStdString());

    ui->lightTargets->removeRow(ui->lightTargets->currentRow());
    if (enlightened_object != selection.light->enlightened_index.end())
      selection.light->enlightened_index.erase(enlightened_object);
    LightCompile();
  }
}

WorldLight* WorldObjectWidget::GetSelectedWorldLight(void) const
{
  return (selection_type == 3 ? selection.light : 0);
}

DynamicObject* WorldObjectWidget::GetSelectedDynamicObject(void) const
{
  return (selection_type == 2 ? selection.dynamic_object : 0);
}

MapObject* WorldObjectWidget::GetSelectedObject(void) const
{
  return (selection_type > 0 && selection_type < 3 ? selection.object : 0);
}

void WorldObjectWidget::SetDialogObject(DialogObject* dialog)
{
  this->dialog_object = dialog;
  connect(ui->objectAdvancedSettings, SIGNAL(clicked()), dialog, SLOT(open()));
}

void WorldObjectWidget::DeleteSelection()
{
  switch (selection_type)
  {
    case 1:
      world->DeleteMapObject(selection.object);
      break ;
    case 2:
      world->DeleteDynamicObject(selection.dynamic_object);
      break ;
    case 3:
      world->DeleteLight(selection.light->name);
      break ;
    case 4:
      particle_system_manager.remove_particlesystem(selection.particle_object->GetParticleSystem());
      world->DeleteParticleObject(selection.particle_object->GetName());
      break ;
    default:
      return ;
  }
  UnsetSelection();
  selection_type = 0;
}

void WorldObjectWidget::UnsetSelection()
{
  if (selection_type > 0 && selection_type < 3)
    selection.object->collider.node.hide();
  else if (selection_type == 3)
  {
    selection.light->collider.node.hide();
    selection.light->SetFrustumVisible(false);
    ui->showFrustum->setChecked(false);
  }
  else if (selection_type == 4)
    particle_system_manager.remove_particlesystem(selection.particle_object->GetParticleSystem());
  selection_type = 0;
  while (ui->tabWidget->count())
    ui->tabWidget->removeTab(0);
  ui->objectName->setText("No objects currently selected");
  ui->objectName->setEnabled(false);
}

void WorldObjectWidget::SetSelection(MapObject* object)
{
  UnsetSelection();
  InitializeMapObject(object);
  selection.object = object;
  selection_type   = 1;
}

void WorldObjectWidget::SetSelection(DynamicObject* object)
{
  UnsetSelection();
  InitializeMapObject(object);
  InitializeDynamicObject(object);
  selection.dynamic_object = object;
  selection_type           = 2;
  dialog_object->SetCurrentObject(object);
}

void WorldObjectWidget::SetSelection(ParticleObject* object)
{
  UnsetSelection();
  ui->particleEffectName->setText(object->GetParticleEffectName().c_str());
  ui->objectName->setText(object->GetName().c_str());
  selection.particle_object = object;
  selection_type            = 4;
  if (!(object->GetParticleSystem().is_null()))
    particle_system_manager.attach_particlesystem(object->GetParticleSystem());
  ui->tabWidget->addTab(ui->particleTab, "Render");
}

void WorldObjectWidget::RestartParticleEffect()
{
  UpdateParticleEffect();
}

void WorldObjectWidget::UpdateParticleEffect()
{
  if (selection_type == 4)
  {
    ParticleObject& object        = *selection.particle_object;
    NodePath        render_parent = world->rootParticleObjects;

    if (!object.GetParticleSystem().is_null())
    {
      render_parent = object.GetParticleSystem()->get_render_parent();
      particle_system_manager.remove_particlesystem(object.GetParticleSystem());
    }
    object.SetParticleEffect(ui->particleEffectName->text().toStdString());
    if (!object.GetParticleSystem().is_null())
    {
      object.GetParticleSystem()->set_render_parent(render_parent);
      particle_system_manager.attach_particlesystem(object.GetParticleSystem());
    }
  }
}

void WorldObjectWidget::SelectParticleEffect()
{
  QString   filter    = "Particle efects (*.json)";
  QString   base_path = QDir::currentPath() + "/data/particle-effects";
  QString   path = QFileDialog::getOpenFileName(this, "Select a particle effect", base_path, filter);
  QFileInfo info(path);
  QString   relative_path;

  if (!(info.exists()))
    return ;
  if (!(path.startsWith(base_path))) // Needs to be moved
  {
    if (!(QFile::copy(path, base_path + info.fileName())))
    {
      QMessageBox::warning(this, "Error", "Couldn't copy file to the project directory.");
      return ;
    }
    path = base_path + info.fileName();
  }
  relative_path = path.remove(0, base_path.length());
  ui->particleEffectName->setText(relative_path);
}

void WorldObjectWidget::SetSelection(WorldLight* light)
{
  UnsetSelection();
  InitializeGeometry(light->nodePath);
  InitializeCollider(light->collider);

  {
    LColor color = light->GetColor();
    ui->lightColorR->setValue(color.get_x());
    ui->lightColorG->setValue(color.get_y());
    ui->lightColorB->setValue(color.get_z());
  }

  ui->lightTargets->clear();
  std::for_each(light->enlightened_index.begin(), light->enlightened_index.end(), [this](const WorldLight::EnlightenedObjectSettings& settings)
  {
    ui->lightTargets->AddEnlightenedObject(settings.name.c_str(), settings.priority, settings.inherited_property);
  });

  ui->lightTypesList->setCurrentIndex((int)light->type);
  InitializeShadowCaster(light);
  InitializeLightAttenuation(light);
  ui->lightPriority->setValue(light->priority);
  ui->objectName->setEnabled(true);
  ui->objectName->setText(QString::fromStdString(light->name));
  ui->lightSetEnabled->setChecked(light->enabled);
  ui->lightSetDisabled->setChecked(!light->enabled);
  selection.light = light;
  selection_type  = 3;
  ui->tabWidget->addTab(ui->lightTab,        "Light");
  ui->tabWidget->addTab(ui->lightTargetsTab, "Light Targets");
}

void WorldObjectWidget::UpdateLightType(void)
{
  if (selection_type == 3)
  {
    WorldLight::Type type  = (WorldLight::Type)ui->lightTypesList->currentIndex();
    WorldLight*      light = selection.light;

    if (type != light->type)
    {
      cout << "Updating light type" << endl;
      std::string name = light->name;

      light->Destroy();
      light->type = type;
      light->Initialize();
      UpdateLightColor();
      UpdateLightAttenuation();
      InitializeLightAttenuation(light);
      InitializeShadowCaster(light);
    }
    else
      cout << "Light type is already " << (int)type << endl;
  }
}

void WorldObjectWidget::InitializeShadowCaster(WorldLight* light)
{
  if (light->type == WorldLight::Point || light->type == WorldLight::Spot || light->type == WorldLight::Directional)
  {
    ui->shadowCasting->show();
    ui->shadowFilmSize->setValue(light->shadow_settings.film_size);
    ui->shadowBufferSizeX->setValue(light->shadow_settings.buffer_size[0]);
    ui->shadowBufferSizeY->setValue(light->shadow_settings.buffer_size[1]);
    ui->shadowNear->setValue(light->shadow_settings.distance_near);
    ui->shadowFar->setValue(light->shadow_settings.distance_far);
    ui->showFrustum->setVisible(true);
  }
  else
  {
    ui->shadowCasting->hide();
    ui->showFrustum->setVisible(false);
  }
}

void WorldObjectWidget::UpdateShadowCaster()
{
  if (selection_type == 3)
  {
    WorldLight* light = selection.light;

    light->shadow_settings.film_size      = ui->shadowFilmSize->value();
    light->shadow_settings.buffer_size[0] = ui->shadowBufferSizeX->value();
    light->shadow_settings.buffer_size[1] = ui->shadowBufferSizeY->value();
    light->shadow_settings.distance_near  = ui->shadowNear->value();
    light->shadow_settings.distance_far   = ui->shadowFar->value();
    light->InitializeShadowCaster();
  }
}

void WorldObjectWidget::UpdateLightPriority()
{
  if (selection_type == 3)
    selection.light->priority = ui->lightPriority->value();
}

void WorldObjectWidget::InitializeLightAttenuation(WorldLight* light)
{
  if (light->type == WorldLight::Point || light->type == WorldLight::Spot)
  {
    ui->lightAttenuationA->setValue(light->GetAttenuation().get_x());
    ui->lightAttenuationB->setValue(light->GetAttenuation().get_y());
    ui->lightAttenuationC->setValue(light->GetAttenuation().get_z());
    ui->labelAttenuation->show();
    ui->lightAttenuationA->show();
    ui->lightAttenuationB->show();
    ui->lightAttenuationC->show();
  }
  else
  {
    ui->labelAttenuation->hide();
    ui->lightAttenuationA->hide();
    ui->lightAttenuationB->hide();
    ui->lightAttenuationC->hide();
  }
}

void WorldObjectWidget::UpdateLightAttenuation()
{
  if (selection_type == 3)
  {
      selection.light->SetAttenuation(ui->lightAttenuationA->value(),
                                      ui->lightAttenuationB->value(),
                                      ui->lightAttenuationC->value());
  }
}

void WorldObjectWidget::UpdateLightZoneSize()
{
}

void WorldObjectWidget::UpdateLightColor()
{
  if (selection_type == 3)
  {
    selection.light->SetColor(ui->lightColorR->value(),
                              ui->lightColorG->value(),
                              ui->lightColorB->value(),
                              0.f);
  }
}

void WorldObjectWidget::InitializeMapObject(MapObject* object)
{
  ui->objectName->setEnabled(true);
  ui->objectName->setText(QString::fromStdString(object->name));
  ui->inheritsFloor->setChecked(object->inherits_floor);
  InitializeGeometry(object->nodePath);
  InitializeCollider(object->collider);
  InitializeRender(object);
  ui->objectFloor->setValue(object->floor);
}

void WorldObjectWidget::InitializeDynamicObject(DynamicObject* object)
{
  ui->tabWidget->addTab(ui->behaviourTab, "Behaviour");
  ui->selectCurrentWaypoint->setEnabled(object->waypoint != 0);
  InitializeBehaviour(object);
}

void WorldObjectWidget::InitializeBehaviour(DynamicObject* object)
{
  ui->objectTypeList->setCurrentIndex(object->type);
  ui->character->setText(object->charsheet.c_str());
  ui->item->setText(object->charsheet.c_str());
  ui->script->setText(object->script.c_str());
  ui->dialog->setText(object->dialog.c_str());
  ui->interactionUse->setChecked      (object->interactions & Interactions::Use);
  ui->interactionUseObject->setChecked(object->interactions & Interactions::UseObject);
  ui->interactionUseSkill->setChecked (object->interactions & Interactions::UseSkill);
  ui->interactionUseSpell->setChecked (object->interactions & Interactions::UseSpell);
  ui->interactionTalkTo->setChecked   (object->interactions & Interactions::TalkTo);
  ui->interactionLookAt->setChecked   (object->interactions & Interactions::LookAt);
  ui->doorLocked->setChecked(object->locked);
}

void WorldObjectWidget::UpdateBehaviour()
{
  QList<QWidget*> key_widgets;       key_widgets << ui->key << ui->labelKey << ui->selectKey;
  QList<QWidget*> character_widgets; character_widgets << ui->labelCharacter << ui->character << ui->selectCharacter;
  QList<QWidget*> item_widgets;      item_widgets << ui->labelItem << ui->item << ui->selectItem;
  QList<QWidget*> inventory_widgets; inventory_widgets << ui->labelInventory << ui->inventory << ui->selectInventory;
  auto            show_widgets = [this](QList<QWidget*> widgets, bool show)
  { foreach (QWidget* widget, widgets) { widget->setVisible(show); } };

  show_widgets(key_widgets, false);
  show_widgets(character_widgets, false);
  show_widgets(item_widgets, false);
  show_widgets(inventory_widgets, true);
  switch (ui->objectTypeList->currentIndex())
  {
    case DynamicObject::Door:
      show_widgets(inventory_widgets, false);
    case DynamicObject::Locker:
      show_widgets(key_widgets, true);
    case DynamicObject::Other:
    case DynamicObject::Shelf:
      ui->dynamicObject->show();
      break ;
    case DynamicObject::Item:
      show_widgets(item_widgets, true);
      ui->dynamicObject->hide();
      break ;
    case DynamicObject::Character:
      show_widgets(character_widgets, true);
      ui->dynamicObject->hide();
      break ;
  }
  if (selection_type == 2)
  {
    DynamicObject* object = selection.dynamic_object;

    object->type = (DynamicObject::Type)ui->objectTypeList->currentIndex();
    switch (object->type)
    {
      case DynamicObject::Character:
        object->charsheet    = ui->character->text().toStdString();
        object->interactions = 0;
        break ;
      case DynamicObject::Item:
        object->charsheet    = ui->item->text().toStdString();
        object->interactions = Interactions::Use;
        break ;
      default:
        object->key          = ui->key->text().toStdString();
        object->locked       = ui->doorLocked->isChecked();
        object->script       = ui->script->text().toStdString();
        object->dialog       = ui->dialog->text().toStdString();
        object->interactions = (ui->interactionUse->isChecked() ? Interactions::Use : 0) +
            (ui->interactionUseObject->isChecked() ? Interactions::UseObject : 0) +
            (ui->interactionUseSkill->isChecked()  ? Interactions::UseSkill  : 0) +
            (ui->interactionUseSpell->isChecked()  ? Interactions::UseSpell  : 0) +
            (ui->interactionTalkTo->isChecked()    ? Interactions::TalkTo    : 0) +
            (ui->interactionLookAt->isChecked()    ? Interactions::LookAt    : 0);
        break ;
    }
  }
}

void WorldObjectWidget::SelectCharacter()
{
  SelectableResource::Charsheets().SelectResource([this](QString name)
  {
    DataTree* data_tree = DataTree::Factory::JSON("data/charsheets/" + name.toStdString() + ".json");

    if (data_tree)
    {
      {
        Data data(data_tree);

        ui->character->setText(name);
        if (data["Appearance"].NotNil())
        {
          ui->objectModel->setText  (data["Appearance"]["model"].Value().c_str());
          ui->objectTexture->setText(data["Appearance"]["texture"].Value().c_str());
        }
      }
      delete data_tree;
    }
  });
}

void WorldObjectWidget::SelectItem()
{
  SelectableResource::Items().SelectResource([this](QString name)
  {
    DataTree* data_tree = DataTree::Factory::JSON("data/objects.json");

    if (data_tree)
    {
      {
        Data data = Data(data_tree)[name.toStdString()];

        if (data.NotNil())
        {
          std::string model   = data["model"].Value();
          std::string texture = data["texture"].Value();

          if (model != "")
            ui->objectModel->setText(model.c_str());
          ui->objectTexture->setText(texture.c_str());
          ui->objectScaleX->setValue(data["scale"].Or(1.f));
          ui->objectScaleY->setValue(data["scale"].Or(1.f));
          ui->objectScaleZ->setValue(data["scale"].Or(1.f));
          ui->item->setText(name);
        }
      }
      delete data_tree;
    }
    // Update Render according to the selected item
  });
}

void WorldObjectWidget::SelectKey()
{
  SelectableResource::Items().SelectResource([this](QString name)
  {
    ui->key->setText(name);
  });
}

void WorldObjectWidget::SelectDialog()
{
  SelectableResource::Dialogs().SelectResource([this](QString name)
  {
    ui->dialog->setText(name);
  });
}

void WorldObjectWidget::SelectScript()
{
  SelectableResource::AIs().SelectResource([this](QString name)
  {
    ui->script->setText(name);
  });
}

void WorldObjectWidget::InitializeRender(MapObject* object)
{
  ui->tabWidget->addTab(ui->renderTab, "Render");
  ui->objectModel->setText(QString::fromStdString(object->strModel));
  ui->useTexture->setChecked(object->use_texture);
  if (object->use_texture)
    ui->objectTexture->setText(QString::fromStdString(object->strTexture));
  ui->useColor->setChecked(object->use_color);
  ui->useOpacity->setChecked(object->use_opacity);
  if (object->use_color)
  {
    ui->colorRed->setValue(object->base_color.get_x());
    ui->colorGreen->setValue(object->base_color.get_y());
    ui->colorBlue->setValue(object->base_color.get_z());
  }
  if (object->use_opacity)
    ui->opacity->setValue(object->base_color.get_w());
}

void WorldObjectWidget::InitializeGeometry(NodePath nodePath)
{
  ui->tabWidget->addTab(ui->geometryTab, "Geometry");
  ui->objectPosX->setValue(nodePath.get_x());
  ui->objectPosY->setValue(nodePath.get_y());
  ui->objectPosZ->setValue(nodePath.get_z());
  ui->objectRotationX->setValue(nodePath.get_hpr().get_x());
  ui->objectRotationY->setValue(nodePath.get_hpr().get_y());
  ui->objectRotationZ->setValue(nodePath.get_hpr().get_z());
  ui->objectScaleX->setValue(nodePath.get_scale().get_x());
  ui->objectScaleY->setValue(nodePath.get_scale().get_y());
  ui->objectScaleZ->setValue(nodePath.get_scale().get_z());
}

void WorldObjectWidget::InitializeCollider(Collider& collider)
{
  NodePath node = collider.node;

  ui->tabWidget->addTab(ui->colliderTab, "Collider");
  ui->collider_type->setCurrentIndex((int)collider.type);
  ui->collider_position->setEnabled(collider.type != Collider::NONE);
  if (collider.type != Collider::NONE)
  {
    ui->collider_pos_x->setValue(node.get_x());
    ui->collider_pos_y->setValue(node.get_y());
    ui->collider_pos_z->setValue(node.get_z());
    ui->collider_hpr_x->setValue(node.get_hpr().get_x());
    ui->collider_hpr_y->setValue(node.get_hpr().get_y());
    ui->collider_hpr_z->setValue(node.get_hpr().get_z());
    ui->collider_scale_x->setValue(node.get_scale().get_x());
    ui->collider_scale_y->setValue(node.get_scale().get_y());
    ui->collider_scale_z->setValue(node.get_scale().get_z());
    if (ui->displayColliders->isChecked())
      node.show();
  }
}

void WorldObjectWidget::UpdateName(QString name)
{
  if (selection_type != 0)
  {
    if (selection_type < 3)
    {
      MapObject* object = selection.object;

      RenameObject(QString::fromStdString(object->name), name);
      object->name = name.toStdString();
      object->nodePath.set_name(object->name);
    }
    else if (selection_type == 3)
    {
      RenameObject(QString::fromStdString(selection.light->name), name);
      selection.light->name = name.toStdString();
    }
    else if (selection_type == 4)
    {
      RenameObject(QString::fromStdString(selection.particle_object->GetName()), name);
      selection.particle_object->SetName(name.toStdString());
    }
  }
}

void WorldObjectWidget::UpdateColliderGeometry()
{
  Collider* collider = 0;

  if (selection_type > 0 && selection_type < 3)
    collider = &selection.object->collider;
  else if (selection_type == 3)
    collider = &selection.light->collider;
  if (collider)
  {
    LPoint3f   position(ui->collider_pos_x->value(), ui->collider_pos_y->value(), ui->collider_pos_z->value());
    LPoint3f   hpr     (ui->collider_hpr_x->value(), ui->collider_hpr_y->value(), ui->collider_hpr_z->value());
    LPoint3f   scale   (ui->collider_scale_x->value(), ui->collider_scale_y->value(), ui->collider_scale_z->value());

    collider->node.set_pos(position);
    collider->node.set_hpr(hpr);
    collider->node.set_scale(scale);
    ui->displayColliders->setChecked(true);
  }
  UpdateColliderDisplay();
}

void WorldObjectWidget::UpdateColliderType()
{
  Collider* collider = 0;
  NodePath  parent;

  if (selection_type > 0 && selection_type < 3)
  {
    collider = &selection.object->collider;
    parent   = selection.object->nodePath;
  }
  else if (selection_type == 3)
  {
    collider = &selection.light->collider;
    parent   = selection.light->symbol;
  }
  if (collider)
  {
    if (collider->type != Collider::NONE)
      collider->node.remove_node();
    collider->type = (Collider::Type)ui->collider_type->currentIndex();
    ui->collider_position->setEnabled(collider->type != Collider::NONE);
    collider->parent = parent;
    collider->InitializeCollider(collider->type, LPoint3f(0, 0, 0), LPoint3f(1, 1, 1), LPoint3f(0, 0, 0));
    if (selection_type > 0 && selection_type < 3)
    {
      LPoint3f scale = NodePathSize(selection.object->render) / 2;
      LPoint3f min_point, max_point;

      selection.object->render.calc_tight_bounds(min_point, max_point);
      ui->collider_pos_x->setValue((std::abs(max_point.get_x()) - std::abs(min_point.get_x())) / 2 * (max_point.get_x() < 0 ? -1 : 1));
      ui->collider_pos_y->setValue((std::abs(max_point.get_y()) - std::abs(min_point.get_y())) / 2 * (max_point.get_y() < 0 ? -1 : 1));
      ui->collider_pos_z->setValue((std::abs(max_point.get_z()) - std::abs(min_point.get_z())) / 2 * (max_point.get_z() < 0 ? -1 : 1));
      ui->collider_scale_x->setValue(scale.get_x());
      ui->collider_scale_y->setValue(scale.get_y());
      ui->collider_scale_z->setValue(scale.get_z());
    }
    UpdateColliderGeometry();
  }
}

void WorldObjectWidget::UpdateColliderDisplay()
{
  Collider* collider = 0;

  if (selection_type > 0 && selection_type < 3)
    collider = &selection.object->collider;
  else if (selection_type == 3)
    collider = &selection.light->collider;
  if (collider)
  {
    if (ui->displayColliders->isChecked())
      collider->node.show();
    else
      collider->node.hide();
  }
}

void WorldObjectWidget::UpdateGeometry()
{
  NodePath nodePath;

  if (selection_type > 0 && selection_type < 3)
    nodePath = selection.object->nodePath;
  else if (selection_type == 3)
    nodePath = selection.light->nodePath;
  if (!(nodePath.is_empty()))
  {
    nodePath.set_x(ui->objectPosX->value());
    nodePath.set_y(ui->objectPosY->value());
    nodePath.set_z(ui->objectPosZ->value());
    nodePath.set_hpr(ui->objectRotationX->value(),
                     ui->objectRotationY->value(),
                     ui->objectRotationZ->value());
    nodePath.set_scale(ui->objectScaleX->value(),
                       ui->objectScaleY->value(),
                       ui->objectScaleZ->value());
  }
}

void WorldObjectWidget::UpdateFloor()
{
  if (selection_type > 0 && selection_type < 3)
  {
    selection.object->inherits_floor = ui->inheritsFloor->isChecked();
    selection.object->floor          = ui->objectFloor->value();
  }
  ui->objectFloor->setEnabled(!ui->inheritsFloor->isChecked());
}

void WorldObjectWidget::LightShowFrustum(bool toggled)
{
  if (selection_type == 3)
    selection.light->SetFrustumVisible(toggled);
}

void WorldObjectWidget::LightSetDisabled(bool disabled)
{
  if (selection_type == 3)
  {
    if (disabled)
      ui->lightSetEnabled->setChecked(false);
  }
}

void WorldObjectWidget::LightSetEnabled(bool enabled)
{
  if (selection_type == 3)
  {
    if (enabled)
      ui->lightSetDisabled->setChecked(false);
    selection.light->SetEnabled(enabled);
  }
}

void WorldObjectWidget::PickModel()
{
  QString filter    = "Panda3D Models (*.egg *.bam *.egg.pz *.bam.pz *.obj)";
  QString base_path = QDir::currentPath() + "/models/";
  QString path      = QFileDialog::getOpenFileName(this, "Select a model", base_path, filter);
  QFileInfo info(path);
  QString   relative_path;

  if (!(info.exists()))
    return ;
  if (!(path.startsWith(base_path))) // Needs to be moved
  {
    if (!(QFile::copy(path, base_path + info.fileName())))
    {
      QMessageBox::warning(this, "Error", "Couldn't copy file to the project directory.");
      return ;
    }
    path = base_path + info.fileName();
  }
  relative_path = path.remove(0, base_path.length());
  ui->objectModel->setText(relative_path);
}

void WorldObjectWidget::PickTexture()
{
  QString   filter    = "Images (*.png *.jpg *.bmp)";
  QString   base_path = QDir::currentPath() + "/textures/";
  QString   path = QFileDialog::getOpenFileName(this, "Select a texture", base_path, filter);
  QFileInfo info(path);
  QString   relative_path;

  if (!(info.exists()))
    return ;
  if (!(path.startsWith(base_path))) // Needs to be moved
  {
    if (!(QFile::copy(path, base_path + info.fileName())))
    {
      QMessageBox::warning(this, "Error", "Couldn't copy file to the project directory.");
      return ;
    }
    path = base_path + info.fileName();
  }
  relative_path = path.remove(0, base_path.length());
  ui->objectTexture->setText(relative_path);
}

void WorldObjectWidget::UpdateRender()
{
  if (selection_type > 0 && selection_type < 3)
  {
    MapObject*  object       = selection.object;
    std::string new_model    = ui->objectModel->text().toStdString();
    std::string new_texture  = ui->objectTexture->text().toStdString();

    if (object->strModel != new_model)
      selection.object->SetModel(new_model);
    if (object->strTexture != new_texture)
      selection.object->SetTexture(new_texture);
    object->use_color   = ui->useColor->isChecked();
    object->use_opacity = ui->useOpacity->isChecked();
    object->use_texture = ui->useTexture->isChecked();
    ui->colorRed->setEnabled(object->use_color);
    ui->colorGreen->setEnabled(object->use_color);
    ui->colorBlue->setEnabled(object->use_color);
    ui->opacity->setEnabled(object->use_opacity);
    ui->selectTexture->setEnabled(object->use_texture);
    if (!object->use_texture)
      object->render.set_texture_off();
    object->nodePath.set_transparency(object->use_opacity ? TransparencyAttrib::M_alpha : TransparencyAttrib::M_none);
    object->base_color.set_w(object->use_opacity ? ui->opacity->value() : 1.f);
    object->base_color.set_x(ui->colorRed->value());
    object->base_color.set_y(ui->colorGreen->value());
    object->base_color.set_z(ui->colorBlue->value());
    if (object->use_color || object->use_opacity)
      object->render.set_color_scale(object->base_color);
  }
}

void WorldObjectWidget::FocusCurrentObject()
{
  if (selection_type > 0 && selection_type < 3)
  {
    NodePath selected_nodepath = selection.object->nodePath;
    auto     functor = [this, selected_nodepath](MapObject& object)
    {
      NodePath nodePath = object.nodePath;

      if (nodePath != selected_nodepath &&
          !(selected_nodepath.is_ancestor_of(nodePath)))
        object.render.hide();
      else
        object.render.show();
    };

    for_each(world->objects.begin(), world->objects.end(), functor);
    for_each(world->dynamicObjects.begin(), world->dynamicObjects.end(), functor);
  }
}

void WorldObjectWidget::ToogleCurrentObject()
{
  if (selection_type > 0 && selection_type < 3)
  {
    NodePath selected_nodepath = selection.object->nodePath;
    bool     was_visible       = !selection.object->render.is_hidden();
    auto     functor = [this, selected_nodepath, was_visible](MapObject& object)
    {
      NodePath nodePath = object.nodePath;

      if (nodePath == selected_nodepath ||
          selected_nodepath.is_ancestor_of(nodePath))
      {
          if (was_visible)
            object.render.hide();
          else
            object.render.show();
      }
    };

    for_each(world->objects.begin(), world->objects.end(), functor);
    for_each(world->dynamicObjects.begin(), world->dynamicObjects.end(), functor);
  }
}

void WorldObjectWidget::SetCurrentWaypoint()
{
  if (selection_type == 2)
  {
    DynamicObject* object = selection.dynamic_object;

    WaypointSetOnObjectRequested(object);
    ui->selectCurrentWaypoint->setEnabled(object->waypoint != 0);
  }
}

void WorldObjectWidget::SelectCurrentWaypoint()
{
  if (selection_type == 2)
    SelectWaypointFromObject(selection.dynamic_object);
}

/*
 * Copy Paste Bullshit
 */
void WorldObjectWidget::Copy(Utils::Packet& packet, MapObject* object)
{
  auto waypoint_backup = object->waypoints;

  object->waypoints.clear();
  packet << 1;
  packet << *object;
  CopyChildren(packet, object->name);
  object->waypoints = waypoint_backup;
}

void WorldObjectWidget::Copy(Utils::Packet& packet, DynamicObject* object)
{
  packet << 2;
  packet << *object;
  CopyChildren(packet, object->name);
}

void WorldObjectWidget::Copy(Utils::Packet& packet, WorldLight* light)
{
  packet << 3;
  packet << *light;
}

void WorldObjectWidget::Copy(Utils::Packet& packet, ParticleObject* particle_object)
{
  packet << 4;
  packet << *particle_object;
}

void WorldObjectWidget::CopyChildren(Utils::Packet& packet, const std::string& parent_name)
{
  for_each(world->objects.begin(), world->objects.end(), [this, &packet, parent_name](MapObject& object)
  {
    if (object.parent == parent_name)
      Copy(packet, &object);
  });
  for_each(world->dynamicObjects.begin(), world->dynamicObjects.end(), [this, &packet, parent_name](DynamicObject& object)
  {
    if (object.parent == parent_name)
      Copy(packet, &object);
  });
  for_each(world->lights.begin(), world->lights.end(), [this, &packet, parent_name](WorldLight& light)
  {
    if (!light.parent.is_empty() && light.parent.get_name() == parent_name)
      Copy(packet, &light);
  });
  for_each(world->particleObjects.begin(), world->particleObjects.end(), [this, &packet, parent_name](ParticleObject& particle_object)
  {
    if (particle_object.GetParentName() == parent_name)
      Copy(packet, &particle_object);
  });
}

Utils::Packet* WorldObjectWidget::Copy()
{
  Utils::Packet* packet = new Utils::Packet;

  switch (selection_type)
  {
    case 1:
      Copy(*packet, selection.object);
      break ;
    case 2:
      Copy(*packet, selection.dynamic_object);
      break ;
    case 3:
      Copy(*packet, selection.light);
      break ;
    case 4:
      Copy(*packet, selection.particle_object);
      break ;
    default:
      delete packet;
      return (0);
  }
  return (packet);
}

void WorldObjectWidget::Paste(Utils::Packet& packet)
{
  Utils::Packet dup(packet.raw(), packet.size());

  clipboard_name_map.clear();
  while (dup.isNextObjectOfType<int>())
  {
    int object_type;

    dup >> object_type;
    switch (object_type)
    {
    case 1:
      PasteObject(dup);
      break ;
    case 2:
      PasteDynamicObject(dup);
      break ;
    case 3:
      PasteWorldLight(dup);
      break ;
    case 4:
      PasteParticleEffect(dup);
      break ;
    }
  }
  ReparentPastedObjects();
}

QString WorldObjectWidget::GenerateNewName(QString base_name)
{
  unsigned int ii = 0;
  QString name;
  QRegExp regexp("#[0-9]+$");

  if (base_name.contains(regexp))
    base_name = base_name.replace(regexp, "");
  do
  {
    ++ii;
    name = base_name + '#' + QString::number(ii);
  } while ((world->GetMapObjectFromName(name.toStdString()) != 0) ||
           (world->GetDynamicObjectFromName(name.toStdString()) != 0) ||
           (world->GetLightByName(name.toStdString()) != 0));
  return (name);
}

void WorldObjectWidget::PasteObject(Utils::Packet& packet)
{
  MapObject object;
  QString   name;

  packet >> object;
  name        = GenerateNewName(QString::fromStdString(object.name));
  clipboard_name_map.insert(object.name.c_str(), name);
  object.name = name.toStdString();
  object.nodePath.set_name(object.name);
  world->objects.push_back(object);
}

void WorldObjectWidget::PasteDynamicObject(Utils::Packet& packet)
{
  DynamicObject object;
  QString   name;

  packet >> object;
  name        = GenerateNewName(QString::fromStdString(object.name));
  clipboard_name_map.insert(object.name.c_str(), name);
  object.name = name.toStdString();
  object.nodePath.set_name(object.name);
  world->dynamicObjects.push_back(object);
}

void WorldObjectWidget::PasteWorldLight(Utils::Packet& packet)
{
  WorldLight object;
  QString   name;

  packet >> object;
  name        = GenerateNewName(QString::fromStdString(object.name));
  clipboard_name_map.insert(object.name.c_str(), name);
  object.name = name.toStdString();
  world->lights.push_back(object);
}

void WorldObjectWidget::PasteParticleEffect(Utils::Packet& packet)
{
  ParticleObject object;
  QString        old_name = object.GetName().c_str();

  packet >> object;
  object.SetName(GenerateNewName(QString::fromStdString(object.GetName())).toStdString());
  clipboard_name_map.insert(old_name, object.GetName().c_str());
  world->particleObjects.push_back(object);
}

void WorldObjectWidget::ReparentPastedObjects()
{
  foreach (QString object_name, clipboard_name_map)
  {
    MapObject*      current_object = world->GetObjectFromName(object_name.toStdString());
    WorldLight*     current_light  = world->GetLightByName(object_name.toStdString());
    ParticleObject* current_effect = world->GetParticleObjectByName(object_name.toStdString());
    std::string     parent_name;

    if (current_object)
      parent_name = current_object->parent;
    else if (current_light)
      parent_name = current_light->parent.get_name();
    else if (current_effect)
      parent_name = current_effect->GetParentName();
    auto parent_it = clipboard_name_map.find(QString::fromStdString(parent_name));

    if (parent_it != clipboard_name_map.end())
    {
      MapObject*     parent_map_object = world->GetMapObjectFromName(parent_it->toStdString());
      DynamicObject* parent_dyn_object = world->GetDynamicObjectFromName(parent_it->toStdString());

      if (current_object)
      {
        if (parent_dyn_object)
          current_object->ReparentTo(parent_dyn_object);
        else if (parent_map_object)
          current_object->ReparentTo(parent_map_object);
      }
      else if (current_light)
      {
        if (parent_dyn_object)
          current_light->ReparentTo(parent_dyn_object);
        else if (parent_map_object)
          current_light->ReparentTo(parent_map_object);
      }
      else if (current_effect)
      {
        if (parent_dyn_object)
          current_effect->ReparentTo(parent_dyn_object);
        else if (parent_map_object)
          current_effect->ReparentTo(parent_map_object);
      }
    }
  }
}
