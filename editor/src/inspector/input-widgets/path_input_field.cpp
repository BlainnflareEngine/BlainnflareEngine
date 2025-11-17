//
// Created by gorev on 17.11.2025.
//

#include "../../../include/inspector/input-widgets/path_input_field.h"

#include "Engine.h"
#include "FileSystemUtils.h"
#include "Log.h"

#include <QDir>
#include <QFileDialog>
#include <QMimeData>
#include <qevent.h>
#include <qfileinfo.h>

namespace editor
{
path_input_field::path_input_field(QWidget *parent)
    : QLineEdit(parent)
{
    setAcceptDrops(true);
    setPlaceholderText("Empty");
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    setBaseSize(80, 25);
}


path_input_field::path_input_field(const QStringList &acceptedExtensions, QWidget *parent)
{
    setAcceptDrops(true);
    setPlaceholderText("Empty");
    SetExtensions(acceptedExtensions);
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    setBaseSize(80, 25);
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
    return text();
}


QString path_input_field::GetAbsolutePath() const
{
    QString currentText = text();
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
    if (relativePath.isEmpty())
    {
        if (text() != relativePath)
        {
            setText(relativePath);
            emit PathChanged(relativePath);
        }
        return;
    }

    /*if (!relativePath.startsWith(".") && !relativePath.startsWith("..") && !relativePath.isEmpty())
    {
        if (text() != relativePath)
        {
            setText(relativePath);
            emit PathChanged(relativePath);
        }
        return;
    }*/

    if (text() != relativePath)
    {
        setText(relativePath);
        emit PathChanged(relativePath);
        return;
    }

    setText("");
    emit PathChanged("");
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
                SetPath(filePath);
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

    QLineEdit::mousePressEvent(event);
}


bool path_input_field::IsFileAccepted(const QString &filePath) const
{
    if (m_acceptedExtensions.isEmpty()) return true;

    QFileInfo fileInfo(filePath);
    QString suffix = fileInfo.suffix().toLower();
    return m_acceptedExtensions.contains(suffix);
}
} // namespace editor