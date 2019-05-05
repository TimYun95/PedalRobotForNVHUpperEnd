#include "communicator.h"

#include <assert.h>

#include "common/message.h"
#include "common/printf.h"

#include "communication/msgtypes.h"

#include "communication/libmodbus/EndianConverter.h"
#include "communication/libmodbus/msgreginfo.h"

Communicator::Communicator(const int id, const char* comNum)
{
    modbus = new ModbusQueue(id, comNum);
}

Communicator::~Communicator()
{
    delete modbus;
}

int Communicator::SendMsg(const MsgStructure::Configuration &cfg)
{
    std::tr1::shared_ptr<EndianConverter> ec = cfg.confMsg->Serialize();

    return SendMsgEndianConverter(cfg.GetMsgType(), ec.get());
}

int Communicator::SendMsg(const MsgStructure::SingleAxis &sa)
{
    EndianConverter ec;
    ec.addInt( sa.axisIndex );
    ec.addDouble( sa.speed );
    return SendMsgEndianConverter(sa.GetMsgType(), &ec);
}

int Communicator::SendMsg(const MsgStructure::InStatus &is)
{
    EndianConverter ec;
    ec.addInt( is.inStatus );
    return SendMsgEndianConverter(is.GetMsgType(), &ec);
}

int Communicator::SendMsg(const MsgStructure::StringStatus &ss)
{
    EndianConverter ec;
    ec.addInt( ss.strStatus );
    return SendMsgEndianConverter(ss.GetMsgType(), &ec);
}

int Communicator::SendMsg(const MsgStructure::SlowDown &sd)
{
    EndianConverter ec;
    ec.addInt( sd.channel );
    return SendMsgEndianConverter(sd.GetMsgType(), &ec);
}

int Communicator::SendMsg(const MsgStructure::ActionMessage &am)
{
    std::tr1::shared_ptr<EndianConverter> ec = am.myActMsg->Serialize();
    return SendMsgEndianConverter(am.GetMsgType(), ec.get());
}

int Communicator::SendMsg(const MsgStructure::OutStatus &os)
{
    EndianConverter ec;
    ec.addInt( os.outStatus );
    return SendMsgEndianConverter(os.GetMsgType(), &ec);
}

int Communicator::SendMsg(const MsgStructure::OutControl &oc)
{
    EndianConverter ec;
    ec.addInt( oc.outStatus );
    return SendMsgEndianConverter(oc.GetMsgType(), &ec);
}

int Communicator::SendMsg(const MsgStructure::ManualControl &mc)
{
    EndianConverter ec;
    ec.addInt( mc.method );
    ec.addInt( mc.direction );
    ec.addDouble( mc.speed );
    return SendMsgEndianConverter(mc.GetMsgType(), &ec);
}

int Communicator::SendMsg(const MsgStructure::GoHomeResult &ghr)
{
    EndianConverter ec;
    ec.addInt( ghr.ghResult );
    return SendMsgEndianConverter(ghr.GetMsgType(), &ec);
}

int Communicator::SendMsg(const MsgStructure::MatrixTheta &mt)
{
    EndianConverter ec;
    for(int i=0;i<mt.lenMatrix;i++) ec.addDouble(mt.matrix[i]);
    for(int i=0;i<mt.lenTheta;i++) ec.addDouble(mt.theta[i]);

    return SendMsgEndianConverter(mt.GetMsgType(), &ec);
}

int Communicator::SendMsg(const MsgStructure::EditWeldSpeed &ews)
{
    EndianConverter ec;
    ec.addDouble( ews.diffSpeed );
    return SendMsgEndianConverter( ews.GetMsgType(), &ec );
}

int Communicator::SendMsg(const MsgStructure::EditSpeed &es)
{
    std::tr1::shared_ptr<EndianConverter> ec = es.editSpeedMsg->Serialize();
    return SendMsgEndianConverter(es.GetMsgType(), ec.get());
}

int Communicator::SendMsg(const MsgStructure::FileContent &fc)
{
    EndianConverter ec;
    ec.addInt( fc.data.length() );
    SendMsgEndianConverter(I2C_FileLength, &ec);

    const int msgType = fc.GetMsgType();
    memcpy(tab_registers+msgInfo[msgType].addr, fc.data.c_str(), fc.data.length());
    return modbus->SendMsg2Queue(msgInfo[msgType].msgType, msgInfo[msgType].addr, fc.data.length()/2);//字符串长度
}

