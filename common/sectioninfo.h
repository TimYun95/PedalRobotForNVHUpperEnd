#ifndef SECTIONINFO_H
#define SECTIONINFO_H

#include <string>
#include <iomanip>
#include <sstream>
#include <vector>
#include <istream>
#include <tr1/memory>

#include "globalvariables.h"

class SectionInfo;
typedef std::tr1::shared_ptr<SectionInfo> SectionInfoPtr;
typedef std::vector<SectionInfoPtr> SectionInfoPtrList;

class SectionInfo {
public:
    SectionInfo(char _m): mode(_m) {}
    virtual ~SectionInfo() {}

    char GetMode() { return mode; }
    void SetMode(char c) { mode = c; }

    static SectionInfoPtrList ParseSectionInfoListFromFile(const std::string &filePath);
    static SectionInfoPtrList ParseSectionInfoListFromStringStream(std::stringstream &ss);
    static bool StringifySectionInfoListIntoFile(SectionInfoPtrList &sectionInfoList, const std::string &filePath);
    static bool StringifySectionInfoListIntoStringStream(SectionInfoPtrList &sectionInfoList, std::stringstream &ss);

    /* 把数据结构序列化成示教文件的格式 */
    virtual std::string Serialize()=0;
    /* 把示教文件反序列化成数据结构 */
    virtual void DeSerialize(std::stringstream &)=0;

    /* 注意核查该三个函数 现在的架构不好 */
    /* 焊接速度设置 */
    virtual void SetWeldSpeed(double) {}
    virtual double GetWeldSpeed(double) {return 0;}
    /* 过渡速度设置 */
    virtual double GetTransSpeed() {return 0;}

protected://Marshall(按次序排列成形) 用于序列化Serialize()
    void MarshallInitStringStream(std::stringstream &ss){
        ss.setf( std::ios::fixed, std:: ios::floatfield );
        ss << mode;//直接输出
        MarshallNewLineToSS(ss);
    }

    template<typename T>
    void MarshallToSS(std::stringstream &ss, const T &value){
        ss << std::right << std::setw(15) << value;
    }

    void MarshallNewLineToSS(std::stringstream &ss){
        ss << "\n";
    }

protected://Unmarshall 用于反序列化DeSerialize()
    template<typename T>
    void UnmarshallFromSS(std::stringstream &ss, T &value){
        ss >> value;
    }

protected:
    char mode;
};

class RSectionInfo : public SectionInfo{
public:
    RSectionInfo(): SectionInfo('R') {
        times=0;
        regulate=0.0;
        transPeriod=0;
        workPeriod=0;
        calibratingPoints=0;
    }
    virtual std::string Serialize(){
        std::stringstream ss;
        MarshallInitStringStream(ss);

        MarshallToSS(ss, times);
        MarshallToSS(ss, regulate);
        MarshallToSS(ss, transPeriod);
        MarshallToSS(ss, workPeriod);
        MarshallToSS(ss, calibratingPoints);
        MarshallNewLineToSS(ss);

        return ss.str();
    }
    virtual void DeSerialize(std::stringstream &ss){
        UnmarshallFromSS(ss, times);
        UnmarshallFromSS(ss, regulate);
        UnmarshallFromSS(ss, transPeriod);
        UnmarshallFromSS(ss, workPeriod);
        UnmarshallFromSS(ss, calibratingPoints);
    }

    int times;//示教文件重复次数
    double regulate;//Z向矫正量
    int transPeriod;//过渡段周期
    int workPeriod;//工作段周期
    int calibratingPoints;//矫正点数(过度段移动增加这些点数使得运动更加准确)
};

class QSectionInfo : public SectionInfo{
public:
    QSectionInfo(): SectionInfo('Q') {}
    double transSpeed;
    /*该段的位姿 一维*/
    double mat[RobotParams::dof];
    double theta[RobotParams::axisNum];

    virtual std::string Serialize(){
        std::stringstream ss;
        MarshallInitStringStream(ss);

        MarshallToSS(ss, transSpeed);
        MarshallNewLineToSS(ss);

        for(int i=0;i<RobotParams::axisNum;i++){
            MarshallToSS(ss, theta[i]);
        }
        MarshallNewLineToSS(ss);

        return ss.str();
    }
    virtual void DeSerialize(std::stringstream &ss){
        UnmarshallFromSS(ss, transSpeed);

        for(int i=0;i<RobotParams::axisNum;i++){
            UnmarshallFromSS(ss, theta[i]);
        }
    }
    double GetTransSpeed(){
        return transSpeed;
    }
};

