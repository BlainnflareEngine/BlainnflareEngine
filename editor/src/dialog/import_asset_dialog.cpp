//
// Created by gorev on 25.09.2025.
//

#include "dialog/import_asset_dialog.h"

#include "FileSystemUtils.h"
#include "ui_import_asset_dialog.h"

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

namespace editor
{
void import_asset_dialog::CreateConfirmButtons()
{
    m_layout = new QHBoxLayout(this);

    m_btn_confirm = new QPushButton("Confirm", this);
    m_btn_confirm->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(m_btn_confirm, &QPushButton::clicked, this, &import_asset_dialog::OnConfirm);

    m_btn_cancel = new QPushButton("Cancel", this);
    m_btn_cancel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(m_btn_cancel, &QPushButton::clicked, this, &import_asset_dialog::OnCancel);

    m_layout->addWidget(m_btn_confirm);
    m_layout->addWidget(m_btn_cancel);

    layout()->addItem(m_layout);
}


import_asset_dialog::import_asset_dialog(const ImportAssetInfo &info, QWidget *parent)
    : QDialog(parent)
    , m_info(info)
    , ui(new Ui::import_asset_dialog)
{
    ui->setupUi(this);
}


import_asset_dialog::~import_asset_dialog()
{
    delete ui;
}


void import_asset_dialog::OnConfirm()
{
    accept();
}


void import_asset_dialog::OnCancel()
{
    reject();
}
} // namespace editor
