//
// Created by gorev on 14.11.2025.
//

#include "entity/transform_widget.h"

#include "LabelsUtils.h"
#include "float_input_field.h"

#include <QKeyEvent>
#include <QLabel>

#include <QSpinBox>
#include <QVBoxLayout>

namespace editor
{
transform_widget::transform_widget(const Blainn::Entity &entity, QWidget *parent)
    : themed_panel(parent)
    , m_entity(entity)
{
    setLayout(new QVBoxLayout());
    layout()->setSpacing(5);
    layout()->setContentsMargins(15, 15, 15, 15);
    layout()->setAlignment(Qt::AlignLeft);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    m_label = new QLabel(ToHeader2("Transform"), this);
    m_label->setTextFormat(Qt::MarkdownText);
    layout()->addWidget(m_label);

    auto separator = new QFrame(this);
    separator->setFrameShape(HLine);
    layout()->addWidget(separator);

    CreateTransformFields();
    LoadTransformValues();
    ConnectSignals();
}


void transform_widget::OnPositionChanged()
{
    BF_DEBUG("Editing changed!");

    if (!m_entity.HasComponent<Blainn::TransformComponent>())
    {
        return;
    }

    auto &transform = m_entity.GetComponent<Blainn::TransformComponent>();
    transform.Translation.x = m_positionX->GetValue();
    transform.Translation.y = m_positionY->GetValue();
    transform.Translation.z = m_positionZ->GetValue();
}


void transform_widget::OnRotationChanged()
{
    if (!m_entity.HasComponent<Blainn::TransformComponent>())
    {
        return;
    }

    auto &transform = m_entity.GetComponent<Blainn::TransformComponent>();

    transform.SetRotationEuler(Blainn::Vec3(m_rotationX->GetValue(), m_rotationY->GetValue(), m_rotationZ->GetValue()));
}


void transform_widget::OnScaleChanged()
{
    if (!m_entity.HasComponent<Blainn::TransformComponent>())
    {
        return;
    }

    auto &transform = m_entity.GetComponent<Blainn::TransformComponent>();
    transform.Scale.x = m_scaleX->GetValue();
    transform.Scale.y = m_scaleY->GetValue();
    transform.Scale.z = m_scaleZ->GetValue();
}


void transform_widget::CreateTransformFields()
{
    auto position = CreateVector3("Position", m_positionX, m_positionY, m_positionZ);
    layout()->addWidget(position);

    auto rotation = CreateVector3("Rotation", m_rotationX, m_rotationY, m_rotationZ);
    layout()->addWidget(rotation);

    auto scale = CreateVector3("Scale", m_scaleX, m_scaleY, m_scaleZ);
    layout()->addWidget(scale);
}


void transform_widget::LoadTransformValues()
{
    if (!m_entity.HasComponent<Blainn::TransformComponent>()) return;

    auto &transform = m_entity.GetComponent<Blainn::TransformComponent>();

    BlockSignals(true);

    m_positionX->SetValue(transform.Translation.x);
    m_positionY->SetValue(transform.Translation.y);
    m_positionZ->SetValue(transform.Translation.z);

    auto euler = transform.GetRotationEuler();
    m_rotationX->SetValue(euler.x);
    m_rotationY->SetValue(euler.y);
    m_rotationZ->SetValue(euler.z);

    m_scaleX->SetValue(transform.Scale.x);
    m_scaleY->SetValue(transform.Scale.y);
    m_scaleZ->SetValue(transform.Scale.z);

    BlockSignals(false);
}


void transform_widget::ConnectSignals()
{
    connect(m_positionX, &float_input_field::EditingFinished, this, &transform_widget::OnPositionChanged);
    connect(m_positionY, &float_input_field::EditingFinished, this, &transform_widget::OnPositionChanged);
    connect(m_positionZ, &float_input_field::EditingFinished, this, &transform_widget::OnPositionChanged);

    connect(m_rotationX, &float_input_field::EditingFinished, this, &transform_widget::OnRotationChanged);
    connect(m_rotationY, &float_input_field::EditingFinished, this, &transform_widget::OnRotationChanged);
    connect(m_rotationZ, &float_input_field::EditingFinished, this, &transform_widget::OnRotationChanged);

    connect(m_scaleX, &float_input_field::EditingFinished, this, &transform_widget::OnScaleChanged);
    connect(m_scaleY, &float_input_field::EditingFinished, this, &transform_widget::OnScaleChanged);
    connect(m_scaleZ, &float_input_field::EditingFinished, this, &transform_widget::OnScaleChanged);
}


void transform_widget::BlockSignals(bool block)
{
    m_positionX->blockSignals(block);
    m_positionY->blockSignals(block);
    m_positionZ->blockSignals(block);
    m_rotationX->blockSignals(block);
    m_rotationY->blockSignals(block);
    m_rotationZ->blockSignals(block);
    m_scaleX->blockSignals(block);
    m_scaleY->blockSignals(block);
    m_scaleZ->blockSignals(block);
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
    gridLayout->setVerticalSpacing(5);
    gridLayout->setHorizontalSpacing(10);

    auto titleLabel = new QLabel(title, group);
    titleLabel->setStyleSheet("font-weight: bold;");
    gridLayout->addWidget(titleLabel, 0, 0, 1, 3, Qt::AlignLeft);

    QString xColor = "#FF4444";
    QString yColor = "#44FF44";
    QString zColor = "#4444FF";
    QSize fieldSize{70, 20};

    auto xLabel = new QLabel("X", group);
    xLabel->setAlignment(Qt::AlignCenter);
    xLabel->setStyleSheet(QString("QLabel {"
                                  "    color: %1;"
                                  "    font-weight: bold;"
                                  "    font-size: 9pt;"
                                  "}")
                              .arg(xColor));
    gridLayout->addWidget(xLabel, 1, 0);

    xField = new float_input_field(group);
    xField->setMinimumSize(fieldSize);
    xField->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    gridLayout->addWidget(xField, 2, 0);

    auto yLabel = new QLabel("Y", group);
    yLabel->setAlignment(Qt::AlignCenter);
    yLabel->setStyleSheet(QString("QLabel {"
                                  "    color: %1;"
                                  "    font-weight: bold;"
                                  "    font-size: 9pt;"
                                  "}")
                              .arg(yColor));
    gridLayout->addWidget(yLabel, 1, 1);

    yField = new float_input_field(group);
    yField->setMinimumSize(fieldSize);
    yField->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    gridLayout->addWidget(yField, 2, 1);

    auto zLabel = new QLabel("Z", group);
    zLabel->setAlignment(Qt::AlignCenter);
    zLabel->setStyleSheet(QString("QLabel {"
                                  "    color: %1;"
                                  "    font-weight: bold;"
                                  "    font-size: 9pt;"
                                  "}")
                              .arg(zColor));
    gridLayout->addWidget(zLabel, 1, 2);

    zField = new float_input_field(group);
    zField->setMinimumSize(fieldSize);
    zField->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    gridLayout->addWidget(zField, 2, 2);

    return group;
}
} // namespace editor