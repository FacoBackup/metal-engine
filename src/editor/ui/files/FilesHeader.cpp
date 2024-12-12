#include "FilesHeader.h"

#include "../../../common/interface/Icons.h"
#include "../../../common/util/files/FilesUtil.h"
#include "../../../context/ApplicationContext.h"
#include "../../common/UIUtil.h"

namespace Metal {
    void FilesHeader::onSync() {
        if (UIUtil::ButtonSimple(Icons::folder, UIUtil::ONLY_ICON_BUTTON_SIZE, UIUtil::ONLY_ICON_BUTTON_SIZE)) {
            FilesUtil::MkDir(filesContext->currentDirectory->absolutePath + "/new-directory");
            FilesUtil::GetDirectoriesFromRoot(*filesContext->currentDirectory);
            filesContext->entriesCurrentDir = FilesUtil::GetEntries(context->getRootDirectory());
        }
    }
} // Metal
