#ifndef HTTPSERVICE_H
#define HTTPSERVICE_H

#include "../../common/IService.h"
#include <string>
#include <functional>

namespace Metal {
    class HttpService final : public IService {
    public:
        void postStream(const std::string &url, const std::string &body, const std::string &token,
                        const std::function<bool(const char *data, size_t data_length)> &content_callback,
                        const std::function<void(const std::string &, bool)> &status_callback);
    };
}

#endif
