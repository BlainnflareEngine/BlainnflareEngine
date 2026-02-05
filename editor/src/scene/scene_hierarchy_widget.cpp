#include "scene_hierarchy_widget.h"

#include "Editor.h"
#include "Engine.h"
#include "FileSystemUtils.h"
#include "InspectorFabric.h"
#include "context-menu/SceneContextMenu.h"
#include "ui_scene_hierarchy_widget.h"

#include <QMimeData>
#include <QApplication>
#include <QDrag>

namespace editor
{
constexpr char MIME_ENTITY_UUID[] = "application/x-blainn-entity-uuid";

scene_hierarchy_widget::scene_hierarchy_widget(QWidget *parent)
    : QTreeWidget(parent)
    , ui(new Ui::scene_hierarchy_widget)
{
    // m_sceneMeta = eastl::make_shared<SceneMeta>("");

    ui->setupUi(this);

    setHeaderHidden(true);
    setIndentation(15);
    setSelectionMode(SingleSelection);
    setContextMenuPolicy(Qt::CustomContextMenu);
    setDragEnabled(true);
    setAcceptDrops(true);
    setDropIndicatorShown(true);
    setDragDropMode(DragDrop);
    setEditTriggers(EditKeyPressed | DoubleClicked);
    viewport()->setAcceptDrops(true);

    m_addToSceneMenu = new SceneContextMenu(*this, this);

    connect(this, &QTreeWidget::customContextMenuRequested, this, &scene_hierarchy_widget::OpenChildContextMenu);
    connect(this, &QTreeWidget::itemSelectionChanged, this, &scene_hierarchy_widget::OnItemSelectionChanged);
    connect(this, &QTreeWidget::itemChanged, this, &scene_hierarchy_widget::OnItemChanged);
    connect(this, &QTreeWidget::itemDoubleClicked, this, &scene_hierarchy_widget::OnItemDoubleClicked);

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


QTreeWidgetItem *scene_hierarchy_widget::FindItemByUuid(const Blainn::uuid &uuid) const
{
    struct StackItem
    {
        QTreeWidgetItem *item;
        int childIndex;
    };

    eastl::vector<StackItem> stack;

    for (int i = 0; i < topLevelItemCount(); ++i)
    {
        stack.push_back({topLevelItem(i), 0});
    }

    while (!stack.empty())
    {
        auto &current = stack.back();

        if (GetUUIDFromItem(current.item) == uuid) return current.item;

        if (current.childIndex < current.item->childCount())
        {
            stack.push_back({current.item->child(current.childIndex), 0});
            current.childIndex++;
        }
        else
        {
            stack.pop_back();
        }
    }

    return nullptr;
}

Blainn::uuid scene_hierarchy_widget::GetUUIDFromItem(QTreeWidgetItem *item) const
{
    if (!item) return {};

    QString uuidStr = item->data(0, Qt::UserRole).toString();
    if (uuidStr.isEmpty()) return {};

    return Blainn::uuid(ToString(uuidStr));
}

void scene_hierarchy_widget::AddItemForEntity(const Blainn::Entity &entity, QTreeWidgetItem *parentItem)
{
    if (!entity.IsValid()) return;

    auto *item = new QTreeWidgetItem();
    item->setText(0, ToQString(entity.Name()));
    item->setFlags(item->flags() | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled);

    QString uuidStr = entity.GetUUID().bytes().c_str();
    item->setData(0, Qt::UserRole, uuidStr);
    item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsDragEnabled
                   | Qt::ItemIsDropEnabled);

    if (parentItem)
    {
        parentItem->addChild(item);
    }
    else
    {
        addTopLevelItem(item);
    }

    auto children = entity.Children();
    for (auto &child : children)
    {
        AddItemForEntity(Blainn::Engine::GetSceneManager().TryGetEntityWithUUID(child), item);
    }
}

void scene_hierarchy_widget::RemoveItemForEntity(const Blainn::uuid &uuid)
{
    if (auto *item = FindItemByUuid(uuid))
    {
        delete item;
    }
}

bool scene_hierarchy_widget::IsDescendant(QTreeWidgetItem *ancestor, QTreeWidgetItem *item) const
{
    while (item)
    {
        item = item->parent();
        if (item == ancestor) return true;
    }
    return false;
}

eastl::vector<Blainn::uuid> scene_hierarchy_widget::SaveExpandedState(QTreeWidgetItem *root) const
{
    eastl::vector<Blainn::uuid> uuids;

    if (root)
    {
        collectExpandedState(root, uuids);
    }
    else
    {
        for (int i = 0; i < topLevelItemCount(); ++i)
        {
            collectExpandedState(topLevelItem(i), uuids);
        }
    }

    return uuids;
}

void scene_hierarchy_widget::RestoreExpandedState(const eastl::vector<Blainn::uuid> &uuids)
{
    for (const auto &uuid : uuids)
    {
        if (auto *item = FindItemByUuid(uuid))
        {
            item->setExpanded(true);
        }
    }
}


void scene_hierarchy_widget::OnEntityCreated(const Blainn::SceneEventPointer &event)
{
    using namespace Blainn;

    const auto entityEvent = static_cast<const EntityCreatedEvent *>(event.get());

    if (!entityEvent->GetEntity().IsValid()) return;

    const auto &entity = entityEvent->GetEntity();

    if (FindItemByUuid(entity.GetUUID())) return;

    if (entity.GetParent().IsValid())
    {
        auto parent = entity.GetParent();
        if (auto *parentItem = FindItemByUuid(parent.GetUUID()))
        {
            AddItemForEntity(entity, parentItem);
        }
        else
        {
            AddItemForEntity(entity, nullptr);
        }
    }
    else
    {
        AddItemForEntity(entity, nullptr);
    }

    if (entityEvent->CreatedByEditor() && !entityEvent->IsSceneChanged())
    {
        if (auto *item = FindItemByUuid(entity.GetUUID()))
        {
            setCurrentItem(item);
            editItem(item);
        }
    }
}

void scene_hierarchy_widget::OnEntityDestroyed(const Blainn::SceneEventPointer &event)
{
    using namespace Blainn;

    const auto entityEvent = static_cast<const EntityDestroyedEvent *>(event.get());
    RemoveItemForEntity(entityEvent->GetUUID());
}

void scene_hierarchy_widget::OnSceneChanged(const Blainn::SceneEventPointer &event)
{
    using namespace Blainn;
    // TODO: add additive scenes?
}

void scene_hierarchy_widget::BuildTreeFromScene()
{
    using namespace Blainn;

    for (auto &scene : Engine::GetSceneManager().GetActiveScenes())
    {
        eastl::unordered_map<uuid, QTreeWidgetItem *> itemMap;
        eastl::vector<Entity> rootEntities;

        auto view = scene->GetAllEntitiesWith<IDComponent, RelationshipComponent>();
        for (auto [entityHandle, idComp, relationshipComp] : view.each())
        {
            Entity entity(entityHandle, scene.get());
            auto *item = new QTreeWidgetItem();
            item->setText(0, ToQString(entity.Name()));
            item->setFlags(item->flags() | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled);

            QByteArray uuidData;
            uuidData.resize(sizeof(uuid));
            uuid entityUuid = entity.GetUUID();
            memcpy(uuidData.data(), &entityUuid, sizeof(uuid));
            item->setData(0, Qt::UserRole, uuidData);

            itemMap[entity.GetUUID()] = item;
        }

        for (auto &[uuid, item] : itemMap)
        {
            Entity entity = scene->TryGetEntityWithUUID(uuid);
            if (!entity.IsValid()) continue;

            if (entity.GetParent().IsValid())
            {
                auto parent = entity.GetParent();
                auto parentIt = itemMap.find(parent.GetUUID());
                if (parentIt != itemMap.end())
                {
                    parentIt->second->addChild(item);
                }
                else
                {
                    addTopLevelItem(item);
                }
            }
            else
            {
                addTopLevelItem(item);
            }
        }
    }
}


void scene_hierarchy_widget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_dragStartPosition = event->pos();
    }
    QTreeWidget::mousePressEvent(event);
}


