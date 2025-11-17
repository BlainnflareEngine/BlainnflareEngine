//
// Created by gorev on 17.11.2025.
//

#pragma once

#include <QLineEdit>

namespace editor
{

class path_input_field : public QLineEdit
{
    Q_OBJECT

public:
    explicit path_input_field(QWidget *parent = nullptr);
    explicit path_input_field(const QStringList &acceptedExtensions, QWidget *parent = nullptr);

    void SetExtensions(const QStringList &extensions);
    QStringList GetExtensions() const;

    QString GetPath() const;
    QString GetAbsolutePath() const;
    void SetPath(const QString &absolutePath);

signals:
    void PathChanged(const QString &path);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    bool IsFileAccepted(const QString &filePath) const;


    QStringList m_acceptedExtensions;
};

} // namespace editor