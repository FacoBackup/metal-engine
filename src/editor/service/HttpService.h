#ifndef HTTPSERVICE_H
#define HTTPSERVICE_H

#include "../../common/IService.h"
#include <string>
#include <functional>

namespace Metal {
    class HttpService final : public IService {
    public:
        HttpService() = default;
        ~HttpService() override = default;

        void post(const std::string& url, const std::string& body, const std::string& token, 
                  const std::function<void(const std::string&, bool)>& callback);
    };
}

#endif
