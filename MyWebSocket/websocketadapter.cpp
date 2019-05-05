#include "websocketadapter.h"

#include <cassert>

#include "MyWebSocket/mytypedefine.h"

//JSON数据序列化相关
#include "MyWebSocket/MySources/websocketmsgnames.h"
#include "MyWebSocket/MySources/JsonWrapper/jsonwrapper.h"
#include "MyWebSocket/MySources/websocketcommonmsg.h"

WebSocketAdapter::WebSocketAdapter(const std::string &groupId, const std::string &groupPassword, const std::string &rsaPublicKeyFile)
    : WebSocketCommunicator(groupId, groupPassword, rsaPublicKeyFile),
      recvJson(NULL), jsonMsgType(-1)
{
    recvJson = new JSONWrapper();
}

WebSocketAdapter::~WebSocketAdapter()
{
    if(recvJson){
        delete recvJson;
    }
}

/*****************************Send*****************************/
int WebSocketAdapter::SendMsg(const MsgStructure::Configuration &cfg)
{
    JSONWrapper json;
    WebSocketCommonMsg::SerializeConfMsg(cfg, json);

    return SendJson(json, &cfg);
}

int WebSocketAdapter::SendMsg(const MsgStructure::SingleAxis &sa)
{
    JSONWrapper json;
    json.addInt(WS_SINGLE_AXIS_Index, sa.axisIndex);
    json.addDouble(WS_SINGLE_AXIS_SPEED, sa.speed);

    return SendJson(json, &sa);
}

int WebSocketAdapter::SendMsg(const MsgStructure::InStatus &is)
{
    JSONWrapper json;
    json.addInt(WS_IN_STATUS, is.inStatus);

    return SendJson(json, &is);
}

int WebSocketAdapter::SendMsg(const MsgStructure::StringStatus &ss)
{
    JSONWrapper json;
    json.addInt(WS_STRING_STATUS, ss.strStatus);

    return SendJson(json, &ss);
}

int WebSocketAdapter::SendMsg(const MsgStructure::SlowDown &sd)
{
    JSONWrapper json;
    json.addInt(WS_SLOW_DOWN_CHANNEL, sd.channel);

    return SendJson(json, &sd);
}

int WebSocketAdapter::SendMsg(const MsgStructure::ActionMessage &am)
{
    JSONWrapper json;
    WebSocketCommonMsg::SerializeActMsg(am, json);

    return SendJson(json, &am);
}

int WebSocketAdapter::SendMsg(const MsgStructure::OutStatus &os)
{
    JSONWrapper json;
    json.addInt(WS_OUT_STATUS, os.outStatus);

    return SendJson(json, &os);
}

int WebSocketAdapter::SendMsg(const MsgStructure::OutControl &oc)
{
    JSONWrapper json;
    json.addInt(WS_OUT_CONTROL_STATUS, oc.outStatus);

    return SendJson(json, &oc);
}

int WebSocketAdapter::SendMsg(const MsgStructure::ManualControl &mc)
{
    JSONWrapper json;
    json.addInt(WS_MANUAL_CONTROL_METHOD, mc.method);
    json.addInt(WS_MANUAL_CONTROL_DIRECTION, mc.direction);
    json.addDouble(WS_MANUAL_CONTROL_SPEED, mc.speed);

    return SendJson(json, &mc);
}

int WebSocketAdapter::SendMsg(const MsgStructure::GoHomeResult &ghr)
{
    JSONWrapper json;
    json.addInt(WS_GO_HOME_RESULT, ghr.ghResult);

    return SendJson(json, &ghr);
}

int WebSocketAdapter::SendMsg(const MsgStructure::MatrixTheta &mt)
{
    JSONWrapper json;
    json.addDoubleArray(WS_MATRIX, mt.matrix, mt.lenMatrix);
    json.addDoubleArray(WS_THETA, mt.theta, mt.lenTheta);

    return SendJson(json, &mt);
}

int WebSocketAdapter::SendMsg(const MsgStructure::EditWeldSpeed &ews)
{
    JSONWrapper json;
    json.addDouble(WS_EDIT_WELD_SPEED, ews.diffSpeed);

    return SendJson(json, &ews);
}

int WebSocketAdapter::SendMsg(const MsgStructure::EditSpeed &es)
{
    JSONWrapper json;
    WebSocketCommonMsg::SerializeEditSpeedMsg(es, json);

    return SendJson(json, &es);
}

