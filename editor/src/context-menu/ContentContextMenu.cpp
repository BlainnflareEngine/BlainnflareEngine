//
// Created by gorev on 23.09.2025.
//

#include "context-menu/ContentContextMenu.h"

#include "ContentFilterProxyModel.h"
#include "create_material_dialog.h"
#include "AssetManager.h"
#include "Editor.h"
#include "Engine.h"
#include "FileSystemUtils.h"
#include "random.h"

#include <QInputDialog>
#include <QMenu>
#include <QMessageBox>
#include <fstream>
#include <qabstractitemview.h>

namespace editor
{
ContentContextMenu::ContentContextMenu(QAbstractItemView &parent)
    : QObject(&parent)
    , m_parent(parent)
{
}


void ContentContextMenu::CreateFolder(const QString &dirPath) const
{
    QInputDialog *inputDialog = new QInputDialog(&m_parent);
    inputDialog->setWindowTitle("Create folder");
    inputDialog->setLabelText("Folder name:");
    inputDialog->setTextValue("");
    inputDialog->setOkButtonText("Create");
    inputDialog->setCancelButtonText("Cancel");
    inputDialog->setMinimumSize(200, 100);

    connect(inputDialog, &QInputDialog::finished, inputDialog,
            [inputDialog, dirPath, this](int result)
            {
                if (result == QDialog::Accepted)
                {
                    QString folderName = inputDialog->textValue().trimmed();
                    if (!folderName.isEmpty())
                    {
                        QDir dir(dirPath);
                        if (!dir.mkdir(folderName))
                        {
                            QMessageBox::warning(&m_parent, "Error", "Failed to create folder");
                        }
                    }
                }

                inputDialog->deleteLater();
            });

    inputDialog->open();
}


void ContentContextMenu::CreateScript(const QString &dirPath) const
{
    QInputDialog *inputDialog = new QInputDialog(&m_parent);
    inputDialog->setWindowTitle("Create script");
    inputDialog->setLabelText("Script name:");
    inputDialog->setTextValue("");
    inputDialog->setOkButtonText("Create");
    inputDialog->setCancelButtonText("Cancel");
    inputDialog->setMinimumSize(200, 100);

    connect(inputDialog, &QInputDialog::finished, inputDialog,
            [inputDialog, dirPath, this](int result)
            {
                if (result == QDialog::Accepted)
                {
                    QString scriptName = inputDialog->textValue().trimmed();
                    if (!scriptName.isEmpty())
                    {
                        if (!scriptName.endsWith("." + formats::scriptFormat, Qt::CaseInsensitive))
                            scriptName += "." + formats::scriptFormat;

                        QString filePath = dirPath + QDir::separator() + scriptName;
                        QFile script(filePath);

                        if (!script.open(QIODevice::ReadWrite))
                        {
                            QMessageBox::warning(&m_parent, "Error", "Failed to create script");
                            BF_WARN("Failed to create script");
                        }
                        else
                        {
                            QFile templateFile(":/templates/ScriptTemplate.lua");
                            if (!templateFile.open(QIODevice::ReadOnly | QIODevice::Text))
                            {
                                BF_ERROR("Failed to load Lua template!");
                                return;
                            }

                            QString templateContent = templateFile.readAll();
                            script.write(templateContent.toUtf8());
                            script.close();
                        }
                    }
                }

                inputDialog->deleteLater();
            });

    inputDialog->open();
}


void ContentContextMenu::CreateBehaviorScript(const QString &dirPath) const
{
    QInputDialog *inputDialog = new QInputDialog(&m_parent);
    inputDialog->setWindowTitle("Create Behavior script");
    inputDialog->setLabelText("Behavior script name:");
    inputDialog->setTextValue("");
    inputDialog->setOkButtonText("Create");
    inputDialog->setCancelButtonText("Cancel");
    inputDialog->setMinimumSize(200, 100);

    connect(inputDialog, &QInputDialog::finished, inputDialog,
            [inputDialog, dirPath, this](int result)
            {
                if (result == QDialog::Accepted)
                {
                    QString scriptName = inputDialog->textValue().trimmed();
                    if (!scriptName.isEmpty())
                    {
                        if (!scriptName.endsWith("." + formats::behaviorFormat, Qt::CaseInsensitive))
                            scriptName += "." + formats::behaviorFormat;

                        QString filePath = dirPath + QDir::separator() + scriptName;
                        QFile script(filePath);

                        if (!script.open(QIODevice::ReadWrite))
                        {
                            QMessageBox::warning(&m_parent, "Error", "Failed to create behavior script");
                            BF_WARN("Failed to create behavior script");
                        }
                        else
                        {
                            QFile templateFile(":/templates/BehaviorScriptTemplate.lua");
                            if (!templateFile.open(QIODevice::ReadOnly | QIODevice::Text))
                            {
                                BF_ERROR("Failed to load Lua template!");
                                return;
                            }

                            QString templateContent = templateFile.readAll();
                            script.write(templateContent.toUtf8());
                            script.close();
                        }
                    }
                }

                inputDialog->deleteLater();
            });

    inputDialog->open();
}


void ContentContextMenu::CreateMaterial(const QString &dirPath) const
{
    create_material_dialog *materialDialog = new create_material_dialog(&m_parent);

    connect(materialDialog, &QDialog::finished, materialDialog,
            [materialDialog, dirPath, this](int result)
            {
                if (result == QDialog::Accepted)
                {
                    QString materialName = materialDialog->GetMaterialName().trimmed();
                    if (materialName.isEmpty()) return;

                    QString filePath = dirPath + QDir::separator() + materialName + "." + formats::materialFormat;

                    YAML::Node config;
                    QDir contentDir(Blainn::Engine::GetContentDirectory());

                    config["ID"] = Blainn::Rand::getRandomUUID().bytes();
                    config["Path"] = ToString(contentDir.relativeFilePath(filePath));

                    auto relPath = [&contentDir](const QString &absPath) -> std::string
                    {
                        if (absPath.isEmpty()) return "";
                        return ToString(contentDir.relativeFilePath(absPath));
                    };

                    config["ShaderPath"] = relPath(materialDialog->GetShaderPath());
                    config["AlbedoPath"] = relPath(materialDialog->GetAlbedoPath());
                    config["AlbedoColor"] = ToString(materialDialog->GetAlbedoColor().name(QColor::HexArgb));
                    config["NormalPath"] = relPath(materialDialog->GetNormalPath());
                    config["NormalScale"] = materialDialog->GetNormalScale();
                    config["MetallicPath"] = relPath(materialDialog->GetMetallicPath());
                    config["MetallicScale"] = materialDialog->GetMetallicScale();
                    config["RoughnessPath"] = relPath(materialDialog->GetRoughnessPath());
                    config["RoughnessScale"] = materialDialog->GetRoughnessScale();
                    config["AOPath"] = relPath(materialDialog->GetAOPath());

                    Blainn::Path configFilePath(ToString(filePath));
                    std::ofstream fout(configFilePath.string());
                    if (!fout.is_open())
                    {
                        QMessageBox::warning(&m_parent, "Error", "Failed to create material file");
                    }
                    else
                    {
                        fout << config;
                        fout.close();
                    }
                }

                materialDialog->deleteLater();
            });

    materialDialog->open();
}


void ContentContextMenu::CreateScene(const QString &dirPath) const
{
    QInputDialog *inputDialog = new QInputDialog(&m_parent);
    inputDialog->setWindowTitle("Create scene");
    inputDialog->setLabelText("Scene name:");
    inputDialog->setTextValue("");
    inputDialog->setOkButtonText("Create");
    inputDialog->setCancelButtonText("Cancel");
    inputDialog->setMinimumSize(200, 100);

    connect(inputDialog, &QInputDialog::finished, inputDialog,
            [inputDialog, dirPath, this](int result)
            {
                if (result == QDialog::Accepted)
                {
                    using namespace Blainn;
                    // absolute
                    QString filePath =
                        dirPath + QDir::separator() + inputDialog->textValue() + "." + formats::sceneFormat;

                    Path relativePath = std::filesystem::relative(ToString(filePath), Engine::GetContentDirectory());
                    AssetManager::CreateScene(relativePath);
                }

                inputDialog->deleteLater();
            });

    inputDialog->open();
}


void ContentContextMenu::OnContextMenu(const QPoint &pos) const
{
    QModelIndex index = m_parent.indexAt(pos);
    if (index.isValid()) return;

    QFileSystemModel *fileSystemModel = nullptr;

    if (auto *proxyModel = qobject_cast<QAbstractProxyModel *>(m_parent.model()))
    {
        fileSystemModel = qobject_cast<QFileSystemModel *>(proxyModel->sourceModel());
    }
    else
    {
        fileSystemModel = qobject_cast<QFileSystemModel *>(m_parent.model());
    }

    if (!fileSystemModel) return;

    QString dirPath = fileSystemModel->rootPath();
    QMenu *menu = new QMenu(nullptr);
    QMenu *createSubMenu = menu->addMenu("Create");

    QAction *createFolderAction = createSubMenu->addAction("Folder");
    QAction *createScriptAction = createSubMenu->addAction("Script");
    QAction *createBehaviorAction = createSubMenu->addAction("Behavior script");
    QAction *createMaterialAction = createSubMenu->addAction("Material");
    QAction *createSceneAction = createSubMenu->addAction("Scene");

    menu->addSeparator();

    QAction *showExplorerAction = menu->addAction("Show in explorer");

    connect(menu, &QMenu::triggered, this,
            [this, menu, dirPath, index, createFolderAction, createScriptAction, createMaterialAction,
             showExplorerAction, createSceneAction, createBehaviorAction](const QAction *selectedAction)
            {
                if (selectedAction == createFolderAction) CreateFolder(dirPath);
                else if (selectedAction == createScriptAction) CreateScript(dirPath);
                else if (selectedAction == createMaterialAction) CreateMaterial(dirPath);
                else if (selectedAction == showExplorerAction) OpenFolderExplorer(dirPath);
                else if (selectedAction == createSceneAction) CreateScene(dirPath);
                else if (selectedAction == createBehaviorAction) CreateBehaviorScript(dirPath);
                menu->deleteLater();
            });

    connect(menu, &QMenu::aboutToHide, menu, &QMenu::deleteLater);

    menu->popup(m_parent.viewport()->mapToGlobal(pos));
}
} // namespace editor