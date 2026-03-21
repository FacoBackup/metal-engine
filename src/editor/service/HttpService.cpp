#include "HttpService.h"
#include "../../common/LoggerUtil.h"
// #define CPPHTTPLIB_OPENSSL_SUPPORT
#include <httplib.h>
#include <iostream>
#include <vector>

namespace Metal {
    void HttpService::post(const std::string& url, const std::string& body, const std::string& token, 
                           const std::function<void(const std::string&, bool)>& callback) {
        
        LOG_INFO("HttpService: Posting to " + url);

        // Simple URL parsing to extract host and path
        size_t host_start = url.find("://");
        if (host_start == std::string::npos) {
            LOG_ERROR("HttpService: Invalid URL (no protocol): " + url);
            callback("Invalid URL", false);
            return;
        }
        host_start += 3;

        size_t path_start = url.find('/', host_start);
        std::string host = (path_start == std::string::npos) ? url.substr(0) : url.substr(0, path_start);
        std::string path = (path_start == std::string::npos) ? "/" : url.substr(path_start);

        httplib::Client cli(host);
        
        httplib::Headers headers = {
            { "Content-Type", "application/json" }
        };
        
        if (!token.empty()) {
            headers.emplace("Authorization", "Bearer " + token);
        }

        auto res = cli.Post(path.c_str(), headers, body, "application/json");

        if (res) {
            if (res->status >= 200 && res->status < 300) {
                callback(res->body, true);
            } else {
                LOG_ERROR("HttpService: Request failed with status: " + std::to_string(res->status));
                LOG_ERROR("HttpService: Response body: " + res->body);
                callback(res->body, false);
            }
        } else {
            auto err = res.error();
            LOG_ERROR("HttpService: Request failed with error code: " + std::to_string(static_cast<int>(err)));
            callback("Request failed", false);
        }
    }
}
