//
// Created by gorev on 09.10.2025.
//


#include "path_bar_widget.h"
#include "ui_path_bar_widget.h"

#include <QLabel>
#include <QPushButton>

namespace editor
{
path_bar_widget::path_bar_widget(QWidget *parent)
    : QWidget(parent)
    , m_ui(new Ui::path_bar_widget)
{
    m_ui->setupUi(this);

    m_layout = new QHBoxLayout();
    m_layout->setSpacing(0);
    m_layout->setContentsMargins(5, 5, 5, 5);
    setLayout(m_layout);
}


path_bar_widget::~path_bar_widget()
{
    delete m_ui;
}


void path_bar_widget::SetRootPath(const QString &root)
{
    m_rootPath = root;
    SetCurrentPath(m_rootPath);
}


void path_bar_widget::SetCurrentPath(const QString &path)
{
    if (m_rootPath.isEmpty())
    {
        qWarning() << "PathBar: Root path is not set";
        return;
    }

    QDir rootDir(m_rootPath);
    rootDir.cdUp();
    QString relativePath = rootDir.relativeFilePath(path);

    if (relativePath.isEmpty())
    {
        return;
    }

    m_currentPath = relativePath;
    UpdatePath();
}


void path_bar_widget::UpdatePath()
{
    QLayoutItem *item;
    while ((item = m_layout->takeAt(0)) != nullptr)
    {
        delete item->widget();
        delete item;
    }

    QStringList segments = m_currentPath.split('/', Qt::SkipEmptyParts);

    QString cumulativePath;
    for (int i = 0; i < segments.size(); ++i)
    {
        if (i > 0)
        {
            QLabel *arrow = new QLabel(">", this);
            m_layout->addWidget(arrow);
        }

        QPushButton *button = new QPushButton(segments[i], this);
        button->setFlat(true);
        button->setStyleSheet("QPushButton { text-align: left; padding: 2px; }");

        cumulativePath = cumulativePath.isEmpty() ? segments[i] : cumulativePath + "/" + segments[i];

        connect(button, &QPushButton::clicked, this,
                [this, cumulativePath]()
                {
                    emit PathClicked(cumulativePath);
                    SetCurrentPath(cumulativePath);
                });

        m_layout->addWidget(button);
    }

    m_layout->addStretch();
}
} // namespace editor