int Communicator::SendMsg(const MsgStructure::MonitorCommand &mc)
{
    EndianConverter ec;
    for(int i=0; i<mc.monitorArrayLen; ++i){
        ec.addInt(mc.monitorMethod[i]);
    }
    for(int i=0; i<mc.monitorArrayLen; ++i){
        ec.addDouble(mc.monitorTheta[i]);
    }
    ec.addInt(mc.params.param);
    return SendMsgEndianConverter(mc.GetMsgType(), &ec);
}

int Communicator::SendMsg(const MsgStructure::PedalOpenValue &pov)
{
    EndianConverter ec;
    ec.addDouble(pov.brakeOpenValue);
    ec.addDouble(pov.accOpenValue);
    return SendMsgEndianConverter(pov.GetMsgType(), &ec);
}

int Communicator::SendMsg(const MsgStructure::MedicalForceValue &mfv)
{
    EndianConverter ec;
    ec.addDouble(mfv.forceValue);
    return SendMsgEndianConverter(mfv.GetMsgType(), &ec);
}

int Communicator::SendMsg(const MsgStructure::MessageInform &mi)
{
    /* 由于Modbus寄存器的特殊性(数据写入寄存器 然后modbus发送)
     * 存在寄存器覆写的问题(前面的数据被后面的数据覆盖)
     * 故，这里需要将 无内容的消息 按照类型发送到不同的 寄存器位
     * 后续改为JSON字符串 然后串行发送 可以参考exynos4412的方式 */
    /* 按照类型发送到不同的 寄存器位 */

    //MsgStructure::MessageInform::MedicalAxis4Angle:
    //MsgStructure::MessageInform::EmergencyStop:
    EndianConverter ec;
    ec.addInt( mi.informType );
    ec.addDouble( mi.informContent );
    return SendMsgEndianConverter(mi.GetMsgType(), &ec);
}

int Communicator::SendMsg(const MsgStructure::UnifiedInform &ui)
{
    EndianConverter ec;
    for(size_t i=0; i<ui.intDataArray.size(); ++i){
        ec.addInt(ui.intDataArray[i]);
    }
    for(size_t i=0; i<ui.doubleDataArray.size(); ++i){
        ec.addDouble(ui.doubleDataArray[i]);
    }
    return SendMsgEndianConverter(ui.GetMsgType(), &ec);
}

int Communicator::SendMsg(const MsgStructure::VelocityCommand &vc)
{
    EndianConverter ec;
    for(int i=0; i<vc.targetSpeedLen; ++i){
        ec.addDouble(vc.targetSpeed[i]);
    }
    return SendMsgEndianConverter(vc.GetMsgType(), &ec);
}

int Communicator::SendMsg(const int msgType)
{
    return SendMsgEndianConverter(msgType, NULL);
}

int Communicator::SendMsgEndianConverter(int msgType, EndianConverter *ec)
{
    if(ec != NULL){
        ec->toModbus();//to Modbus endian
        memcpy(tab_registers+msgInfo[msgType].addr, ec->Data(), ec->length());

        /* 如果程序中出现该assert错误  请检查消息长度与寄存器的长度 */
        if(ec->length()/2 >  msgInfo[msgType].nregs){
            PRINTF(LOG_CRIT, "%s: msg(%d) length is too long(%d/%d)...\n",  __func__, msgType, ec->length()/2, msgInfo[msgType].nregs);
            assert(false);
        }
    }

    return modbus->SendMsg2Queue(msgInfo[msgType].msgType, msgInfo[msgType].addr, msgInfo[msgType].nregs);
}

Message Communicator::CheckMsg()
{
    return modbus->CheckMsg();
}

void Communicator::ReceiveMsg(MsgStructure::Configuration &cfg)
{
    unsigned short* addr = tab_registers+msgInfo[cfg.GetMsgType()].addr;
    cfg.confMsg->Deserialize(addr);
}

