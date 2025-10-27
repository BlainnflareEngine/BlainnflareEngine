//
// Created by gorev on 27.10.2025.
//

#include "TexturePreviewAsyncLoader.h"

#include <QPixmap>

namespace editor
{
TexturePreviewAsyncLoader::TexturePreviewAsyncLoader(const QString &filePath, const QPersistentModelIndex &index)
    : m_filePath(filePath)
    , m_index(index)
{
    setAutoDelete(true);
}


void TexturePreviewAsyncLoader::run()
{
    QPixmap pixmap = QPixmap(m_filePath);

    if (!pixmap.isNull())
    {
        pixmap = pixmap.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation);;
    }
    else
    {
        pixmap = QPixmap(QString(":/icons/error.png"));
        pixmap = pixmap.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    emit loaded(m_filePath, pixmap, m_index);
}
} // namespace editor