class TSectionInfo : public SectionInfo{
public:
    TSectionInfo(): SectionInfo('T'){}
    double transSpeed;
    /*该段的位姿 一维*/
    double mat[RobotParams::dof];
    double theta[RobotParams::axisNum];

    virtual std::string Serialize(){
        std::stringstream ss;
        MarshallInitStringStream(ss);

        MarshallToSS(ss, transSpeed);
        MarshallNewLineToSS(ss);

        for(int i=0;i<RobotParams::axisNum;i++){
            MarshallToSS(ss, theta[i]);
        }
        MarshallNewLineToSS(ss);

        return ss.str();
    }
    virtual void DeSerialize(std::stringstream &ss){
        UnmarshallFromSS(ss, transSpeed);

        for(int i=0;i<RobotParams::axisNum;i++){
            UnmarshallFromSS(ss, theta[i]);
        }
    }
    double GetTransSpeed(){
        return transSpeed;
    }
};

class LSectionInfo : public SectionInfo{
public:
    LSectionInfo(): SectionInfo('L'){}
    double weldSpeed,transSpeed;
    int vibType;
    double vibrationFreqFB,vibrationAmplFB,vibrationFreqLR,vibrationAmplLR;
    /*该段的位姿 二维*/
    double mat[2][RobotParams::dof];
    double theta[2][RobotParams::axisNum];

    virtual std::string Serialize(){
        std::stringstream ss;
        MarshallInitStringStream(ss);

        MarshallToSS(ss, weldSpeed);
        MarshallToSS(ss, transSpeed);
        MarshallNewLineToSS(ss);

        MarshallToSS(ss, 0);
        MarshallToSS(ss, 10);
        MarshallToSS(ss, 15);
        MarshallNewLineToSS(ss);

        MarshallToSS(ss, vibType);
        MarshallToSS(ss, vibrationFreqFB);
        MarshallToSS(ss, vibrationAmplFB);
        MarshallToSS(ss, vibrationFreqLR);
        MarshallToSS(ss, vibrationAmplLR);
        MarshallNewLineToSS(ss);

        for(int i=0;i<2;i++){
            for(int j=0;j<RobotParams::axisNum;j++){
                MarshallToSS(ss, theta[i][j]);
            }
            MarshallNewLineToSS(ss);
        }

        return ss.str();
    }
    virtual void DeSerialize(std::stringstream &ss){
        int useless;
        UnmarshallFromSS(ss, weldSpeed);
        UnmarshallFromSS(ss, transSpeed);

        UnmarshallFromSS(ss, useless);
        UnmarshallFromSS(ss, useless);
        UnmarshallFromSS(ss, useless);

        UnmarshallFromSS(ss, vibType);
        UnmarshallFromSS(ss, vibrationFreqFB);
        UnmarshallFromSS(ss, vibrationAmplFB);
        UnmarshallFromSS(ss, vibrationFreqLR);
        UnmarshallFromSS(ss, vibrationAmplLR);

        for(int i=0;i<2;i++){
            for(int j=0;j<RobotParams::axisNum;j++){
                UnmarshallFromSS(ss, theta[i][j]);
            }
        }
    }
    void SetWeldSpeed(double newSpeed){
        weldSpeed=newSpeed;
    }

    double GetTransSpeed(){
        return transSpeed;
    }
};

class DSectionInfo : public SectionInfo{
public:
    DSectionInfo(): SectionInfo('D'){}
    int dotTime;//ms
    double transSpeed;
    double mat[RobotParams::dof];
    double theta[RobotParams::axisNum];

    virtual std::string Serialize(){
        std::stringstream ss;
        MarshallInitStringStream(ss);

        MarshallToSS(ss, dotTime);
        MarshallToSS(ss, transSpeed);
        MarshallNewLineToSS(ss);

        MarshallToSS(ss, 0);
        MarshallToSS(ss, 10);
        MarshallToSS(ss, 15);
        MarshallNewLineToSS(ss);

        for(int j=0;j<RobotParams::axisNum;j++){
            MarshallToSS(ss, theta[j]);
        }
        MarshallNewLineToSS(ss);

        return ss.str();
    }
    virtual void DeSerialize(std::stringstream &ss){
        int useless;
        UnmarshallFromSS(ss, dotTime);
        UnmarshallFromSS(ss, transSpeed);

        UnmarshallFromSS(ss, useless);
        UnmarshallFromSS(ss, useless);
        UnmarshallFromSS(ss, useless);

        for(int j=0;j<RobotParams::axisNum;j++){
            UnmarshallFromSS(ss, theta[j]);
        }
    }
    double GetDotTime(){
        return dotTime;
    }
};

