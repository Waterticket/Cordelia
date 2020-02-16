// Nginx Setting 1.0
// Made by KYS
#pragma once
#ifndef N_S_DEFINED
#define N_S_DEFINED
#define TAB_SPACE "    "
#include <random>
#include <algorithm>

class NginxSetting
{
    public:
    struct Ngx_Setting;
    std::string write_setting(Ngx_Setting st);
    std::string gen_hash(int len = 16);
    std::string hash_preset(std::string prefix);

    NginxSetting()
    {

    }
};

struct NginxSetting::Ngx_Setting
{
    std::string server_name = "localhost";
    int http_port = -1;
    int https_port = -1;
    bool is_ssl = false;
    bool is_hsts = false;
    int hsts_length = 15768000;
    bool is_http2 = false;
    std::string ssl_certificate = "";
    std::string ssl_certificate_key = "";
    std::string ssl_protocols = "TLSv1 TLSv1.1 TLSv1.2";
    std::string ssl_ciphers = "ECDHE-RSA-AES128-GCM-SHA256:ECDHE-ECDSA-AES128-GCM-SHA256:ECDHE-RSA-AES256-GCM-SHA384:ECDHE-ECDSA-AES256-GCM-SHA384:DHE-RSA-AES128-GCM-SHA256:DHE-DSS-AES128-GCM-SHA256:kEDH+AESGCM:ECDHE-RSA-AES128-SHA256:ECDHE-ECDSA-AES128-SHA256:ECDHE-RSA-AES128-SHA:ECDHE-ECDSA-AES128-SHA:ECDHE-RSA-AES256-SHA384:ECDHE-ECDSA-AES256-SHA384:ECDHE-RSA-AES256-SHA:ECDHE-ECDSA-AES256-SHA:DHE-RSA-AES128-SHA256:DHE-RSA-AES128-SHA:DHE-DSS-AES128-SHA256:DHE-RSA-AES256-SHA256:DHE-DSS-AES256-SHA:DHE-RSA-AES256-SHA:AES128-GCM-SHA256:AES256-GCM-SHA384:AES128-SHA256:AES256-SHA256:AES128-SHA:AES256-SHA:AES:CAMELLIA:DES-CBC3-SHA:!aNULL:!eNULL:!EXPORT:!DES:!RC4:!MD5:!PSK:!aECDH:!EDH-DSS-DES-CBC3-SHA:!EDH-RSA-DES-CBC3-SHA:!KRB5-DES-CBC3-SHA";
    bool lets_encrypt_auto_enable = false;
    bool http_redirect = false;
    std::string dir_root = "/var/packages/WebStation/target/ready";
    std::string file_index = "index.php index.html index.htm index.cgi";
    std::string error_page = "400 401 402 403 404 405 406 407 408 500 501 502 503 504 505 @error_page";
    std::string dir_hash = "xxxx-xxxx-xxxx";
};

std::string NginxSetting::hash_preset(std::string prefix)
{
    return prefix + "_" + NginxSetting::gen_hash(8) + "-" + NginxSetting::gen_hash(4) + "-" + NginxSetting::gen_hash(4) + "-" + NginxSetting::gen_hash(4) + "-" + NginxSetting::gen_hash(12);
}

std::string NginxSetting::gen_hash(int len)
{
    std::string str("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");

    std::random_device rd;
    std::mt19937 generator(rd());

    std::shuffle(str.begin(), str.end(), generator);

    return str.substr(0, len);
}

std::string NginxSetting::write_setting(Ngx_Setting st)
{
    std::string Setting_str = "";
    Setting_str.append("server{");
    Setting_str.append("\n");

    //http
    if(st.http_port != -1){
    Setting_str.append("\n");
    Setting_str.append(TAB_SPACE);
    Setting_str += "listen " + std::to_string(st.http_port);
    Setting_str.append(";\n");
    Setting_str.append(TAB_SPACE);
    Setting_str += "listen [::]:" + std::to_string(st.http_port);
    Setting_str.append(";\n\n");
    }
    //http end

    //https
    if(st.https_port != -1 && st.is_ssl == true){
    Setting_str.append(TAB_SPACE);
    Setting_str += "listen " + std::to_string(st.https_port);
    if(st.is_ssl) Setting_str += " ssl";
    if(st.is_http2) Setting_str += " http2";
    Setting_str.append(";\n");
    Setting_str.append(TAB_SPACE);
    Setting_str += "listen [::]:" + std::to_string(st.https_port);
    if(st.is_ssl) Setting_str += " ssl";
    if(st.is_http2) Setting_str += " http2";
    Setting_str.append(";\n\n");
    }
    //https end

    Setting_str.append(TAB_SPACE);
    Setting_str += "server_name " + st.server_name;
    Setting_str.append(";\n\n");

    Setting_str.append(TAB_SPACE);
    Setting_str += "root " + st.dir_root;
    Setting_str.append(";\n");
    Setting_str.append(TAB_SPACE);
    Setting_str += "index " + st.file_index;
    Setting_str.append(";\n");
    Setting_str.append(TAB_SPACE);
    Setting_str += "error_page " + st.error_page;
    Setting_str.append(";\n\n");

    if(st.is_ssl == true)
    {
        if(st.lets_encrypt_auto_enable == true)
        {
        st.ssl_certificate = "/usr/local/etc/certificate/WebStation/vhost-"+st.dir_hash+"/fullchain.pem";
        st.ssl_certificate_key = "/usr/local/etc/certificate/WebStation/vhost-"+st.dir_hash+"/privkey.pem";
        }

        if(st.ssl_certificate != ""){
        Setting_str.append(TAB_SPACE);
        Setting_str += "ssl_certificate " + st.ssl_certificate;
        Setting_str.append(";\n");
        }
        if(st.ssl_certificate_key != ""){
        Setting_str.append(TAB_SPACE);
        Setting_str += "ssl_certificate_key " + st.ssl_certificate_key;
        Setting_str.append(";\n\n");
        }
        if(st.ssl_protocols != ""){
        Setting_str.append(TAB_SPACE);
        Setting_str += "ssl_protocols " + st.ssl_protocols;
        Setting_str.append(";\n");
        }
        if(st.ssl_ciphers != ""){
        Setting_str.append(TAB_SPACE);
        Setting_str += "ssl_ciphers " + st.ssl_ciphers;
        Setting_str.append(";\n");
        Setting_str.append(TAB_SPACE);
        Setting_str += "ssl_prefer_server_ciphers   on;\n\n";
        }
        if(st.is_hsts == true)
        {
        Setting_str.append(TAB_SPACE);
        Setting_str += "add_header  Strict-Transport-Security max-age="+ std::to_string(st.hsts_length) +";\n\n";
        }
        if(st.lets_encrypt_auto_enable == true)
        {
        Setting_str += "    location ^~ /.well-known/acme-challenge {\n        root /var/lib/letsencrypt;\n        default_type text/plain;\n    }\n\n";
        }
        if(st.http_redirect == true)
        {
        Setting_str += "    if ($server_port = \"80\") {\n        return 301 https://$server_name$request_uri;\n    }\n\n";
        }
    }
    Setting_str.append(TAB_SPACE);
    Setting_str += "location @error_page {\n        root /var/packages/WebStation/target/error_page;\n        rewrite ^ /$status.html break;\n    }\n\n";
    Setting_str.append(TAB_SPACE);
    Setting_str += "include /usr/local/etc/nginx/conf.d/"+st.dir_hash+"/user.conf*;\n\n";

    Setting_str += "}\n";
    return Setting_str;
}
#endif