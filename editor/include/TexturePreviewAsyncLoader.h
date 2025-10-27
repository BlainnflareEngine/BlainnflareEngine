//
// Created by gorev on 27.10.2025.
//

#pragma once
#include "ContentFilterProxyModel.h"


#include <QObject>
#include <QRunnable>

namespace editor
{

class TexturePreviewAsyncLoader : public QObject, public QRunnable
{
    Q_OBJECT

public:
    explicit TexturePreviewAsyncLoader(const QString &filePath, const QPersistentModelIndex &index);

    void run() override;

signals:
    void loaded(const QString &filePath, const QPixmap &pixmap, const QPersistentModelIndex &index);

private:
    QString m_filePath;
    QPersistentModelIndex m_index;
};

} // namespace editor