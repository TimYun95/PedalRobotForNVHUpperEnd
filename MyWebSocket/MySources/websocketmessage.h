#ifndef WEBSOCKETMESSAGE_H
#define WEBSOCKETMESSAGE_H

#include "MyWebSocket/mytypedefine.h"

/* WebSocket通讯消息格式化
 * 该类格式化所发送的消息(类似TCP通讯时添加TCP头部)
 * 格式=| srcGroupIdStr | dstGroupIdStr | msgType | msgContent |
 * 长度=      32B              32B          32B      动态长度 */

class WebSocketMessage
{
public:
    WebSocketMessage();
    virtual ~WebSocketMessage();

    void SetSrcGroupId(const std::string& srcId);
    void SetDstGroupId(const std::string& dstId);
    void SetMsgContent(const int type, const char* content, const size_t len);

    int GetSrcGroupId(std::string& srcId) const;
    int GetDstGroupId(std::string& dstId) const;
    int GetMsgType() const;
    std::string GetMsgContent() const;

    int Deformat(const std::string& msg);
    int Format(std::string& msg);

    //仅获取msg中的dstGroupIdStr
    int DeformatDstGroupId(const std::string& msg, std::string& dstId);

private:
    bool CheckIdSetting() const;//return true, if ids are ok

    void AppendSegment(std::string& msg, const std::string& segmentStr);
    void AppendSegment(std::string& msg, const int num);
    void AppendSegment(std::string& msg, const shared_string_ptr segmentStr);
    const std::string TrimPaddingChar(const std::string& str);

private:
    int IdSetting;//检查是否已设置来源及目的Id

    static const int variableNum = 4;
    std::string srcGroupIdStr;//消息来源组Id
    std::string dstGroupIdStr;//消息目的组Id
    int msgType;
    shared_string_ptr msgContent;//消息内容
};

#endif // WEBSOCKETMESSAGE_H
