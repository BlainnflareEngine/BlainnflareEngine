//
// Created by gorev on 17.11.2025.
//

#include "input-widgets/path_input_field.h"

#include "Engine.h"
#include "FileSystemUtils.h"

#include <QFileDialog>
#include <QLabel>
#include <QLineEdit>
#include <QMimeData>
#include <QPushButton>
#include <QVBoxLayout>
#include <qevent.h>
#include <qfileinfo.h>

namespace editor
{

path_input_field::path_input_field(const QString &name, const QStringList &acceptedExtensions, QWidget *parent)
{
    setAcceptDrops(true);

    setLayout(new QHBoxLayout());
    layout()->setContentsMargins(0, 0, 0, 0);
    layout()->setSpacing(10);
    layout()->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    m_name = new QLabel(name, this);
    m_name->setStyleSheet("QLabel {"
                          "    font-weight: bold;"
                          "}");
    m_name->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_name->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_name->adjustSize();
    layout()->addWidget(m_name);

    m_input = new QLineEdit(this);

    m_input->setPlaceholderText("Empty");
    m_input->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    m_input->setBaseSize(80, 25);
    SetExtensions(acceptedExtensions);
    layout()->addWidget(m_input);

    m_browse = new QPushButton("", this);
    m_browse->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_browse->setIcon(QIcon(QPixmap(":/icons/folder.png")));
    connect(m_browse, &QPushButton::clicked, this, &path_input_field::OnBrowse);
    layout()->addWidget(m_browse);

    m_delete = new QPushButton("", this);
    m_delete->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_delete->setIcon(QIcon(QPixmap(":/icons/delete.png")));
    connect(m_delete, &QPushButton::clicked, this, &path_input_field::OnDelete);
    layout()->addWidget(m_delete);
}


void path_input_field::SetExtensions(const QStringList &extensions)
{
    m_acceptedExtensions = extensions;
}


QStringList path_input_field::GetExtensions() const
{
    return m_acceptedExtensions;
}


QString path_input_field::GetPath() const
{
    return m_input->text();
}


QString path_input_field::GetAbsolutePath() const
{
    QString currentText = m_input->text();
    if (currentText.isEmpty())
    {
        return currentText;
    }

    if (QDir::isAbsolutePath(currentText))
    {
        return currentText;
    }

    QString contentDir = QString::fromStdString(Blainn::Engine::GetContentDirectory().string());
    return QDir(contentDir).filePath(currentText);
}


void path_input_field::SetPath(const QString &relativePath)
{
    if (m_input->text() == relativePath) return;

    if (relativePath.isEmpty())
    {
        if (m_input->text() != relativePath)
        {
            m_input->setText(relativePath);
            emit PathChanged(relativePath);
        }

        return;
    }


    if (m_input->text() != relativePath)
    {
        m_input->setText(relativePath);
        emit PathChanged(relativePath);
        return;
    }

    m_input->setText("");
    emit PathChanged("");
}


void path_input_field::OnBrowse()
{
    QStringList patterns;
    for (const QString &ext : m_acceptedExtensions)
    {
        patterns << ("*." + ext.toLower());
    }

    QString filter = QString("Supported Files (%1)").arg(patterns.join(" "));

    filter += ";;All Files (*)";

    SelectFileAsync(this, "Select File", QString::fromStdString(Blainn::Engine::GetContentDirectory().string()), filter,
                    [this](const QString &selectedFile)
                    {
                        if (!selectedFile.isEmpty())
                        {
                            QDir contentDir =
                                QDir(QString::fromStdString(Blainn::Engine::GetContentDirectory().string()));

                            SetPath(contentDir.relativeFilePath(selectedFile));
                        }
                    });
}


void path_input_field::OnDelete()
{
    SetPath("");
}


void path_input_field::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls())
    {
        const QList<QUrl> urls = event->mimeData()->urls();
        if (!urls.isEmpty())
        {
            event->acceptProposedAction();
            return;
        }
    }

    event->ignore();
}


void path_input_field::dragMoveEvent(QDragMoveEvent *event)
{
    event->accept();
}


void path_input_field::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasUrls())
    {
        const QList<QUrl> urls = event->mimeData()->urls();

        // considering that we can drag single file only
        if (!urls.isEmpty() && urls[0].isLocalFile())
        {
            QString filePath = urls[0].toLocalFile();
            if (IsFileAccepted(filePath))
            {
                QDir contentDir = QDir(QString::fromStdString(Blainn::Engine::GetContentDirectory().string()));
                QDir path = QDir(filePath);
                auto a = contentDir.relativeFilePath(path.path());
                SetPath(contentDir.relativeFilePath(path.path()));
                event->acceptProposedAction();
                return;
            }
        }
    }
    event->ignore();
}


void path_input_field::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        QString filter;
        if (!m_acceptedExtensions.isEmpty())
        {
            filter = "Supported files (";
            for (const QString &ext : m_acceptedExtensions)
            {
                filter += "*." + ext + " ";
            }
            filter.chop(1);
            filter += ");;All files (*.*)";
        }
        else
        {
            filter = "All files (*.*)";
        }

        QString initialDir = GetPath();
        if (initialDir.isEmpty() || !QFileInfo::exists(initialDir))

            initialDir = QString::fromStdString(Blainn::Engine::GetContentDirectory().string());

        else if (QFileInfo(initialDir).isFile()) initialDir = QFileInfo(initialDir).absolutePath();


        SelectFileAsync(this, "Select File", initialDir, filter,
                        [this](const QString &selectedFile)
                        {
                            if (!selectedFile.isEmpty())
                            {
                                SetPath(selectedFile);
                            }
                        });
    }

    QWidget::mousePressEvent(event);
}


bool path_input_field::IsFileAccepted(const QString &filePath) const
{
    if (m_acceptedExtensions.isEmpty()) return true;

    QFileInfo fileInfo(filePath);
    QString suffix = fileInfo.suffix().toLower();
    return m_acceptedExtensions.contains(suffix);
}
} // namespace editor