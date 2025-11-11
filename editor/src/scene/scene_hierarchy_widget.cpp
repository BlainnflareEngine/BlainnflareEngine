//
// Created by gorev on 25.10.2025.
//


#include "scene_hierarchy_widget.h"

#include "Engine.h"
#include "EntityDelegate.h"
#include "FileSystemUtils.h"
#include "SceneItemModel.h"
#include "context-menu/AddToSceneContextMenu.h"
#include "ui_scene_hierarchy_widget.h"

namespace editor
{
scene_hierarchy_widget::scene_hierarchy_widget(QWidget *parent)
    : QTreeView(parent)
    , ui(new Ui::scene_hierarchy_widget)
{
    ui->setupUi(this);

    m_sceneModel = new SceneItemModel(this);
    setModel(m_sceneModel);
    setHeaderHidden(true);
    setSelectionMode(SingleSelection);
    setContextMenuPolicy(Qt::CustomContextMenu);
    setItemDelegate(new EntityDelegate(this));

    m_addToSceneMenu = new AddToSceneContextMenu(*this, this);

    connect(this, &QTreeView::customContextMenuRequested, m_addToSceneMenu, &AddToSceneContextMenu::OnContextMenu);

    connect(m_sceneModel, &QAbstractItemModel::dataChanged, this, &scene_hierarchy_widget::OnItemDataChanged);


    Blainn::Scene::AddEventListener(Blainn::SceneEventType::EntityCreated,
                                    [this](const Blainn::SceneEventPointer &event) { this->OnEntityCreated(event); });
    Blainn::Scene::AddEventListener(Blainn::SceneEventType::EntityDestroyed,
                                    [this](const Blainn::SceneEventPointer &event) { this->OnEntityDestroyed(event); });
}

scene_hierarchy_widget::~scene_hierarchy_widget()
{
    delete ui;
}


void scene_hierarchy_widget::OpenContextMenu(const QPoint &position)
{
    m_addToSceneMenu->OpenMenu(position);
}


SceneItemModel &scene_hierarchy_widget::GetSceneModel() const
{
    return *m_sceneModel;
}


void scene_hierarchy_widget::OnEntityCreated(const Blainn::SceneEventPointer &event)
{
    using namespace Blainn;
    BF_DEBUG("OnEntityCreated");

    // TODO: mb use delegate for this
    const EntityCreatedEvent *entityEvent = static_cast<const EntityCreatedEvent *>(event.get());
    auto newIndex = GetSceneModel().AddNewEntity(entityEvent->GetEntity());

    if (newIndex.isValid())
    {
        setCurrentIndex(newIndex);

        if (newIndex.isValid())
        {
            expand(newIndex);
        }

        edit(newIndex);
    }
}


void scene_hierarchy_widget::OnEntityDestroyed(const Blainn::SceneEventPointer &event)
{
    BF_DEBUG("OnEntityDestroyed");
}


void scene_hierarchy_widget::OnItemDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight,
                                               const QVector<int> &roles)
{
    if (!topLeft.isValid()) return;


    QString newName = topLeft.data(Qt::DisplayRole).toString();
    BF_DEBUG("Item renamed to: {0}", ToString(newName))

    SceneItemModel *sceneModel = qobject_cast<SceneItemModel *>(model());
    if (!sceneModel) return;

    if (EntityNode *node = sceneModel->GetNodeFromIndex(topLeft))
    {
        node->SetName(newName);

        Blainn::Entity entity = node->GetEntity();
        if (entity && entity.HasComponent<Blainn::TagComponent>())
        {
            auto &tagComponent = entity.GetComponent<Blainn::TagComponent>();
            tagComponent.Tag = ToEASTLString(newName);
        }

        BF_DEBUG("Entity {0} renamed to: {1}", entity.GetUUID().str(), ToString(newName));
    }
}


} // namespace editor
