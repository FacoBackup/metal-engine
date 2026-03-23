#ifndef HTTPSERVICE_H
#define HTTPSERVICE_H

#include "common/IService.h"
#include <string>
#include <functional>

namespace Metal {
    struct HttpService final : IService {
        void post(const std::string &url, const std::string &body, const std::string &token,
                  const std::function<void(const std::string &, bool)> &callback);
    };
}

#endif
