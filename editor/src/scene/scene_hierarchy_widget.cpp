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
#include "context-menu/SceneContextMenu.h"
#include "ui_scene_hierarchy_widget.h"

#include <QMimeData>

namespace editor
{
scene_hierarchy_widget::scene_hierarchy_widget(QWidget *parent)
    : QTreeView(parent)
    , ui(new Ui::scene_hierarchy_widget)
{
    m_sceneMeta = eastl::make_unique<SceneMeta>("");

    ui->setupUi(this);

    setSortingEnabled(false);

    m_sceneModel = new SceneItemModel(this);
    setModel(m_sceneModel);
    setHeaderHidden(true);
    setSelectionMode(SingleSelection);
    setContextMenuPolicy(Qt::CustomContextMenu);
    setItemDelegate(new EntityDelegate(this));
    setDragEnabled(true);
    setAcceptDrops(true);
    setDropIndicatorShown(true);
    setDragDropMode(InternalMove);

    m_addToSceneMenu = new SceneContextMenu(*this, this);

    connect(this, &QTreeView::customContextMenuRequested, m_addToSceneMenu, &SceneContextMenu::OnContextMenu);

    connect(m_sceneModel, &QAbstractItemModel::dataChanged, this, &scene_hierarchy_widget::OnItemDataChanged);

    connect(this->selectionModel(), &QItemSelectionModel::selectionChanged, this,
            &scene_hierarchy_widget::OnSelectionChanged);

    m_sceneEvents.emplace_back(Blainn::Scene::AddEventListener(Blainn::SceneEventType::EntityCreated,
                                                               [this](const Blainn::SceneEventPointer &event)
                                                               {
                                                                   BLAINN_PROFILE_SCOPE(QT_OnEntityCreated);
                                                                   this->OnEntityCreated(event);
                                                               }),
                               Blainn::SceneEventType::EntityCreated);
    m_sceneEvents.emplace_back(Blainn::Scene::AddEventListener(Blainn::SceneEventType::EntityDestroyed,
                                                               [this](const Blainn::SceneEventPointer &event)
                                                               {
                                                                   BLAINN_PROFILE_SCOPE(QT_OnEntityDestroyed);
                                                                   this->OnEntityDestroyed(event);
                                                               }),
                               Blainn::SceneEventType::EntityDestroyed);
    m_sceneEvents.emplace_back(Blainn::Scene::AddEventListener(Blainn::SceneEventType::SceneChanged,
                                                               [this](const Blainn::SceneEventPointer &event)
                                                               {
                                                                   BLAINN_PROFILE_SCOPE(QT_OnSceneChanged);
                                                                   this->OnSceneChanged(event);
                                                               }),
                               Blainn::SceneEventType::SceneChanged);


    m_selectionHandle = Blainn::Engine::GetSelectionManager().CallbackList.append(
        [this](Blainn::uuid id)
        {
            BLAINN_PROFILE_SCOPE(QtSceneWidgetPickingCallback);
            QMetaObject::invokeMethod(this, [this, id]() { ChangeSelection(id); }, Qt::QueuedConnection);
        });
}

scene_hierarchy_widget::~scene_hierarchy_widget()
{
    for (auto &[event, type] : m_sceneEvents)
        Blainn::Scene::RemoveEventListener(type, event);

    Blainn::Engine::GetSelectionManager().CallbackList.remove(m_selectionHandle);

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


void scene_hierarchy_widget::CreateEntityInHierarchy(Blainn::Entity &entity, const bool bSceneChanged,
                                                     bool bCreatedInEditor)
{
    Blainn::Entity parent = entity.GetParent();
    QModelIndex newIndex;

    if (parent.IsValid())
    {
        newIndex =
            GetSceneModel().AddNewEntity(entity, SceneItemModel::FindIndexByEntity(m_sceneModel, parent.GetUUID()));
    }
    else
    {
        newIndex = GetSceneModel().AddNewEntity(entity);
    }

    if (newIndex.isValid())
    {
        if (newIndex.isValid()) expand(newIndex);

        if (!bSceneChanged && bCreatedInEditor)
        {
            setCurrentIndex(newIndex);
            edit(newIndex);
        }
    }

    if (m_sceneMeta && !bSceneChanged)
    {
        int newPosition = newIndex.row();
        m_sceneMeta->AddEntity(entity, newPosition);
    }

    // TODO: meta is not working now)
    // m_sceneModel->SortAccordingToMeta(m_sceneMeta);
}


void scene_hierarchy_widget::CreateEntityInHierarchy(Blainn::Entity &&entity, bool bSceneChanged, bool bCreatedInEditor)
{
    Blainn::Entity parent = entity.GetParent();
    QModelIndex newIndex;

    if (parent.IsValid())
    {
        newIndex =
            GetSceneModel().AddNewEntity(entity, SceneItemModel::FindIndexByEntity(m_sceneModel, parent.GetUUID()));
    }
    else
    {
        newIndex = GetSceneModel().AddNewEntity(entity);
    }

    if (newIndex.isValid())
    {
        if (newIndex.isValid()) expand(newIndex);

        if (!bSceneChanged && bCreatedInEditor)
        {
            setCurrentIndex(newIndex);
            edit(newIndex);
        }
    }

    if (m_sceneMeta && !bSceneChanged)
    {
        int newPosition = newIndex.row();
        m_sceneMeta->AddEntity(entity, newPosition);
    }
}


void scene_hierarchy_widget::OnEntityCreated(const Blainn::SceneEventPointer &event)
{
    using namespace Blainn;

    const auto entityEvent = static_cast<const EntityCreatedEvent *>(event.get());

    //if (entityEvent->IsSceneChanged()) return;
    if (!entityEvent->GetEntity().IsValid()) return;

    CreateEntityInHierarchy(entityEvent->GetEntity(), entityEvent->IsSceneChanged(), entityEvent->CreatedByEditor());
}


void scene_hierarchy_widget::OnEntityDestroyed(const Blainn::SceneEventPointer &event)
{
    using namespace Blainn;

    const auto entityEvent = static_cast<const EntityDestroyedEvent *>(event.get());
    if (entityEvent->IsSceneChanged()) return;
    Entity destroyedEntity = entityEvent->GetEntity();

    SceneItemModel *model = qobject_cast<SceneItemModel *>(this->model());
    if (!model)
    {
        return;
    }

    QModelIndex nodeIndex = SceneItemModel::FindIndexByEntity(model, entityEvent->GetUUID());

    if (!nodeIndex.isValid())
    {
        return;
    }

    if (nodeIndex.row() >= model->rowCount(nodeIndex.parent()))
    {
        return;
    }

    model->removeRow(nodeIndex.row(), nodeIndex.parent());

    // if (!entityEvent->IsSceneChanged()) m_sceneModel->SortAccordingToMeta(m_sceneMeta);
}


void scene_hierarchy_widget::OnSceneChanged(const Blainn::SceneEventPointer &event)
{
    using namespace Blainn;

    return;
    auto sceneEvent = static_cast<SceneChangedEvent *>(event.get());
    m_sceneMeta = eastl::make_shared<SceneMeta>(QString::fromStdString(sceneEvent->GetName().c_str()));

    // TODO: somehow add additive scenes
    eastl::vector<Entity> entities = {};
    Engine::GetSceneManager().GetActiveScene()->GetEntitiesInHierarchy(entities);

    for (auto &entity : entities)
    {
        CreateEntityInHierarchy(entity, true);
    }
}

void scene_hierarchy_widget::paintEvent(QPaintEvent *event)
{
    BLAINN_PROFILE_FUNC();

    QTreeView::paintEvent(event);
}


void scene_hierarchy_widget::resizeEvent(QResizeEvent *event)
{
    BLAINN_PROFILE_FUNC();

    QTreeView::resizeEvent(event);
}


void scene_hierarchy_widget::OnItemDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight,
                                               const QVector<int> &roles)
{
    if (!topLeft.isValid()) return;


    QString newName = topLeft.data(Qt::DisplayRole).toString();

    if (EntityNode *node = SceneItemModel::GetNodeFromIndex(topLeft))
    {
        node->SetName(newName);
    }
}


void scene_hierarchy_widget::OnSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{

    QModelIndexList selectedIndexes = selected.indexes();

    if (selectedIndexes.isEmpty())
    {
        Blainn::Editor::GetInstance().GetInspector().SetItem(new QWidget());
        Blainn::Engine::GetSelectionManager().SelectUUID({});
        return;
    }

    auto index = selectedIndexes.first();
    auto entity = SceneItemModel::GetNodeFromIndex(index);

    Blainn::Engine::GetSelectionManager().SelectUUID(entity->GetUUID());

    {
        BLAINN_PROFILE_SCOPE(InvalidIndexReturn0);
        if (!index.isValid())
        {
            Blainn::Editor::GetInstance().GetInspector().SetItem(new QWidget());
            return;
        }
    }

    InspectorFabric fabric;
    EntityInspectorData data;
    {
        BLAINN_PROFILE_SCOPE(GetEntityData);
        auto entity = SceneItemModel::GetNodeFromIndex(index);
        data.tag = entity->GetName();
        data.node = entity;
    }
    entity_inspector_content *inspector;
    {
        BLAINN_PROFILE_SCOPE(GetEntityInspectorContent);
        inspector = fabric.GetEntityInspector(data);
    }
    {
        BLAINN_PROFILE_SCOPE(SetItem);
        Blainn::Editor::GetInstance().GetInspector().SetItem(inspector);
    }
}


void scene_hierarchy_widget::SaveCurrentMeta()
{
    if (m_sceneMeta) m_sceneMeta->Save();
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

void scene_hierarchy_widget::ChangeSelection(Blainn::uuid id)
{
    BLAINN_PROFILE_FUNC();


    QModelIndex index;
    {
        BLAINN_PROFILE_SCOPE(FindIndexByEntity);
        index = SceneItemModel::FindIndexByEntity(m_sceneModel, id);
    }

    {
        BLAINN_PROFILE_SCOPE(SetCurrentIndex);
        setCurrentIndex(index);
    }
}


} // namespace editor
