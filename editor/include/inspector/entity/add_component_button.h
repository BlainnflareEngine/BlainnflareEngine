//
// Created by gorev on 13.11.2025.
//

#pragma once
#include "scene/Entity.h"


#include <QMenu>
#include <QPushButton>


class QBoxLayout;
namespace editor
{

class add_component_button : public QPushButton
{
    Q_OBJECT
public:
    add_component_button(const Blainn::Entity &entity, QBoxLayout *layout, QWidget *parent);

public slots:
    void OnClicked();
    void OnTransformAction();
    void OnMeshAction();
    void OnPhysicsAction();
    void OnScriptingAction();
    void OnCameraAction();
    void OnSkyboxAction();

    // AI
    void OnAIControllerAction();
    void OnStimulusAction();
    void OnNavmeshVolumeAction();

private:
    Blainn::Entity m_entity;
    QBoxLayout *m_layout;
    QMenu *m_menu = nullptr;
    QMenu *m_aiMenu = nullptr;
    QAction *m_transformAction = nullptr;
    QAction *m_meshAction = nullptr;
    QAction *m_physicsAction = nullptr;
    QAction *m_scriptingAction = nullptr;
    QAction *m_aiControllerAction = nullptr;
    QAction *m_stimulusAction = nullptr;
    QAction *m_cameraAction = nullptr;
    QAction *m_skyboxAction = nullptr;
    QAction *m_navmeshVolumeAction = nullptr;
};

} // namespace editor