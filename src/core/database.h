#pragma once

#include <QString>
#include <QVector>

#include "project.h"

// Cơ sở dữ liệu JSON đơn giản cho danh sách dự án.
// File lưu tại ~/.config/weblauncher/projects.json
class Database {
public:
    Database();

    QVector<Project> projects() const { return m_projects; }
    Project* find(const QString& id);
    const Project* find(const QString& id) const;

    bool addProject(const Project& p);            // thêm mới
    void updateProject(const Project& p);         // cập nhật bản ghi có sẵn
    void removeProject(const QString& id);
    bool containsPath(const QString& path) const;

    // tải / lưu từ đĩa
    void load();
    void save() const;

    static QString databasePath();

private:
    QVector<Project> m_projects;
};
