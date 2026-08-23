#pragma once
#include "pathlib.hpp"
#include <cctype>
#include <cmath>
#include <iomanip>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

namespace json {
class Value {
public:
    using ArrayType = std::vector<Value>;
    using ObjectType = std::map<std::string, Value>;
    Value() : data_(nullptr) {}
    Value(std::nullptr_t) : data_(nullptr) {}
    Value(bool v) : data_(v) {}
    Value(int v) : data_(double(v)) {}
    Value(long v) : data_(double(v)) {}
    Value(long long v) : data_(double(v)) {}
    Value(double v) : data_(v) {}
    Value(const char* v) : data_(std::string(v)) {}
    Value(std::string v) : data_(std::move(v)) {}
    Value(ArrayType v) : data_(std::move(v)) {}
    Value(ObjectType v) : data_(std::move(v)) {}
    bool is_null() const { return std::holds_alternative<std::nullptr_t>(data_); }
    bool is_bool() const { return std::holds_alternative<bool>(data_); }
    bool is_number() const { return std::holds_alternative<double>(data_); }
    bool is_string() const { return std::holds_alternative<std::string>(data_); }
    bool is_array() const { return std::holds_alternative<ArrayType>(data_); }
    bool is_object() const { return std::holds_alternative<ObjectType>(data_); }
    bool as_bool() const { return get<bool>("boolean"); }
    double as_number() const { return get<double>("number"); }
    int as_int() const { return static_cast<int>(as_number()); }
    const std::string& as_string() const { return get<std::string>("string"); }
    const ArrayType& as_array() const { return get<ArrayType>("array"); }
    ArrayType& as_array() { return get<ArrayType>("array"); }
    const ObjectType& as_object() const { return get<ObjectType>("object"); }
    ObjectType& as_object() { return get<ObjectType>("object"); }
    std::string toString() const { return as_string(); }
    int toInt() const { return as_int(); }
    bool toBool() const { return as_bool(); }
    ArrayType toArray() const { return as_array(); }
    ObjectType toObject() const { return as_object(); }
    Value& operator[](const std::string& key) { if (is_null()) data_ = ObjectType{}; return as_object()[key]; }
    const Value& operator[](const std::string& key) const { return as_object().at(key); }
    Value& operator[](std::size_t i) { return as_array().at(i); }
    const Value& operator[](std::size_t i) const { return as_array().at(i); }
    std::size_t size() const { if (is_array()) return as_array().size(); if (is_object()) return as_object().size(); if (is_string()) return as_string().size(); return 0; }
    void push_back(Value v) { as_array().push_back(std::move(v)); }
    bool contains(const std::string& key) const { return is_object() && as_object().count(key); }
private:
    template<class T> const T& get(const char* type) const { auto p=std::get_if<T>(&data_); if(!p) throw std::runtime_error(std::string("JSON value is not a ")+type); return *p; }
    template<class T> T& get(const char* type) { auto p=std::get_if<T>(&data_); if(!p) throw std::runtime_error(std::string("JSON value is not a ")+type); return *p; }
    std::variant<std::nullptr_t,bool,double,std::string,ArrayType,ObjectType> data_;
};
using Object=Value::ObjectType;
using Array=Value::ArrayType;

namespace detail {
inline void utf8(std::string& out, unsigned n) {
    if(n<=0x7f) out+=char(n); else if(n<=0x7ff){out+=char(0xc0|(n>>6));out+=char(0x80|(n&63));}
    else {out+=char(0xe0|(n>>12));out+=char(0x80|((n>>6)&63));out+=char(0x80|(n&63));}
}
class Parser {
public:
    explicit Parser(const std::string& s):s_(s){}
    Value parse(){space();auto v=value();space();if(p_!=s_.size())fail("trailing text");return v;}
private:
    Value value(){if(p_>=s_.size())fail("expected value");char c=s_[p_];if(c=='{')return object();if(c=='[')return array();if(c=='\"')return string();if(c=='t'){word("true");return true;}if(c=='f'){word("false");return false;}if(c=='n'){word("null");return nullptr;}if(c=='-'||std::isdigit((unsigned char)c))return number();fail("unexpected character");}
    Object object(){Object o;++p_;space();if(take('}'))return o;for(;;){if(p_>=s_.size()||s_[p_]!='\"')fail("expected key");auto k=string();space();if(!take(':'))fail("expected ':'");space();o[std::move(k)]=value();space();if(take('}'))return o;if(!take(','))fail("expected ','");space();}}
    Array array(){Array a;++p_;space();if(take(']'))return a;for(;;){a.push_back(value());space();if(take(']'))return a;if(!take(','))fail("expected ','");space();}}
    std::string string(){++p_;std::string out;while(p_<s_.size()){char c=s_[p_++];if(c=='\"')return out;if((unsigned char)c<32)fail("control character");if(c!='\\'){out+=c;continue;}if(p_>=s_.size())fail("unfinished escape");switch(c=s_[p_++]){case '\"':out+='\"';break;case '\\':out+='\\';break;case '/':out+='/';break;case 'b':out+='\b';break;case 'f':out+='\f';break;case 'n':out+='\n';break;case 'r':out+='\r';break;case 't':out+='\t';break;case 'u':utf8(out,hex4());break;default:fail("bad escape");}}fail("unterminated string");}
    unsigned hex4(){if(p_+4>s_.size())fail("bad unicode escape");unsigned n=0;for(int i=0;i<4;++i){char c=s_[p_++];n<<=4;if(c>='0'&&c<='9')n+=c-'0';else if(c>='a'&&c<='f')n+=c-'a'+10;else if(c>='A'&&c<='F')n+=c-'A'+10;else fail("bad unicode escape");}return n;}
    double number(){auto start=p_;if(take('-')){}if(take('0')){}else{if(p_>=s_.size()||!std::isdigit((unsigned char)s_[p_]))fail("bad number");while(p_<s_.size()&&std::isdigit((unsigned char)s_[p_]))++p_;}if(take('.')){if(p_>=s_.size()||!std::isdigit((unsigned char)s_[p_]))fail("bad number");while(p_<s_.size()&&std::isdigit((unsigned char)s_[p_]))++p_;}if(p_<s_.size()&&(s_[p_]=='e'||s_[p_]=='E')){++p_;if(p_<s_.size()&&(s_[p_]=='+'||s_[p_]=='-'))++p_;if(p_>=s_.size()||!std::isdigit((unsigned char)s_[p_]))fail("bad exponent");while(p_<s_.size()&&std::isdigit((unsigned char)s_[p_]))++p_;}return std::stod(s_.substr(start,p_-start));}
    void word(const char* w){while(*w)if(p_>=s_.size()||s_[p_++]!=*w++)fail("bad literal");}
    bool take(char c){if(p_<s_.size()&&s_[p_]==c){++p_;return true;}return false;}
    void space(){while(p_<s_.size()&&std::isspace((unsigned char)s_[p_]))++p_;}
    [[noreturn]]void fail(const std::string& m)const{throw std::runtime_error("Invalid JSON at offset "+std::to_string(p_)+": "+m);}
    const std::string&s_;std::size_t p_=0;
};
inline std::string escaped(const std::string&s){std::ostringstream o;o<<'\"';for(unsigned char c:s){switch(c){case '\"':o<<"\\\"";break;case '\\':o<<"\\\\";break;case '\b':o<<"\\b";break;case '\f':o<<"\\f";break;case '\n':o<<"\\n";break;case '\r':o<<"\\r";break;case '\t':o<<"\\t";break;default:if(c<32)o<<"\\u"<<std::hex<<std::setw(4)<<std::setfill('0')<<int(c)<<std::dec;else o<<char(c);}}o<<'\"';return o.str();}
inline void write(const Value&v,std::ostringstream&o,int indent,int level){auto pad=[&](int n){if(indent)o<<std::string(n*indent,' ');};if(v.is_null())o<<"null";else if(v.is_bool())o<<(v.as_bool()?"true":"false");else if(v.is_number()){if(!std::isfinite(v.as_number()))throw std::runtime_error("JSON cannot represent NaN or infinity");o<<std::setprecision(15)<<v.as_number();}else if(v.is_string())o<<escaped(v.as_string());else if(v.is_array()){o<<'[';auto&a=v.as_array();for(size_t i=0;i<a.size();++i){if(i)o<<',';if(indent){o<<'\n';pad(level+1);}write(a[i],o,indent,level+1);}if(indent&&!a.empty()){o<<'\n';pad(level);}o<<']';}else{o<<'{';size_t i=0;for(auto&[k,x]:v.as_object()){if(i++)o<<',';if(indent){o<<'\n';pad(level+1);}o<<escaped(k)<<(indent?": ":":");write(x,o,indent,level+1);}if(indent&&!v.as_object().empty()){o<<'\n';pad(level);}o<<'}';}}
}
inline Value loads(const std::string&s){return detail::Parser(s).parse();}
inline std::string dumps(const Value&v,int indent=4){std::ostringstream o;detail::write(v,o,indent,0);return o.str();}
inline Value load(const pathlib::Path&p){return loads(p.read_text());}
inline void dump(const Value&v,const pathlib::Path&p,int indent=4){p.write_text(dumps(v,indent));}
}
