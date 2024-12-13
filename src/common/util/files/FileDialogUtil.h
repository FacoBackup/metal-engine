#ifndef FILEDIALOGUTIL_H
#define FILEDIALOGUTIL_H
#include <iostream>
#include <string>
#include <vector>
#include <nfd.hpp>

namespace Metal:: FileDialogUtil {
    static std::vector<std::string> PickFiles(std::vector<nfdu8filteritem_t> filtersToApply) {
        std::vector<std::string> files;
        NFD::Guard nfdGuard;
        NFD::UniquePathSet outPaths;

        nfdresult_t result = NFD::OpenDialogMultiple(outPaths, filtersToApply.data(), 2);
        if (result == NFD_OKAY) {
            std::cout << "Success!" << std::endl;

            nfdpathsetsize_t numPaths;
            NFD::PathSet::Count(outPaths, numPaths);

            nfdpathsetsize_t i;
            for (i = 0; i < numPaths; ++i) {
                NFD::UniquePathSetPath path;
                NFD::PathSet::GetPath(outPaths, i, path);
                files.emplace_back(path.get());
            }
        }
        return files;
    }

    static std::string SelectDirectory() {
        NFD::Guard nfdGuard;
        NFD::UniquePath outPath;

        nfdresult_t result = NFD::PickFolder(outPath);
        if (result == NFD_OKAY) {
            return outPath.get();
        }
        return "";
    }
} // Metal

#endif //FILEDIALOGUTIL_H
