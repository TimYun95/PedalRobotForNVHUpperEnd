#include "websocketmessage.h"

#include <iostream>
#include <sstream>

const int segmentLen(32);
const char paddingChar(' ');

WebSocketMessage::WebSocketMessage()
    : IdSetting(0), srcGroupIdStr(), dstGroupIdStr(), msgType(-1), msgContent()
{

}

WebSocketMessage::~WebSocketMessage()
{
    msgContent.reset();//shared_ptr dereference
}

void WebSocketMessage::SetSrcGroupId(const std::string &srcId)
{
    srcGroupIdStr = srcId;
    IdSetting |= 0x01;
}

void WebSocketMessage::SetDstGroupId(const std::string &dstId)
{
    dstGroupIdStr = dstId;
    IdSetting |= 0x10;
}

void WebSocketMessage::SetMsgContent(const int type, const char *content, const size_t len)
{
    msgType = type;
    msgContent = std::tr1::shared_ptr<std::string> (new std::string(content, len));
}

int WebSocketMessage::GetSrcGroupId(std::string &srcId) const
{
    if( !CheckIdSetting() ){
        return -1;
    }

    srcId = srcGroupIdStr;
    return 0;
}

int WebSocketMessage::GetDstGroupId(std::string &dstId) const
{
    if( !CheckIdSetting() ){
        return -1;
    }

    dstId = dstGroupIdStr;
    return 0;
}

int WebSocketMessage::GetMsgType() const
{
    if( !CheckIdSetting() ){
        return -1;
    }

    return msgType;
}

std::string WebSocketMessage::GetMsgContent() const
{
    if( !CheckIdSetting() ){
        return "";
    }

    return std::string(msgContent->c_str(), msgContent->length());
}

int WebSocketMessage::Deformat(const std::string &msg)
{
    //每个成员变量占据segmentLen的长度 不足的部分补足paddingChar
    std::string segment[variableNum];
    for(int i=0; i<variableNum-1; ++i){
        segment[i].assign(msg.begin() + segmentLen*i, msg.begin() + segmentLen*(i+1));
    }
    segment[variableNum-1].assign(msg.begin() + segmentLen*(variableNum-1), msg.end());

    SetSrcGroupId( TrimPaddingChar(segment[0]) );
    SetDstGroupId( TrimPaddingChar(segment[1]) );
    msgType   = std::atoi( TrimPaddingChar(segment[2]).c_str() );
    msgContent= std::tr1::shared_ptr<std::string> (new std::string( segment[3] ));

    return 0;
}

int WebSocketMessage::Format(std::string &msg)
{
    if( !CheckIdSetting() ){
        return -1;
    }

    msg.clear();
    AppendSegment(msg, srcGroupIdStr);
    AppendSegment(msg, dstGroupIdStr);
    AppendSegment(msg, msgType);
    AppendSegment(msg, msgContent);

    return 0;
}

int WebSocketMessage::DeformatDstGroupId(const std::string &msg, std::string &dstId)
{
    std::string segment;
    segment.assign(msg.begin()+segmentLen, msg.begin()+2*segmentLen);//第二段
    SetDstGroupId( TrimPaddingChar(segment) );
    dstId = dstGroupIdStr;
    return 0;
}

bool WebSocketMessage::CheckIdSetting() const
{
    if((IdSetting&0x11) == 0x11){
        return true;
    }else{//some id has not been set
        return false;
    }
}

void WebSocketMessage::AppendSegment(std::string &msg, const std::string &segmentStr)
{
    std::string buf(segmentStr);
    buf.resize(segmentLen, paddingChar);

    msg.append(buf);
}

void WebSocketMessage::AppendSegment(std::string &msg, const int num)
{
    std::stringstream ss;
    ss<<num;

    std::string buf;
    ss>>buf;
    buf.resize(segmentLen, paddingChar);

    msg.append(buf);
}

void WebSocketMessage::AppendSegment(std::string &msg, const shared_string_ptr segmentStr)
{
    //密文内容的添加 需要指定长度,防止\0截断字符串
    msg.append(segmentStr->c_str(), segmentStr->length());
}

const std::string WebSocketMessage::TrimPaddingChar(const std::string &str)
{
    return str.substr(0, str.find_first_of(paddingChar));
}