void scene_hierarchy_widget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat(MIME_ENTITY_UUID))
    {
        event->acceptProposedAction();
        return;
    }

    event->ignore();
}

void scene_hierarchy_widget::dragMoveEvent(QDragMoveEvent *event)
{
    auto *targetItem = itemAt(event->position().toPoint());
    auto *sourceItem = currentItem();

    if (!sourceItem)
    {
        event->ignore();
        return;
    }

    if (targetItem && (targetItem == sourceItem || IsDescendant(sourceItem, targetItem)))
    {
        event->ignore();
        return;
    }

    event->acceptProposedAction();
}

void scene_hierarchy_widget::dropEvent(QDropEvent *event)
{
    BLAINN_PROFILE_SCOPE(DropEntityInHierarchy);

    auto *targetItem = itemAt(event->position().toPoint());
    blockSignals(true);

    if (!event->mimeData()->hasFormat(MIME_ENTITY_UUID))
    {
        event->ignore();
        blockSignals(false);
        return;
    }

    QByteArray encoded = event->mimeData()->data(MIME_ENTITY_UUID);

    if (encoded.isEmpty())
    {
        event->ignore();
        blockSignals(false);
        return;
    }

    QString uuidStr = QString::fromUtf8(encoded);
    Blainn::uuid uuid = Blainn::uuid::fromStrFactory(ToString(uuidStr));

    auto *draggedItem = FindItemByUuid(uuid);
    if (!draggedItem)
    {
        BF_ERROR("Cannot find item with UUID {} in hierarchy", uuid.str());
        event->ignore();
        blockSignals(false);
        return;
    }

    if (targetItem && (targetItem == draggedItem || IsDescendant(draggedItem, targetItem)))
    {
        BF_WARN("Cannot drop on self or descendant");
        event->ignore();
        blockSignals(false);
        return;
    }

    auto expandedState = SaveExpandedState(draggedItem);

    auto *oldParent = draggedItem->parent();
    int oldIndex = oldParent ? oldParent->indexOfChild(draggedItem) : indexOfTopLevelItem(draggedItem);

    if (oldParent) oldParent->takeChild(oldIndex);
    else takeTopLevelItem(oldIndex);

    if (targetItem)
    {
        targetItem->addChild(draggedItem);
        targetItem->setExpanded(true);
    }
    else
    {
        addTopLevelItem(draggedItem);
    }

    RestoreExpandedState(expandedState);

    auto &sceneManager = Blainn::Engine::GetSceneManager();
    auto entity = sceneManager.TryGetEntityWithUUID(uuid);
    if (entity.IsValid())
    {
        if (targetItem)
        {
            auto targetUuid = GetUUIDFromItem(targetItem);
            auto newParent = sceneManager.TryGetEntityWithUUID(targetUuid);
            if (newParent.IsValid()) sceneManager.ParentEntity(entity, newParent);
            else sceneManager.UnparentEntity(entity);
        }
        else
        {
            sceneManager.UnparentEntity(entity);
        }
    }

    clearSelection();
    draggedItem->setSelected(true);
    scrollToItem(draggedItem);
    blockSignals(false);

    event->acceptProposedAction();
}