int WebSocketAdapter::SendMsg(const MsgStructure::FileContent &fc)
{
    JSONWrapper json;
    json.addString(WS_FILE_CONTENT, fc.data);

    return SendJson(json, &fc);
}

int WebSocketAdapter::SendMsg(const MsgStructure::MonitorCommand &mc)
{
    JSONWrapper json;
    json.addIntArray(WS_MONITOR_COMMAND_METHOD, mc.monitorMethod, mc.monitorArrayLen);
    json.addDoubleArray(WS_MONITOR_COMMAND_THETA, mc.monitorTheta, mc.monitorArrayLen);
    json.addInt(WS_MONITOR_COMMAND_PARAM, mc.params.param);

    return SendJson(json, &mc);
}

int WebSocketAdapter::SendMsg(const MsgStructure::PedalOpenValue &pov)
{
    JSONWrapper json;
    json.addDouble(WS_PEDAL_ACC_OPEN_VALUE, pov.accOpenValue);
    json.addDouble(WS_PEDAL_BRAKE_OPEN_VALUE, pov.brakeOpenValue);

    return SendJson(json, &pov);
}

int WebSocketAdapter::SendMsg(const MsgStructure::MedicalForceValue &mfv)
{
    JSONWrapper json;
    json.addDouble(WS_MEDICAL_FORCE_VALUE, mfv.forceValue);

    return SendJson(json, &mfv);
}

int WebSocketAdapter::SendMsg(const MsgStructure::MessageInform &mi)
{
    JSONWrapper json;
    json.addInt(WS_MESSAGE_INFORM_TYPE, mi.informType);
    json.addDouble(WS_MESSAGE_INFORM_CONTENT, mi.informContent);

    return SendJson(json, &mi);
}

int WebSocketAdapter::SendMsg(const int msgType)
{
    JSONWrapper json;
    json.addInt(WS_MSG_TYPE, msgType);

    return SendJsonStr(json.toString());
}

int WebSocketAdapter::SendJson(JSONWrapper &json, const MsgStructure::BaseStructure *msgStructure)
{
    //追加消息类型后发送
    json.addInt(WS_MSG_TYPE, msgStructure->GetMsgType());
    return SendJsonStr(json.toString());
}

int WebSocketAdapter::SendJsonStr(const std::string &jsonStr)
{
    WebSocketCommunicator::SendWebSocketMsg(jsonStr);//使用父类的protected函数
    return 0;
}

/*****************************Recv*****************************/
int WebSocketAdapter::CheckMsg()
{
    shared_string_ptr msg;
    if( WebSocketCommunicator::CheckRecvMsg(msg) ){//receive new message
        std::cout<<"ws check new"<<std::endl;

        //解析后的JSON保存在recvJson变量中
        //务必确保CheckMsg()之后 紧跟着调用相应的ReceiveMsg()读取消息内容
        recvJson->Parse( *msg );
        recvJson->loadInt(WS_MSG_TYPE, jsonMsgType);
        return jsonMsgType;
    }else{//no new message
        return -1;
    }
}

void WebSocketAdapter::ReceiveMsg(MsgStructure::Configuration &cfg)
{
    assert(jsonMsgType == cfg.GetMsgType());

    WebSocketCommonMsg::DeserializeConfMsg(*recvJson, cfg);
}

void WebSocketAdapter::ReceiveMsg(MsgStructure::SingleAxis &sa)
{
    assert(jsonMsgType == sa.GetMsgType());

    recvJson->loadInt(WS_SINGLE_AXIS_Index, sa.axisIndex);
    recvJson->loadDouble(WS_SINGLE_AXIS_SPEED, sa.speed);
}

void WebSocketAdapter::ReceiveMsg(MsgStructure::InStatus &is)
{
    assert(jsonMsgType == is.GetMsgType());

    recvJson->loadInt(WS_IN_STATUS, is.inStatus);
}

void WebSocketAdapter::ReceiveMsg(MsgStructure::StringStatus &ss)
{
    assert(jsonMsgType == ss.GetMsgType());

    recvJson->loadInt(WS_STRING_STATUS, ss.strStatus);
}

