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

#ifndef TREEITEM_H
#define TREEITEM_H

#include <QList>
#include <QVariant>
#include <QVector>

class TreeItem
{
public:
    TreeItem(const QVector<QVariant> &data, TreeItem *parent = 0);
    ~TreeItem();

    bool removeChildren(int position, int count);
    bool removeColumns(int position, int columns);
    bool setData(int column, const QVariant &value);
    bool insertChildren(int position, int count, int columns);
    bool insertColumns(int position, int columns);
    int childCount() const;
    int childNumber() const;
    int columnCount() const;
    QVariant data(int column) const;
    TreeItem *child(int number);
    TreeItem *parent();

private:
    QList<TreeItem*> childItems;
    QVector<QVariant> itemData;
    TreeItem *parentItem;
};

#endif
