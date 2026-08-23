#pragma once
#include "../files/json.hpp"
#include <curl/curl.h>
#include <cctype>
#include <map>
#include <mutex>
#include <stdexcept>
#include <string>

namespace requests {
using Headers=std::map<std::string,std::string>;
using Params=std::map<std::string,std::string>;
class RequestError:public std::runtime_error{public:using std::runtime_error::runtime_error;};
class Response{
public:
    int status_code=0;std::string text,url;Headers headers;std::string error;
    bool ok()const{return status_code>=200&&status_code<400&&error.empty();}
    ::json::Value json()const{return ::json::loads(text);}
    void raise_for_status()const{if(!error.empty())throw RequestError(error);if(!ok())throw RequestError("HTTP "+std::to_string(status_code)+" for "+url);}
};
struct Options{Headers headers;Params params;int timeout_ms=30000;bool follow_redirects=true;bool verify_ssl=true;};
namespace detail {
inline void init(){static std::once_flag once;std::call_once(once,[]{if(curl_global_init(CURL_GLOBAL_DEFAULT)!=CURLE_OK)throw RequestError("Could not initialize libcurl");});}
inline size_t body(char*d,size_t s,size_t n,void*out){static_cast<std::string*>(out)->append(d,s*n);return s*n;}
inline std::string trim(std::string s){while(!s.empty()&&std::isspace((unsigned char)s.front()))s.erase(s.begin());while(!s.empty()&&std::isspace((unsigned char)s.back()))s.pop_back();return s;}
inline size_t header(char*d,size_t s,size_t n,void*out){std::string line(d,s*n);auto p=line.find(':');if(p!=std::string::npos)(*static_cast<Headers*>(out))[trim(line.substr(0,p))]=trim(line.substr(p+1));return s*n;}
inline std::string make_url(CURL*c,const std::string&url,const Params&params){std::string out=url;bool first=url.find('?')==std::string::npos;for(auto&[k,v]:params){char*ek=curl_easy_escape(c,k.c_str(),int(k.size()));char*ev=curl_easy_escape(c,v.c_str(),int(v.size()));out+=first?'?':'&';first=false;out+=ek;out+='=';out+=ev;curl_free(ek);curl_free(ev);}return out;}
inline Response send(const std::string&method,const std::string&address,const std::string&data,const Options&options){
    init();CURL*c=curl_easy_init();if(!c)throw RequestError("Could not create HTTP request");
    Response r;r.url=make_url(c,address,options.params);char errors[CURL_ERROR_SIZE]={};curl_slist*list=nullptr;
    for(auto&[k,v]:options.headers)list=curl_slist_append(list,(k+": "+v).c_str());
    curl_easy_setopt(c,CURLOPT_URL,r.url.c_str());curl_easy_setopt(c,CURLOPT_CUSTOMREQUEST,method.c_str());
    curl_easy_setopt(c,CURLOPT_FOLLOWLOCATION,options.follow_redirects?1L:0L);curl_easy_setopt(c,CURLOPT_TIMEOUT_MS,long(options.timeout_ms));
    curl_easy_setopt(c,CURLOPT_SSL_VERIFYPEER,options.verify_ssl?1L:0L);curl_easy_setopt(c,CURLOPT_SSL_VERIFYHOST,options.verify_ssl?2L:0L);
    curl_easy_setopt(c,CURLOPT_ERRORBUFFER,errors);curl_easy_setopt(c,CURLOPT_WRITEFUNCTION,body);curl_easy_setopt(c,CURLOPT_WRITEDATA,&r.text);
    curl_easy_setopt(c,CURLOPT_HEADERFUNCTION,header);curl_easy_setopt(c,CURLOPT_HEADERDATA,&r.headers);if(list)curl_easy_setopt(c,CURLOPT_HTTPHEADER,list);
    if(!data.empty()||method=="POST"||method=="PUT"||method=="PATCH"){curl_easy_setopt(c,CURLOPT_POSTFIELDS,data.data());curl_easy_setopt(c,CURLOPT_POSTFIELDSIZE,long(data.size()));}
    CURLcode result=curl_easy_perform(c);long status=0;curl_easy_getinfo(c,CURLINFO_RESPONSE_CODE,&status);r.status_code=int(status);
    char*final_url=nullptr;curl_easy_getinfo(c,CURLINFO_EFFECTIVE_URL,&final_url);if(final_url)r.url=final_url;
    if(result!=CURLE_OK)r.error=errors[0]?errors:curl_easy_strerror(result);curl_slist_free_all(list);curl_easy_cleanup(c);return r;
}}
inline Response get(const std::string&u,const Options&o={}){return detail::send("GET",u,{},o);}
inline Response post(const std::string&u,const std::string&d={},Options o={}){return detail::send("POST",u,d,o);}
inline Response post(const std::string&u,const json::Value&v,Options o={}){o.headers["Content-Type"]="application/json";return detail::send("POST",u,json::dumps(v,0),o);}
inline Response put(const std::string&u,const std::string&d={},Options o={}){return detail::send("PUT",u,d,o);}
inline Response patch(const std::string&u,const std::string&d={},Options o={}){return detail::send("PATCH",u,d,o);}
inline Response del(const std::string&u,const Options&o={}){return detail::send("DELETE",u,{},o);}
}
