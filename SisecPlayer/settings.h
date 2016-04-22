#ifndef SETTINGS_H
#define SETTINGS_H
#include <map>
#include <QString>

#define  SETTIN_TABLE_NAME  "SettingInfo"

#define  KEY_SERVER         "server_ip"
#define  KEY_CURVERSION     "cur_verstion"
#define  KEY_MD5            "cur_md5"
#define  KEY_FILE_URL       "cur_file_url"

#define  KEY_DOWNLOAD_VERSION   "download_verstion"
#define  KEY_DOWNLOAD_MD5       "download_md5"
#define  KEY_DOWNLOAD_FILE_URL  "download_file_url"

#define  KEY_SERVER_VERSION   "server_verstion"
#define  KEY_SERVER_MD5       "server_md5"
#define  KEY_SERVER_FILE_URL  "server_file_url"

#define  KEY_UPGRADE_STATE      "upgrade_state"
#define  KEY_UPGRADE_STATE_INSTALLED        ""
#define  KEY_UPGRADE_STATE_DOWNLOADING      "2"
#define  KEY_UPGRADE_STATE_DOWNLOADED       "3"
#define  KEY_UPGRADE_STATE_INSTALLING       "4"

#define  KEY_LOGO_FILE              "logo_file_path"
#define  KEY_PRDUCT_NAME            "product_name"

#define  KEY_PRODUCT_INFO              "product_info"
#define  KEY_PRODUCT_INFO_EN            "product_info_en"

#define  KEY_PASSWORD_TA            "pass_word_ta"
#define  KEY_TIME_TA                "time_ta"
#define  KEY_CERTIFY_TA             "certify_ta"
#define  KEY_LAST_TA                "l_ta"

#define  KEY_NET_IP_SEARCH          "net_ip_search"
#define  NET_IP_SEARCH_SPLIT        "|"
class Settings
{
private:
    Settings();
public:
    static void createTable();
    static bool setItem(const QString &key, const QString &value);
    static QString getItem(const QString &key);
};

#endif // SETTINGS_H
