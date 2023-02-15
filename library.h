#ifndef CPPDSB_LIBRARY_H
#define CPPDSB_LIBRARY_H

#include <string>
#include <list>
#include <json/json.h>

#define BASE_URL "https://mobileapi.dsbcontrol.de"

struct credentialAuthParams {
    std::string bundleid = "de.heinekingmedia.dsbmobile";
    std::string appversion = "35";
    std::string osversion = "22";
    std::string pushid;
    std::string username;
    std::string password;
};

struct tokenAuthParams {
    std::string authid;
};

struct plan {
    std::string id;
    bool isHtml;
    std::string uploadedDate;
    std::string title;
    std::string url;
    std::string previewUrl = "https://light.dsbcontrol.de/DSBlightWebsite/Data/";
};

struct news {
    std::string title;
    std::string date;
    std::string content;
};

struct posting {
    std::string id;
    std::string uploadedDate;
    std::string title;
    std::string url;
    std::string previewUrl = "https://light.dsbcontrol.de/DSBlightWebsite/Data/";
};

class CPPDSB {
public:
    explicit CPPDSB(const std::string& username = "", const std::string& password = "");
    std::list<plan> getPlans();
    std::list<news> getNews();
    std::list<posting> getPostings();
private:
    struct credentialAuthParams credentialParams;
    struct tokenAuthParams tokenParams;
    std::string getCredentialQuery();
    std::string getTokenAuthQuery();
    static std::string get(const std::string& url, const std::string& params);
    static Json::Value getJson(const std::string& url, const std::string& params);
    int getToken();
};

#endif //CPPDSB_LIBRARY_H