void Communicator::ReceiveMsg(MsgStructure::SingleAxis &sa)
{
    EndianConverter ec;
    ec.addInt().addDouble();
    ec.loadFromRegisters(tab_registers+msgInfo[sa.GetMsgType()].addr, ec.length()/2);
    ec.toLocal();

    sa.axisIndex = ec.parseInt();
    sa.speed = ec.parseDouble();
}

void Communicator::ReceiveMsg(MsgStructure::InStatus &is)
{
    EndianConverter ec;
    ec.addInt();
    ec.loadFromRegisters(tab_registers+msgInfo[is.GetMsgType()].addr, ec.length()/2);
    ec.toLocal();

    is.inStatus = ec.parseInt();
}

void Communicator::ReceiveMsg(MsgStructure::StringStatus &ss)
{
    EndianConverter ec;
    ec.addInt();
    ec.loadFromRegisters(tab_registers+msgInfo[ss.GetMsgType()].addr, ec.length()/2);
    ec.toLocal();

    ss.strStatus = ec.parseInt();
}

void Communicator::ReceiveMsg(MsgStructure::SlowDown &sd)
{
    EndianConverter ec;
    ec.addInt();
    ec.loadFromRegisters(tab_registers+msgInfo[sd.GetMsgType()].addr, ec.length()/2);
    ec.toLocal();

    sd.channel = ec.parseInt();
}

void Communicator::ReceiveMsg(MsgStructure::ActionMessage &am)
{
    unsigned short* addr = tab_registers+msgInfo[am.GetMsgType()].addr;
    am.myActMsg->Deserialize(addr);
}

void Communicator::ReceiveMsg(MsgStructure::OutStatus &os)
{
    EndianConverter ec;
    ec.addInt();
    ec.loadFromRegisters(tab_registers+msgInfo[os.GetMsgType()].addr, ec.length()/2);
    ec.toLocal();

    os.outStatus = ec.parseInt();
}

void Communicator::ReceiveMsg( MsgStructure::OutControl &oc)
{
    EndianConverter ec;
    ec.addInt();
    ec.loadFromRegisters(tab_registers+msgInfo[oc.GetMsgType()].addr, ec.length()/2);
    ec.toLocal();

    oc.outStatus = ec.parseInt();
}

void Communicator::ReceiveMsg(MsgStructure::ManualControl &mc)
{
    EndianConverter ec;
    ec.addInt().addInt().addDouble();
    ec.loadFromRegisters(tab_registers+msgInfo[mc.GetMsgType()].addr,ec.length()/2);
    ec.toLocal();

    mc.method = ec.parseInt();
    mc.direction = ec.parseInt();
    mc.speed = ec.parseDouble();
}

void Communicator::ReceiveMsg(MsgStructure::GoHomeResult &ghr)
{
    EndianConverter ec;
    ec.addInt();
    ec.loadFromRegisters(tab_registers+msgInfo[ghr.GetMsgType()].addr,ec.length()/2);
    ec.toLocal();

    ghr.ghResult = ec.parseInt();
}

void Communicator::ReceiveMsg(MsgStructure::MatrixTheta &mt)
{
    EndianConverter ec;
    for(int i=0; i<mt.lenMatrix; ++i) ec.addDouble();
    for(int i=0; i<mt.lenTheta; ++i) ec.addDouble();
    ec.loadFromRegisters(tab_registers+msgInfo[mt.GetMsgType()].addr,ec.length()/2);
    ec.toLocal();

    for(int i=0; i<mt.lenMatrix; ++i) mt.matrix[i] = ec.parseDouble();
    for(int i=0; i<mt.lenTheta; ++i) mt.theta[i] = ec.parseDouble();
}

void Communicator::ReceiveMsg(MsgStructure::EditWeldSpeed &ews)
{
    EndianConverter ec;
    ec.addDouble();
    ec.loadFromRegisters(tab_registers+msgInfo[ews.GetMsgType()].addr,ec.length()/2);
    ec.toLocal();

    ews.diffSpeed = ec.parseDouble();
}

void Communicator::ReceiveMsg(MsgStructure::EditSpeed &es)
{
    unsigned short* addr = tab_registers+msgInfo[es.GetMsgType()].addr;
    es.editSpeedMsg->Deserialize(addr);
}

