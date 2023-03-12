#include <string>
std::string getUnixPath(const std::string& url) {
    if (url.rfind("file:///", 0) == 0) {
        return url.substr(7);
    }
    return url;
}