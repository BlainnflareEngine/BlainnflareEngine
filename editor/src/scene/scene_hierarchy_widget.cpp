//
// Created by gorev on 25.10.2025.
//


#include "scene_hierarchy_widget.h"

#include "EntityDelegate.h"
#include "SceneModel.h"
#include "context-menu/AddToSceneContextMenu.h"
#include "ui_scene_hierarchy_widget.h"

namespace editor
{
scene_hierarchy_widget::scene_hierarchy_widget(QWidget *parent)
    : QTreeView(parent)
    , ui(new Ui::scene_hierarchy_widget)
{
    ui->setupUi(this);

    m_sceneModel = new SceneModel(this);
    setModel(m_sceneModel);
    setHeaderHidden(true);
    setSelectionMode(SingleSelection);
    setContextMenuPolicy(Qt::CustomContextMenu);
    setItemDelegate(new EntityDelegate(this));

    m_addToSceneMenu = new AddToSceneContextMenu(*this, this);

    connect(this, &QTreeView::customContextMenuRequested, m_addToSceneMenu, &AddToSceneContextMenu::OnContextMenu);
}

scene_hierarchy_widget::~scene_hierarchy_widget()
{
    delete ui;
}


void scene_hierarchy_widget::OpenContextMenu(const QPoint &position)
{
    m_addToSceneMenu->OpenMenu(position);
}


SceneModel &scene_hierarchy_widget::GetSceneModel()
{
    return *m_sceneModel;
}
} // namespace editor
