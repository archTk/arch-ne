////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2011 Mario Negri Institute & Orobix Srl
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
////////////////////////////////////////////////////////////////////////////////

#include <QtGui>

#include "treedelegate.h"

void TreeDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,const QModelIndex &index) const
{
    QStyledItemDelegate::paint(painter, option, index);
}

QSize TreeDelegate::sizeHint(const QStyleOptionViewItem &option,const QModelIndex &index) const
{
    return QStyledItemDelegate::sizeHint(option, index);
}

QWidget *TreeDelegate::createEditor(QWidget *parent,const QStyleOptionViewItem &option,const QModelIndex &index) const

{
    QStringList list = index.model()->data(index.model()->index(index.row(),4,index.parent())).toStringList();
    if (!list.isEmpty()) {
        QComboBox *editor = new QComboBox(parent);
        list.prepend("...");  
        editor->insertItems(0,list);
        return editor;
    } else {
        return QStyledItemDelegate::createEditor(parent, option, index);
    }
}

void TreeDelegate::setEditorData(QWidget *editor,const QModelIndex &index) const
{
    QStringList list = index.model()->data(index.model()->index(index.row(),4,index.parent())).toStringList();
    if (!list.isEmpty()) {
        QComboBox *comboBox = static_cast<QComboBox *>(editor);
        int currIndex = comboBox->findText(index.model()->data(index).toString())>=0 ? comboBox->findText(index.model()->data(index).toString()): 0;
        comboBox->setCurrentIndex(currIndex);
    } else {
        QStyledItemDelegate::setEditorData(editor, index);
    }
}

void TreeDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,const QModelIndex &index) const
{
    QStringList list = index.model()->data(index.model()->index(index.row(),4,index.parent())).toStringList();
    if (!list.isEmpty()) {
        QComboBox *comboBox = static_cast<QComboBox*>(editor);
        QString value = comboBox->currentText()=="..." ? "": comboBox->currentText();
        model->setData(index, value, Qt::EditRole);
    } else {
        QStyledItemDelegate::setModelData(editor, model, index);
    }
}