void scene_hierarchy_widget::OnItemSelectionChanged()
{
    BLAINN_PROFILE_SCOPE(OnItemSelectionChanged);

    auto selectedItems = this->selectedItems();
    if (selectedItems.isEmpty())
    {
        Blainn::Editor::GetInstance().GetInspector().SetItem(new QWidget());
        Blainn::Engine::GetSelectionManager().SelectUUID({});
        m_entityInspector = nullptr;
        return;
    }

    auto *item = selectedItems.first();
    auto uuid = GetUUIDFromItem(item);

    Blainn::Engine::GetSelectionManager().SelectUUID(uuid);

    auto entity = Blainn::Engine::GetSceneManager().TryGetEntityWithUUID(uuid);
    if (!entity.IsValid())
    {
        Blainn::Editor::GetInstance().GetInspector().SetItem(new QWidget());
        m_entityInspector = nullptr;
        return;
    }

    InspectorFabric fabric;
    m_entityInspector = fabric.GetEntityInspector(uuid);
    Blainn::Editor::GetInstance().GetInspector().SetItem(m_entityInspector);
}

void scene_hierarchy_widget::OnItemChanged(QTreeWidgetItem *item, int column)
{
    if (column != 0) return;

    auto uuid = GetUUIDFromItem(item);
    auto &sceneManager = Blainn::Engine::GetSceneManager();
    auto entity = sceneManager.TryGetEntityWithUUID(uuid);

    if (entity.IsValid() && entity.HasComponent<Blainn::TagComponent>())
    {
        entity.GetComponent<Blainn::TagComponent>().Tag = ToEASTLString(item->text(0));
        if (m_entityInspector->GetCurrentEntityUUID() == uuid) m_entityInspector->SetTag(item->text(0));
    }
}