class SSectionInfo : public SectionInfo{
public:
    SSectionInfo(): SectionInfo('S'){}
    double weldSpeed,transSpeed;
    int vibType;
    double vibrationFreqFB,vibrationAmplFB,vibrationFreqLR,vibrationAmplLR;
    /* 多段直线 vector存放每个点的位姿 */
    struct Position{
        double mat[RobotParams::dof];
        double theta[RobotParams::axisNum];
    };
    std::vector<Position> point;

    virtual std::string Serialize(){
        std::stringstream ss;
        MarshallInitStringStream(ss);

        MarshallToSS(ss, point.size());
        MarshallToSS(ss, weldSpeed);
        MarshallToSS(ss, transSpeed);
        MarshallNewLineToSS(ss);

        MarshallToSS(ss, 0);
        MarshallToSS(ss, 10);
        MarshallToSS(ss, 15);
        MarshallNewLineToSS(ss);

        MarshallToSS(ss, vibType);
        MarshallToSS(ss, vibrationFreqFB);
        MarshallToSS(ss, vibrationAmplFB);
        MarshallToSS(ss, vibrationFreqLR);
        MarshallToSS(ss, vibrationAmplLR);
        MarshallNewLineToSS(ss);

        for(size_t i=0;i<point.size();i++){
            for(int j=0;j<RobotParams::axisNum;j++){
                MarshallToSS(ss, point[i].theta[j]);
            }
            MarshallNewLineToSS(ss);
        }

        MarshallNewLineToSS(ss);

        return ss.str();
    }
    virtual void DeSerialize(std::stringstream &ss){
        int useless, pointNum;
        UnmarshallFromSS(ss, pointNum);
        UnmarshallFromSS(ss, weldSpeed);
        UnmarshallFromSS(ss, transSpeed);

        UnmarshallFromSS(ss, useless);
        UnmarshallFromSS(ss, useless);
        UnmarshallFromSS(ss, useless);

        UnmarshallFromSS(ss, vibType);
        UnmarshallFromSS(ss, vibrationFreqFB);
        UnmarshallFromSS(ss, vibrationAmplFB);
        UnmarshallFromSS(ss, vibrationFreqLR);
        UnmarshallFromSS(ss, vibrationAmplLR);

        point.assign(pointNum,Position());
        for(int i=0;i<pointNum;i++){
            for(int j=0;j<RobotParams::axisNum;j++){
                UnmarshallFromSS(ss, point[i].theta[j]);
            }
        }
    }
    void SetWeldSpeed(double newSpeed){
        weldSpeed=newSpeed;
    }
};

class CSectionInfo : public SectionInfo{
public:
    CSectionInfo(char mod): SectionInfo(mod){}
    double weldSpeed,transSpeed;
    int vibType;
    double vibrationFreqFB,vibrationAmplFB,vibrationFreqLR,vibrationAmplLR;
    double mat[3][RobotParams::dof];//每一行=圆弧上记录的一点
    double theta[3][RobotParams::axisNum];

    virtual std::string Serialize(){
        std::stringstream ss;
        MarshallInitStringStream(ss);

        MarshallToSS(ss, weldSpeed);
        MarshallToSS(ss, transSpeed);
        MarshallNewLineToSS(ss);

        MarshallToSS(ss, 0);
        MarshallToSS(ss, 10);
        MarshallToSS(ss, 15);
        MarshallNewLineToSS(ss);

        MarshallToSS(ss, vibType);
        MarshallToSS(ss, vibrationFreqFB);
        MarshallToSS(ss, vibrationAmplFB);
        MarshallToSS(ss, vibrationFreqLR);
        MarshallToSS(ss, vibrationAmplLR);
        MarshallNewLineToSS(ss);

        for(int i=0;i<3;i++){
            for(int j=0;j<RobotParams::axisNum;j++){
                MarshallToSS(ss, theta[i][j]);
            }
            MarshallNewLineToSS(ss);
        }

        return ss.str();
    }
    virtual void DeSerialize(std::stringstream &ss){
        int useless;
        UnmarshallFromSS(ss, weldSpeed);
        UnmarshallFromSS(ss, transSpeed);

        UnmarshallFromSS(ss, useless);
        UnmarshallFromSS(ss, useless);
        UnmarshallFromSS(ss, useless);

        UnmarshallFromSS(ss, vibType);
        UnmarshallFromSS(ss, vibrationFreqFB);
        UnmarshallFromSS(ss, vibrationAmplFB);
        UnmarshallFromSS(ss, vibrationFreqLR);
        UnmarshallFromSS(ss, vibrationAmplLR);

        for(int i=0;i<3;i++){
            for(int j=0;j<RobotParams::axisNum;j++){
                UnmarshallFromSS(ss, theta[i][j]);
            }
        }
    }
    void SetWeldSpeed(double newSpeed){
        weldSpeed=newSpeed;
    }
};

