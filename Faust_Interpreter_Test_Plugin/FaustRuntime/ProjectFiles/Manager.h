#ifndef PROJECT_FILES_MANAGER_H
#define PROJECT_FILES_MANAGER_H

#include <string>
#include <vector>
#include <set>

class ProjectFilesManager
{
public:
    ProjectFilesManager(std::string&);

    std::vector<std::string> getProjectFiles() const;

    bool load(const std::string&, std::wstring&);

    bool create(std::string&);

    bool writeFile(const std::string&, const std::wstring& contents) const;

    bool rename(const std::string&, const std::string&);

    bool remove(const std::string&);

    bool pfNameUnique(const std::string&);

private:

    std::string projectFilesDir;
    std::set<std::string> project_files;
    std::set<size_t> usedUntitledNumbers;
    std::string untitled_filename;    

    void scanProjectFileDir();
    void updateUntitledSet(const std::string&);
    size_t getNextUntitledNumber();
    bool removeUntitledNumber(const std::string&);
    std::string getFilePath(const std::string&) const;
};

#endif
