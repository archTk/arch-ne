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

#ifndef TREEMODEL_H
#define TREEMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <QDomDocument>

class TreeItem;

class TreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    TreeModel(QObject *parent = 0);
    ~TreeModel();

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::EditRole);
    bool insertColumns(int position, int columns, const QModelIndex &parent = QModelIndex());
    bool removeColumns(int position, int columns, const QModelIndex &parent = QModelIndex());
    bool insertRows(int position, int rows, const QModelIndex &parent = QModelIndex());
    bool removeRows(int position, int rows, const QModelIndex &parent = QModelIndex());
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation,int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

public slots:
    void setErrorLine(int line);

private:
    TreeItem *getItem(const QModelIndex &index) const;
    TreeItem *rootItem;
    void setupModelData(const QStringList &lines, TreeItem *parent);
    int errorLine;
};

#endif