class JSectionInfo : public SectionInfo{
public:
    JSectionInfo(): SectionInfo('J') {}

    virtual std::string Serialize(){
        std::stringstream ss;
        MarshallInitStringStream(ss);

        return ss.str();
    }
    virtual void DeSerialize(std::stringstream &){}
};

class MSectionInfo : public SectionInfo{//监听模式Monitor
public:
    MSectionInfo(): SectionInfo('M') {}

    virtual std::string Serialize(){
        std::stringstream ss;
        MarshallInitStringStream(ss);

        MarshallToSS(ss, duration_s);
        MarshallToSS(ss, period_us);
        MarshallNewLineToSS(ss);

        MarshallToSS(ss, workMode);
        MarshallToSS(ss, pulseTime);
        MarshallToSS(ss, internalCycle);
        MarshallNewLineToSS(ss);

        MarshallToSS(ss, totalPoints);
        MarshallToSS(ss, dividedPoints);
        MarshallNewLineToSS(ss);

        MarshallToSS(ss, enableSendPosition);
        MarshallToSS(ss, enableCheckConnection);
        MarshallNewLineToSS(ss);

        return ss.str();
    }
    virtual void DeSerialize(std::stringstream &ss){
        UnmarshallFromSS(ss, duration_s);
        UnmarshallFromSS(ss, period_us);

        UnmarshallFromSS(ss, workMode);
        UnmarshallFromSS(ss, pulseTime);
        UnmarshallFromSS(ss, internalCycle);

        UnmarshallFromSS(ss, totalPoints);
        UnmarshallFromSS(ss, dividedPoints);

        UnmarshallFromSS(ss, enableSendPosition);
        UnmarshallFromSS(ss, enableCheckConnection);
    }

    virtual double GetSpeed() {
        return 0.0;
    }

    /* |________|___|
     *     p1     p2
     * p1=dividedPoints 插补划分运动 p2=直接向最后的目标点运动
     * p1+p2=totalPoints */
    double duration_s;//监听模式的持续时间 0=始终监听
    double period_us;//监听模式的工作周期
    int workMode;//停等/连续的工作模式
    int pulseTime;//设定脉冲时的时间
    int internalCycle;//每个插补点的内部循环周期次数
    int totalPoints;//每个周期的插补点数
    int dividedPoints;//每个周期的多少插补点数后，直接定为目标点
    int enableSendPosition;//是否需要发送位置通讯信息
    int enableCheckConnection;//是否需要检查连接情况

    enum TagMode{
        StopWaitMode,
        KeepWorkingMode
    };
    enum TagEnableSend{
        EnableSend,
        DisableSend
    };
    enum TagEnableCheck{
        EnableCheck,
        DisableCheck
    };
};

class VSectionInfo : public SectionInfo{//速度模式Velocity
public:
    VSectionInfo(): SectionInfo('V') {}

    virtual std::string Serialize(){
        std::stringstream ss;
        MarshallInitStringStream(ss);

        MarshallToSS(ss, durationSeconds);
        MarshallToSS(ss, workPeriodUs);
        MarshallToSS(ss, pulseTimeNs);
        MarshallNewLineToSS(ss);

        MarshallToSS(ss, sendThetaMatrixIntervalCount);
        MarshallToSS(ss, connectionLostCountThreshold);
        MarshallNewLineToSS(ss);

        return ss.str();
    }
    virtual void DeSerialize(std::stringstream &ss){
        UnmarshallFromSS(ss, durationSeconds);
        UnmarshallFromSS(ss, workPeriodUs);
        UnmarshallFromSS(ss, pulseTimeNs);

        UnmarshallFromSS(ss, sendThetaMatrixIntervalCount);
        UnmarshallFromSS(ss, connectionLostCountThreshold);
    }

    virtual double GetSpeed() {
        return 0.0;
    }

    double durationSeconds;//持续时间 0=始终
    int workPeriodUs;//工作周期
    unsigned int pulseTimeNs;//设定脉冲时的时间
    int sendThetaMatrixIntervalCount;//多久上传一次位置 (多少个工作周期)
    int connectionLostCountThreshold;//连接丢失计数阈值 0=不检查丢失 (多少个工作周期)
};

#endif // SECTIONINFO_H