void WebSocketAdapter::ReceiveMsg(MsgStructure::SlowDown &sd)
{
    assert(jsonMsgType == sd.GetMsgType());

    recvJson->loadInt(WS_SLOW_DOWN_CHANNEL, sd.channel);
}

void WebSocketAdapter::ReceiveMsg(MsgStructure::ActionMessage &am)
{
    assert(jsonMsgType == am.GetMsgType());

    WebSocketCommonMsg::DeserializeActMsg(*recvJson, am);
}

void WebSocketAdapter::ReceiveMsg(MsgStructure::OutStatus &os)
{
    assert(jsonMsgType == os.GetMsgType());
    recvJson->loadInt(WS_OUT_STATUS, os.outStatus);
}

void WebSocketAdapter::ReceiveMsg(MsgStructure::OutControl &oc)
{
    assert(jsonMsgType == oc.GetMsgType());

    recvJson->loadInt(WS_OUT_CONTROL_STATUS, oc.outStatus);
}

void WebSocketAdapter::ReceiveMsg(MsgStructure::ManualControl &mc)
{
    assert(jsonMsgType == mc.GetMsgType());

    recvJson->loadInt(WS_MANUAL_CONTROL_METHOD, mc.method);
    recvJson->loadInt(WS_MANUAL_CONTROL_DIRECTION, mc.direction);
    recvJson->loadDouble(WS_MANUAL_CONTROL_SPEED, mc.speed);
}

void WebSocketAdapter::ReceiveMsg(MsgStructure::GoHomeResult &ghr)
{
    assert(jsonMsgType == ghr.GetMsgType());

    recvJson->loadInt(WS_GO_HOME_RESULT, ghr.ghResult);
}

void WebSocketAdapter::ReceiveMsg(MsgStructure::MatrixTheta &mt)
{
    assert(jsonMsgType == mt.GetMsgType());

    recvJson->loadDoubleArray(WS_MATRIX, mt.matrix, mt.lenMatrix);
    recvJson->loadDoubleArray(WS_THETA, mt.theta, mt.lenTheta);
}

void WebSocketAdapter::ReceiveMsg(MsgStructure::EditWeldSpeed &ews)
{
    assert(jsonMsgType == ews.GetMsgType());

    recvJson->loadDouble(WS_EDIT_WELD_SPEED, ews.diffSpeed);
}

void WebSocketAdapter::ReceiveMsg(MsgStructure::EditSpeed &es)
{
    assert(jsonMsgType == es.GetMsgType());

    WebSocketCommonMsg::DeserializeEditSpeedMsg(*recvJson, es);
}

void WebSocketAdapter::ReceiveMsg(MsgStructure::FileContent &fc)
{
    assert(jsonMsgType == fc.GetMsgType());

    recvJson->loadString(WS_FILE_CONTENT, fc.data);
}

void WebSocketAdapter::ReceiveMsg(MsgStructure::MonitorCommand &mc)
{
    assert(jsonMsgType == mc.GetMsgType());

    recvJson->loadIntArray(WS_MONITOR_COMMAND_METHOD, mc.monitorMethod, mc.monitorArrayLen);
    recvJson->loadDoubleArray(WS_MONITOR_COMMAND_THETA, mc.monitorTheta, mc.monitorArrayLen);
    recvJson->loadInt(WS_MONITOR_COMMAND_PARAM, mc.params.param);
}

void WebSocketAdapter::ReceiveMsg(MsgStructure::PedalOpenValue &pov)
{
    assert(jsonMsgType == pov.GetMsgType());

    recvJson->loadDouble(WS_PEDAL_ACC_OPEN_VALUE, pov.accOpenValue);
    recvJson->loadDouble(WS_PEDAL_BRAKE_OPEN_VALUE, pov.brakeOpenValue);
}

void WebSocketAdapter::ReceiveMsg(MsgStructure::MedicalForceValue &mfv)
{
    assert(jsonMsgType == mfv.GetMsgType());

    recvJson->loadDouble(WS_MEDICAL_FORCE_VALUE, mfv.forceValue);
}

void WebSocketAdapter::ReceiveMsg(MsgStructure::MessageInform &mi)
{
    assert(jsonMsgType == mi.GetMsgType());

    recvJson->loadInt(WS_MESSAGE_INFORM_TYPE, mi.informType);
    recvJson->loadDouble(WS_MESSAGE_INFORM_CONTENT, mi.informContent);
}
