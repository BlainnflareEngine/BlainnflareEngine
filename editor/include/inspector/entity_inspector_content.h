//
// Created by gorev on 12.11.2025.
//

#pragma once
#include "EntityInspectorData.h"
#include "inspector_content_base.h"
#include <QLabel>
#include <QVBoxLayout>


namespace editor
{

class entity_inspector_content : public inspector_content_base
{
    Q_OBJECT

public:
    explicit entity_inspector_content(const EntityInspectorData &data, QWidget *parent = nullptr);

    void SetTag(const QString &tag);

    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

protected:
    EntityInspectorData m_data;

    QLabel *m_tag;
};

} // namespace editor