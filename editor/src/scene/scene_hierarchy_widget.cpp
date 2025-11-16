//
// Created by gorev on 25.10.2025.
//


#include "scene_hierarchy_widget.h"

#include "Editor.h"
#include "Engine.h"
#include "EntityDelegate.h"
#include "FileSystemUtils.h"
#include "InspectorFabric.h"
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

    connect(this->selectionModel(), &QItemSelectionModel::selectionChanged, this,
            &scene_hierarchy_widget::OnSelectionChanged);

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

    const auto entityEvent = static_cast<const EntityCreatedEvent *>(event.get());
    QModelIndex newIndex = GetSceneModel().AddNewEntity(entityEvent->GetEntity());

    if (newIndex.isValid())
    {
        if (newIndex.isValid()) expand(newIndex);

        if (!entityEvent->IsSceneChanged())
        {
            setCurrentIndex(newIndex);
            edit(newIndex);
        }
    }
}


void scene_hierarchy_widget::OnEntityDestroyed(const Blainn::SceneEventPointer &event)
{
    using namespace Blainn;

    const auto entityEvent = static_cast<const EntityDestroyedEvent *>(event.get());
    Entity destroyedEntity = entityEvent->GetEntity();

    SceneItemModel *model = qobject_cast<SceneItemModel *>(this->model());
    if (!model)
    {
        return;
    }

    QModelIndex nodeIndex = SceneItemModel::FindIndexByEntity(model, destroyedEntity);

    if (!nodeIndex.isValid())
    {
        BF_ERROR("Index not found for entity!");
        return;
    }

    if (nodeIndex.row() >= model->rowCount(nodeIndex.parent()))
    {
        BF_ERROR("Row index out of bounds!");
        return;
    }

    model->removeRow(nodeIndex.row(), nodeIndex.parent());
}


void scene_hierarchy_widget::OnItemDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight,
                                               const QVector<int> &roles)
{
    if (!topLeft.isValid()) return;


    QString newName = topLeft.data(Qt::DisplayRole).toString();
    BF_DEBUG("Item renamed to: {0}", ToString(newName))

    if (EntityNode *node = SceneItemModel::GetNodeFromIndex(topLeft))
    {
        node->SetName(newName);
    }
}


void scene_hierarchy_widget::OnSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    QModelIndexList selectedIndexes = selected.indexes();

    if (selectedIndexes.isEmpty()) return;

    auto entity = SceneItemModel::GetNodeFromIndex(selectedIndexes.first());

    InspectorFabric fabric;
    EntityInspectorData data;
    data.tag = entity->GetName();
    data.node = entity;

    auto inspector = fabric.GetEntityInspector(data);
    Blainn::Editor::GetInstance().GetInspector().SetItem(inspector);
}


void scene_hierarchy_widget::keyPressEvent(QKeyEvent *event)
{
    const auto currentIndex = this->currentIndex();

    if (currentIndex.isValid())
    {
        QKeySequence keySequence = QKeySequence(event->key() | event->modifiers());

        if (keySequence == m_addToSceneMenu->GetDeleteKey())
        {
            m_addToSceneMenu->DeleteEntity(currentIndex);
            event->accept();
            return;
        }

        if (keySequence == m_addToSceneMenu->GetRenameKey())
        {
            m_addToSceneMenu->RenameEntity(currentIndex);
            event->accept();
            return;
        }
    }

    QTreeView::keyPressEvent(event);
}


} // namespace editor