void Communicator::ReceiveMsg(MsgStructure::FileContent &fc)
{
    //文件长度
    EndianConverter ec;
    ec.addInt();
    ec.loadFromRegisters(tab_registers+msgInfo[I2C_FileLength].addr, ec.length()/2);
    ec.toLocal();
    const int fileLen = ec.parseInt();
    PRINTF(LOG_DEBUG, "%s: file len=%d\n", __func__, fileLen);

    //文件内容
    char* pc = (char*)(tab_registers+msgInfo[fc.GetMsgType()].addr);
    char buff[fileLen+1];
    memcpy(buff, pc, fileLen);
    buff[fileLen] = '\0';

    PRINTF(LOG_DEBUG, "%s: contents=%s\n", __func__, buff);

    fc.data = std::string(buff, fileLen);
}

void Communicator::ReceiveMsg(MsgStructure::MonitorCommand &mc)
{
    EndianConverter ec;
    for(int i=0; i<mc.monitorArrayLen; ++i){
        ec.addInt();
    }
    for(int i=0; i<mc.monitorArrayLen; ++i){
        ec.addDouble();
    }
    ec.addInt();
    ec.loadFromRegisters(tab_registers+msgInfo[mc.GetMsgType()].addr, ec.length()/2);
    ec.toLocal();

    for(int i=0; i<mc.monitorArrayLen; ++i){
        mc.monitorMethod[i] = ec.parseInt();
    }
    for(int i=0; i<mc.monitorArrayLen; ++i){
        mc.monitorTheta[i] = ec.parseDouble();
    }
    mc.params.param = ec.parseInt();
}

void Communicator::ReceiveMsg(MsgStructure::PedalOpenValue &pov)
{
    EndianConverter ec;
    ec.addDouble().addDouble();
    ec.loadFromRegisters(tab_registers+msgInfo[pov.GetMsgType()].addr, ec.length()/2);
    ec.toLocal();

    pov.brakeOpenValue = ec.parseDouble();
    pov.accOpenValue = ec.parseDouble();
}

void Communicator::ReceiveMsg(MsgStructure::MedicalForceValue &mfv)
{
    EndianConverter ec;
    ec.addDouble();
    ec.loadFromRegisters(tab_registers+msgInfo[mfv.GetMsgType()].addr, ec.length() / 2);
    ec.toLocal();

    mfv.forceValue = ec.parseDouble();
}

void Communicator::ReceiveMsg(MsgStructure::MessageInform &mi)
{
    //MsgStructure::MessageInform::MedicalAxis4Angle
    //MsgStructure::MessageInform::EmergencyStop
    EndianConverter ec;
    ec.addInt().addDouble();
    ec.loadFromRegisters(tab_registers+msgInfo[mi.GetMsgType()].addr, ec.length()/2);
    ec.toLocal();

    mi.informType = ec.parseInt();
    mi.informContent = ec.parseDouble();
}

void Communicator::ReceiveMsg(MsgStructure::UnifiedInform &ui)
{
    EndianConverter ec;
    for(size_t i=0; i<ui.intDataArray.size(); ++i){
        ec.addInt();
    }
    for(size_t i=0; i<ui.doubleDataArray.size(); ++i){
        ec.addDouble();
    }
    ec.loadFromRegisters(tab_registers+msgInfo[ui.GetMsgType()].addr, ec.length()/2);
    ec.toLocal();

    for(size_t i=0; i<ui.intDataArray.size(); ++i){
        ui.intDataArray[i] = ec.parseInt();
    }
    for(size_t i=0; i<ui.doubleDataArray.size(); ++i){
        ui.doubleDataArray[i] = ec.parseDouble();
    }
}

void Communicator::ReceiveMsg(MsgStructure::VelocityCommand &vc)
{
    EndianConverter ec;
    for(int i=0; i<vc.targetSpeedLen; ++i){
        ec.addDouble();
    }
    ec.loadFromRegisters(tab_registers+msgInfo[vc.GetMsgType()].addr, ec.length()/2);
    ec.toLocal();

    for(int i=0; i<vc.targetSpeedLen; ++i){
        vc.targetSpeed[i] = ec.parseDouble();
    }
}
