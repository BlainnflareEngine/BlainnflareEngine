//
// Created by gorev on 14.11.2025.
//

#include "entity/transform_widget.h"
#include "scene/EntityTemplates.h"

#include "input-widgets/float_input_field.h"
#include "LabelsUtils.h"

#include <QKeyEvent>
#include <QLabel>

#include <QTimer>
#include <QVBoxLayout>

#include "Engine.h"
#include "subsystems/PhysicsSubsystem.h"
#include "input-widgets/vector3_input_widget.h"

namespace editor
{
transform_widget::transform_widget(const Blainn::Entity &entity, QWidget *parent)
    : component_widget_base(entity, "Transform", parent)
{
    CreateTransformFields();
    LoadTransformValues();
    ConnectSignals();
}


void transform_widget::OnPositionChanged()
{
    if (!m_entity.HasComponent<Blainn::TransformComponent>()) return;

    const auto &scene = Blainn::Engine::GetActiveScene();
    if (!scene)
    {
        return;
    }
    // scene->ConvertToLocalSpace(m_entity);

    auto &transform = m_entity.GetComponent<Blainn::TransformComponent>();
    transform.SetTranslation(m_position->GetValue());
    // scene->ConvertToWorldSpace(m_entity);

    if (m_entity.HasComponent<Blainn::PhysicsComponent>())
    {
        Blainn::PhysicsSubsystem::UpdateBodyInJolt(*scene, m_entity.GetUUID());
    }
}


void transform_widget::OnRotationChanged()
{
    if (!m_entity.HasComponent<Blainn::TransformComponent>()) return;

    auto &transform = m_entity.GetComponent<Blainn::TransformComponent>();

    auto quat = Blainn::Quat::CreateFromYawPitchRoll(m_rotation->GetValue() * XM_PI / 180.f);
    transform.SetRotation(quat);

    const auto &scene = Blainn::Engine::GetActiveScene();
    if (!scene)
    {
        return;
    }

    if (m_entity.HasComponent<Blainn::PhysicsComponent>())
    {
        Blainn::PhysicsSubsystem::UpdateBodyInJolt(*scene, m_entity.GetUUID());
    }
}


void transform_widget::OnScaleChanged()
{
    if (!m_entity.HasComponent<Blainn::TransformComponent>()) return;

    auto &transform = m_entity.GetComponent<Blainn::TransformComponent>();
    transform.SetScale(m_scale->GetValue());
}


void transform_widget::OnUpdate()
{
    component_widget_base::OnUpdate();

    if (!m_entity.IsValid()) return;

    LoadTransformValues();
}


void transform_widget::DeleteComponent()
{
    if (m_entity.IsValid()) m_entity.RemoveComponentIfExists<Blainn::TransformComponent>();
    deleteLater();
}
void transform_widget::paintEvent(QPaintEvent *event)
{
    BLAINN_PROFILE_FUNC();
    component_widget_base::paintEvent(event);
}


void transform_widget::CreateTransformFields()
{
    m_position = new vector3_input_widget("Position", Blainn::Vec3::Zero, this);
    m_position->SetDecimals(3);
    layout()->addWidget(m_position);

    m_rotation = new vector3_input_widget("Rotation", Blainn::Vec3::Zero, this);
    m_rotation->SetDecimals(3);
    m_rotation->SetSingleStep(5);
    layout()->addWidget(m_rotation);

    m_scale = new vector3_input_widget("Scale", Blainn::Vec3::One, this);
    m_scale->SetDecimals(3);
    layout()->addWidget(m_scale);
}


void transform_widget::LoadTransformValues()
{
    if (!m_entity.HasComponent<Blainn::TransformComponent>()) return;

    auto &transform = m_entity.GetComponent<Blainn::TransformComponent>();
    const auto &scene = Blainn::Engine::GetActiveScene();
    if (!scene) return;

    // scene->ConvertToLocalSpace(m_entity);

    BlockSignals(true);
    if (!m_position->HasFocus()) m_position->SetValue(transform.GetTranslation());

    if (!m_rotation->HasFocus()) m_rotation->SetValue(transform.GetRotationEuler() / XM_PI * 180.0f);

    if (!m_scale->HasFocus()) m_scale->SetValue(transform.GetScale());
    BlockSignals(false);

    // scene->ConvertToWorldSpace(m_entity);
}


void transform_widget::ConnectSignals()
{
    connect(m_position, &vector3_input_widget::ValueChanged, this, &transform_widget::OnPositionChanged);
    connect(m_rotation, &vector3_input_widget::ValueChanged, this, &transform_widget::OnRotationChanged);
    connect(m_scale, &vector3_input_widget::ValueChanged, this, &transform_widget::OnScaleChanged);
}


void transform_widget::BlockSignals(bool block)
{
    m_position->blockSignals(block);
    m_rotation->blockSignals(block);
    m_scale->blockSignals(block);
}


bool transform_widget::IsValidNumericInput(const QString &currentText, const QString &newText)
{
    if (newText.isEmpty() || newText == "-" || newText == "-." || newText == ".")
    {
        return true;
    }

    bool ok;
    newText.toFloat(&ok);
    return ok;
}


QWidget *transform_widget::CreateVector3(const QString &title, float_input_field *&xField, float_input_field *&yField,
                                         float_input_field *&zField)
{
    auto group = new QWidget(this);
    auto gridLayout = new QGridLayout(group);
    gridLayout->setContentsMargins(0, 5, 0, 5);
    gridLayout->setVerticalSpacing(10);
    gridLayout->setHorizontalSpacing(10);

    auto titleLabel = new QLabel(ToHeader3(title), group);
    titleLabel->setTextFormat(Qt::MarkdownText);
    titleLabel->setStyleSheet("font-weight: bold;");
    gridLayout->addWidget(titleLabel, 0, 0, 1, 3, Qt::AlignLeft);

    QColor xColor = QColor::fromRgb(255, 68, 68);
    QColor yColor = QColor::fromRgb(68, 255, 68);
    QColor zColor = QColor::fromRgb(68, 68, 255);

    QSize fieldSize{70, 20};

    xField = new float_input_field("X", 0, group, true, xColor);
    xField->setMinimumSize(fieldSize);
    xField->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    gridLayout->addWidget(xField, 2, 0);


    yField = new float_input_field("Y", 0, group, true, yColor);
    yField->setMinimumSize(fieldSize);
    yField->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    gridLayout->addWidget(yField, 2, 1);

    zField = new float_input_field("Z", 0, group, true, zColor);
    zField->setMinimumSize(fieldSize);
    zField->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    gridLayout->addWidget(zField, 2, 2);

    return group;
}
} // namespace editor