void scene_hierarchy_widget::OnItemDoubleClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column);
    // TODO: open entity editor?
}


void scene_hierarchy_widget::ChangeSelection(const Blainn::uuid &id)
{
    BLAINN_PROFILE_FUNC();

    if (auto *item = FindItemByUuid(id))
    {
        blockSignals(true);
        setCurrentItem(item);
        scrollToItem(item);
        blockSignals(false);
    }
}


void scene_hierarchy_widget::collectExpandedState(QTreeWidgetItem *item, eastl::vector<Blainn::uuid> &uuids) const
{
    if (item->isExpanded() && item->childCount() > 0)
    {
        uuids.push_back(GetUUIDFromItem(item));
    }

    for (int i = 0; i < item->childCount(); ++i)
    {
        collectExpandedState(item->child(i), uuids);
    }
}

void scene_hierarchy_widget::paintEvent(QPaintEvent *event)
{
    BLAINN_PROFILE_FUNC();
    QTreeWidget::paintEvent(event);
}

void scene_hierarchy_widget::resizeEvent(QResizeEvent *event)
{
    BLAINN_PROFILE_FUNC();
    QTreeWidget::resizeEvent(event);
}

void scene_hierarchy_widget::keyPressEvent(QKeyEvent *event)
{
    if (auto *current = currentItem())
    {
        QKeySequence keySequence(event->key() | event->modifiers());

        if (keySequence == m_addToSceneMenu->GetDeleteKey())
        {
            m_addToSceneMenu->DeleteCurrentEntity();
            event->accept();
            return;
        }

        if (keySequence == m_addToSceneMenu->GetRenameKey())
        {
            editItem(current);
            event->accept();
            return;
        }
    }

    QTreeWidget::keyPressEvent(event);
}


QMimeData *scene_hierarchy_widget::mimeData(const QList<QTreeWidgetItem *> &items) const
{
    if (items.isEmpty()) return nullptr;

    auto *mimeData = new QMimeData();

    auto *item = items.first();

    QVariant userData = item->data(0, Qt::UserRole);
    QString uuidStr;

    if (userData.typeId() == QMetaType::QString)
    {
        uuidStr = userData.toString();
    }
    else if (userData.typeId() == QMetaType::QByteArray)
    {
        uuidStr = QString::fromUtf8(userData.toByteArray());
    }

    if (!uuidStr.isEmpty())
    {
        mimeData->setData(MIME_ENTITY_UUID, uuidStr.toUtf8());
    }

    return mimeData;
}


Qt::DropActions scene_hierarchy_widget::supportedDropActions() const
{
    return Qt::MoveAction | Qt::CopyAction;
}

void scene_hierarchy_widget::startDrag(Qt::DropActions supportedActions)
{
    QTreeWidget::startDrag(supportedActions);
}

void scene_hierarchy_widget::OpenContextMenu(const QPoint &position)
{
    m_addToSceneMenu->OpenMenu(position);
}


void scene_hierarchy_widget::OpenChildContextMenu(const QPoint &pos)
{
    auto *item = itemAt(pos);
    if (item)
    {
        setCurrentItem(item);
    }

    m_addToSceneMenu->OnContextMenu(pos);
}

} // namespace editor