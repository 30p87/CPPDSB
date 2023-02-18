#include "CPPDSB.h"
#include <fmt/core.h>
#include "curl_easy.h"
#include "curl_header.h"

CPPDSB::CPPDSB(const std::string &username, const std::string &password) {
    credentialParams.username = username;
    credentialParams.password = password;
    getToken();
}

std::string CPPDSB::getCredentialQuery() {
    return fmt::format("bundleid={}&appversion={}&osversion={}&pushid={}&user={}&password={}",
                       credentialParams.bundleid, credentialParams.appversion, credentialParams.osversion,
                       credentialParams.pushid, credentialParams.username, credentialParams.password);
}

std::string CPPDSB::getTokenAuthQuery() {
    return fmt::format("authid={}", tokenParams.authid);
}

std::string CPPDSB::get(const std::string &url, const std::string &params) {
    std::ostringstream str;
    curl::curl_ios<std::ostringstream> writer(str);
    curl::curl_easy easy(writer);
    easy.add<CURLOPT_URL>(fmt::format("{}?{}", url, params).c_str());
    easy.add<CURLOPT_CUSTOMREQUEST>("GET");

    try {
        easy.perform();
    } catch (curl::curl_easy_exception &error) {
        std::perror(error.what());
        error.print_traceback();
        return "";
    }

    return str.str();
}

Json::Value CPPDSB::getJson(const std::string &url, const std::string &params) {
    std::string rawJson = get(url, params);
    Json::Value out;
    JSONCPP_STRING err;
    Json::CharReaderBuilder builder;
    const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
    if (!reader->parse(rawJson.c_str(), rawJson.c_str() + rawJson.length(), &out,
                       &err)) {
        perror(err.c_str());
        return EXIT_FAILURE;
    }
    return out;
}

int CPPDSB::getToken() {
    std::string ret = get(fmt::format("{}/authid", BASE_URL), getCredentialQuery());
    if (ret == "\"\"") {
        std::perror("Invalid Credentials");
        return -1;
    }
    ret.erase(0, 1);
    ret.pop_back();
    tokenParams.authid = ret;
    return 0;
}

std::list<plan> CPPDSB::getPlans() {
    std::list<plan> plans;
    struct plan plan;
    Json::Value rawPlans = getJson(fmt::format("{}/dsbtimetables", BASE_URL), getTokenAuthQuery());
    for (const Json::Value &rawPlan: rawPlans) {
        for (const Json::Value &i: rawPlan["Childs"]) {
            plan.id = i["id"].asString();
            plan.isHtml = i["ConType"].asInt() == 6;
            plan.uploadedDate = i["Date"].asString();
            plan.title = i["Title"].asString();
            plan.url = i["Detail"].asString();
            plan.previewUrl += i["Preview"].asString();
            plans.push_back(plan);
        }
    }
    return plans;
}

std::list<news> CPPDSB::getNews() {
    std::list<news> newsl;
    struct news news;
    Json::Value rawNews = getJson(fmt::format("{}/newstab", BASE_URL), getTokenAuthQuery());
    for (const Json::Value &i: rawNews) {
        news.title = i["Title"].asString();
        news.date = i["Date"].asString();
        news.content = i["Detail"].asString();
        newsl.push_back(news);
    }
    return newsl;
}

std::list<posting> CPPDSB::getPostings() {
    std::list<posting> postings;
    struct posting posting;
    Json::Value rawPostings = getJson(fmt::format("{}/newstab", BASE_URL), getTokenAuthQuery());
    for (const Json::Value &rawPosting: rawPostings) {
        posting.id = rawPosting["id"].asString();
        posting.uploadedDate = rawPosting["Date"].asString();
        posting.title = rawPosting["Title"].asString();
        posting.url = rawPosting["Detail"].asString();
        posting.previewUrl += rawPosting["Preview"].asString();
        postings.push_back(posting);
    }
    return postings;
}