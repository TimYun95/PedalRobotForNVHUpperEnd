#include "teach.h"

#include <math.h>

#include <QMessageBox>
#include <QFileDialog>

#include "common/printf.h"
#include "common/message.h"
#include "mymathlib/mymathlib.h"
#include "mywidget/mymessagebox.h"

static constexpr int MaxListSize = 10;
static constexpr double ContinuesSegmentsThreshold = 10.0;

Teach::Teach()
{
    conf = Configuration::Instance();

    teachFileName = "";
    teachStatusCurr = IDLE;
    teachStatusLast = IDLE;

    lastSectionMode = 0;
    insertTeachIndex = 0;

    for(int i=0; i<RobotParams::dof; ++i){
        matrixCurrStartPoint[i] = 0.0;
        matrixLastEndPoint[i] = 0.0;
    }

    for(int i=0; i<RobotParams::dof; ++i){
        matrixArc1[i] = 0.0;
        matrixArc2[i] = 0.0;
        matrixArc3[i] = 0.0;
    }
}

Teach::~Teach()
{
    //
}

bool Teach::StartTeach()
{
    QString fileName = QFileDialog::getSaveFileName(Q_NULLPTR, "Choose teach file", Configuration::Instance()->GetFilePath(Configuration::RootFolderPath).c_str());
    if(fileName.isEmpty()){
        PRINTF(LOG_ERR, "%s: cannot getSaveFileName\n", __func__);
        return false;
    }
    teachFileName = fileName.toStdString();

    sectionList.clear();
    sectionList.push_back( SectionInfoPtr(new RSectionInfo()) );

    teachStatusCurr = NEW_TEACH;
    return true;
}

Teach::TeachStatus Teach::GetTeachStatus()
{
    return teachStatusCurr;
}

SectionInfo *Teach::GetSectionInfo(int sectionIndex)
{
    if(sectionIndex < 0 || sectionIndex >= (int)sectionList.size()){
        return NULL;
    }

    return sectionList.at(sectionIndex).get();
}

void Teach::RecordTransSection()
{
    SectionInfoPtr currentSection(new TSectionInfo());
    TSectionInfo* tsi=dynamic_cast<TSectionInfo*>(currentSection.get());
    tsi->transSpeed=0;
    RecordPosition(tsi->mat,tsi->theta);
    MakeOneSection(currentSection);
    lastSectionMode='T';
    UpdateMatrixLastEndPoint(tsi->mat);
}

void Teach::RecordSuspendSection()
{
    SectionInfoPtr currentSection(new QSectionInfo());
    QSectionInfo* qsi=dynamic_cast<QSectionInfo*>(currentSection.get());
    RecordPosition(qsi->mat,qsi->theta);
    MakeOneSection(currentSection);
    lastSectionMode='Q';
    UpdateMatrixLastEndPoint(qsi->mat);
}

void Teach::RecordJointSection()
{
    SectionInfoPtr currentSection(new JSectionInfo());
    //JSectionInfo* jsi=dynamic_cast<JSectionInfo*>(currentSection.get());
    MakeOneSection(currentSection);
    lastSectionMode='J';
}

int Teach::RecordLineSectionStartPoint(double weldSpeed, int vibType,double vibrationFreqFB,double vibrationAmplFB,
                 double vibrationFreqLR,double vibrationAmplLR)
{
    CopyCurrentMatrixTo(matrixCurrStartPoint);
    if( !IsSectionsNearby() ){
        ShowAutoContinuesInformMessageBox();
    }

    currentSection.reset(new LSectionInfo());
    LSectionInfo* lsi=dynamic_cast<LSectionInfo*>(currentSection.get());
    lsi->weldSpeed=weldSpeed;
    lsi->transSpeed=0;
    lsi->vibType=vibType;
    lsi->vibrationAmplFB=(vibType ? vibrationAmplFB :0);
    lsi->vibrationAmplLR=(vibType ? vibrationAmplLR :0);
    lsi->vibrationFreqFB=(vibType ? vibrationFreqFB :0);
    lsi->vibrationFreqLR=(vibType ? vibrationFreqLR :0);
    RecordPosition(&(lsi->mat[0][0]), &(lsi->theta[0][0]));

    return 0;
}

int Teach::RecordLineSectionEndPoint()
{
    LSectionInfo *lsi=dynamic_cast<LSectionInfo*>(currentSection.get());
    if(lsi==NULL){
        return -1;
    }
    RecordPosition(&(lsi->mat[1][0]),&(lsi->theta[1][0]));

    double distance = MyMathLib::CalcEuclideanDistance(lsi->mat[1], lsi->mat[0], 3);
    if(distance < MyMathLib::smallValue)
    {
        MyMessageBox::ShowInformMessageBox("直线的2个示教点重合, 无法构成直线");
        currentSection.reset();
    }
    else
    {
        MakeOneSection(currentSection);
        lastSectionMode='L';
        UpdateMatrixLastEndPoint(lsi->mat[1]);
    }

    return 0;
}

int Teach::RecordCircleSectionPoint1(double weldSpeed, int vibType,double vibrationFreqFB,double vibrationAmplFB,
                double vibrationFreqLR,double vibrationAmplLR)
{
    CopyCurrentMatrixTo(matrixCurrStartPoint);

    if( !IsSectionsNearby() ){
        ShowAutoContinuesInformMessageBox();
    }

    char arcMethod = QuestionCircleSectionType();
    if(arcMethod == 0){
        return -1;
    }

    currentSection.reset(new CSectionInfo(arcMethod));
    CSectionInfo* csi=dynamic_cast<CSectionInfo*>(currentSection.get());
    csi->weldSpeed=weldSpeed;
    csi->transSpeed=0;
    csi->vibType=vibType;
    csi->vibrationAmplFB=(vibType ? vibrationAmplFB :0);
    csi->vibrationAmplLR=(vibType ? vibrationAmplLR :0);
    csi->vibrationFreqFB=(vibType ? vibrationFreqFB :0);
    csi->vibrationFreqLR=(vibType ? vibrationFreqLR :0);
    RecordPosition(&(csi->mat[0][0]),&(csi->theta[0][0]));
    CopyCurrentMatrixTo(matrixArc1);

    return 0;
}

int Teach::RecordCircleSectionPoint2()
{
    CSectionInfo *csi=dynamic_cast<CSectionInfo*>(currentSection.get());
    if(csi==NULL){
        return -1;
    }
    RecordPosition(&(csi->mat[1][0]),&(csi->theta[1][0]));
    CopyCurrentMatrixTo(matrixArc2);

    return 0;
}

int Teach::RecordCircleSectionPoint3()
{
    CSectionInfo *csi=dynamic_cast<CSectionInfo*>(currentSection.get());
    if(csi==NULL){
        return -1;
    }
    RecordPosition(&(csi->mat[2][0]),&(csi->theta[2][0]));
    CopyCurrentMatrixTo(matrixArc3);


    double d1 = MyMathLib::CalcEuclideanDistance(matrixArc2, matrixArc1, 3);
    double d2 = MyMathLib::CalcEuclideanDistance(matrixArc3, matrixArc1, 3);
    double d3 = MyMathLib::CalcEuclideanDistance(matrixArc3, matrixArc2, 3);
    if(d1<MyMathLib::smallValue || d2<MyMathLib::smallValue || d3<MyMathLib::smallValue)
    {
        MyMessageBox::ShowInformMessageBox("所指定的3个示教点有重合, 无法构成圆弧!");
    }
    else if((d1+d2-d3)<MyMathLib::smallValue || (d1+d3-d2)<MyMathLib::smallValue || (d2+d3-d1)<MyMathLib::smallValue )
    {
        MyMessageBox::ShowInformMessageBox("所指定的3个示教点在直线上,无法构成圆弧!\n若想使用直线焊接,请点击直线选项卡!");
    }
    else if(csi->GetMode() == 'O')
    {
        double deltaArc21,deltaArc32,deltaArc13;
        deltaArc21=matrixArc2[3]-matrixArc1[3];
        deltaArc32=matrixArc3[3]-matrixArc2[3];
        deltaArc13=matrixArc1[3]-matrixArc3[3];
        if(deltaArc21*deltaArc32>0 && fabs(deltaArc13)>2.5)
        {
            if(deltaArc21>0 && (MyMathLib::RadToAngle(matrixArc1[3])+360) > conf->limitPos[2])
            {
                MyMessageBox::ShowInformMessageBox("该段整圆示教会超出3#正极限,请重新示教!\n建议:改变第一示教点的位置!");
            }
            else if(deltaArc21<0 && (MyMathLib::RadToAngle(matrixArc1[3])-360) < conf->limitNeg[2])
            {
                MyMessageBox::ShowInformMessageBox("该段整圆示教会超出3#负极限,请重新示教!\n建议:改变第一示教点的位置!");
            }
            else
            {
                MakeOneSection(currentSection);
                lastSectionMode='O';
                UpdateMatrixLastEndPoint(matrixArc1);
            }
        }
        else
        {
            MakeOneSection(currentSection);
            lastSectionMode='O';
            UpdateMatrixLastEndPoint(matrixArc1);
        }
    }
    else
    {
        MakeOneSection(currentSection);
        lastSectionMode='C';
        UpdateMatrixLastEndPoint(matrixArc3);
    }
    currentSection.reset();

    return 0;
}

int Teach::RecordSegmentSectionFirst(double weldSpeed, int vibType,double vibrationFreqFB,double vibrationAmplFB,
                     double vibrationFreqLR,double vibrationAmplLR)
{
    CopyCurrentMatrixTo(matrixCurrStartPoint);

    if( !IsSectionsNearby() ){
        ShowAutoContinuesInformMessageBox();
    }

    currentSection.reset(new SSectionInfo());
    SSectionInfo* ssi=dynamic_cast<SSectionInfo*>(currentSection.get());
    ssi->weldSpeed=weldSpeed;
    ssi->transSpeed=0;
    ssi->vibType=vibType;
    ssi->vibrationAmplFB=(vibType ? vibrationAmplFB :0);
    ssi->vibrationAmplLR=(vibType ? vibrationAmplLR :0);
    ssi->vibrationFreqFB=(vibType ? vibrationFreqFB :0);
    ssi->vibrationFreqLR=(vibType ? vibrationFreqLR :0);
    SSectionInfo::Position pt;
    RecordPosition(pt.mat,pt.theta);
    ssi->point.push_back(pt);

    return 0;
}

int Teach::RecordSegmentSectionNext()
{
    SSectionInfo *ssi=dynamic_cast<SSectionInfo*>(currentSection.get());
    if(ssi==NULL){
        return -1;
    }

    SSectionInfo::Position pt;
    RecordPosition(pt.mat,pt.theta);
    ssi->point.push_back(pt);

    return ssi->point.size();
}

int Teach::RecordSegmentSectionFinish()
{
    SSectionInfo *ssi=dynamic_cast<SSectionInfo*>(currentSection.get());
    if(ssi==NULL){
        return -1;
    }

    double robotMatrix[RobotParams::dof];
    for(int i=0; i<RobotParams::dof; ++i){
        robotMatrix[i] = RobotThetaMatrix::Instance()->GetMatrix(i);
    }

    MakeOneSection(currentSection);
    lastSectionMode='S';
    UpdateMatrixLastEndPoint(robotMatrix);

    return 0;
}

int Teach::UndoRecordSegmentSection()
{
    SSectionInfo *ssi=dynamic_cast<SSectionInfo*>(currentSection.get());
    if(ssi==NULL){
        return -1;
    }

    if(!ssi->point.empty())
        ssi->point.pop_back();

    return ssi->point.size();
}

int Teach::GetSegmentSectionPointsNum()
{
    if(!currentSection){
        return 0;
    }

    SSectionInfo *ssi=dynamic_cast<SSectionInfo*>(currentSection.get());
    if(ssi==NULL){
        return 0;
    }

    return ssi->point.size();
}

void Teach::RecordDotSection(int dotTime)
{
    SectionInfoPtr currentSection(new DSectionInfo());
    DSectionInfo* dsi=dynamic_cast<DSectionInfo*>(currentSection.get());
    dsi->dotTime=dotTime;
    dsi->transSpeed=0;
    RecordPosition(dsi->mat,dsi->theta);
    MakeOneSection(currentSection);
    lastSectionMode='D';
    UpdateMatrixLastEndPoint(dsi->mat);
}

void Teach::RecordPlcSection()
{
    //
}

void Teach::CopyCurrentMatrixTo(double *target)
{
    double robotMatrix[RobotParams::dof];
    for(int i=0; i<RobotParams::dof; ++i){
        robotMatrix[i] = RobotThetaMatrix::Instance()->GetMatrix(i);
    }

    for(int i=0; i<RobotParams::dof; ++i){
        target[i] = robotMatrix[i];
    }
}

void Teach::StopTeach()
{
    if(sectionList.empty() || sectionList[0]->GetMode() != RSectionInfo().GetMode()){
        MyMessageBox::ShowInformMessageBox("错误的示教文件配置!");
        return;
    }

    RSectionInfo *rsi = dynamic_cast<RSectionInfo*>( sectionList[0].get() );
    rsi->calibratingPoints = conf->calibratingPoints;

    SectionInfoPtrList sectionInfoList = sectionList;
    SectionInfo::StringifySectionInfoListIntoFile(sectionInfoList, teachFileName);

    StatusString::Instance()->SetStatusIndex(StatusString::TeachStatus_FinishConstructTeachSection);
    sectionList.clear();
    listUndo.clear();
    listRedo.clear();
    teachStatusCurr = IDLE;
}

int Teach::EditTeach(const std::string &fileName)
{
    teachStatusCurr = Teach::EDIT_TEACH;
    teachFileName=fileName;
    listUndo.clear();
    listRedo.clear();

    SectionInfoPtrList sectionInfoList = SectionInfo::ParseSectionInfoListFromFile(fileName);
    if(sectionInfoList.empty()){
        PRINTF(LOG_ERR, "%s: error parse teach file[%s]\n", __func__, fileName.c_str());
        return -1;
    }
    sectionList = sectionInfoList;

    return 0;
}

void Teach::DeleteSection(int pos)
{
    if(pos >= (int)sectionList.size()){
        return;
    }

    listUndo.push_back(sectionList);
    if(listUndo.size() > MaxListSize){
        listUndo.erase(listUndo.begin());
    }
    listRedo.clear();

    sectionList.erase(sectionList.begin()+pos);
}

void Teach::MakeOneSection(SectionInfoPtr si)
{
    listUndo.push_back(sectionList);
    if(listUndo.size() > MaxListSize){
        listUndo.erase(listUndo.begin());
    }
    listRedo.clear();

    if(GetTeachStatus()==INSERT_TEACH){
        sectionList.insert(sectionList.begin()+insertTeachIndex, si);
        teachStatusCurr = teachStatusLast;
    }else{
        sectionList.push_back(si);
    }
    currentSection.reset();
}

void Teach::RecordPosition(double *matrix, double* theta)
{
    double robotTheta[RobotParams::axisNum];
    for(int i=0; i<RobotParams::axisNum; ++i){
        robotTheta[i] = RobotThetaMatrix::Instance()->GetTheta(i);
    }
    double robotMatrix[RobotParams::dof];
    for(int i=0; i<RobotParams::dof; ++i){
        robotMatrix[i] = RobotThetaMatrix::Instance()->GetMatrix(i);
    }

    for(int i=0;i<RobotParams::dof;i++){
        matrix[i]=robotMatrix[i];
    }
    for(int i=0;i<RobotParams::axisNum;i++){
        theta[i]=robotTheta[i];
    }
}

void Teach::InsertTeach(int insertIndex)
{
    teachStatusLast = teachStatusCurr;
    teachStatusCurr = INSERT_TEACH;
    insertTeachIndex = insertIndex;
}

void Teach::CancelInsertTeach()
{
    teachStatusCurr = teachStatusLast;
}

std::vector<char> Teach::GetSectionsList()
{
    std::vector<char> sectionModeList;
    for(const SectionInfoPtr& section : sectionList){
        char mode = section->GetMode();
        sectionModeList.push_back(mode);
    }
    return sectionModeList;
}

int Teach::GetUndoListCount()
{
    return listUndo.size();
}

int Teach::GetRedoListCount()
{
    return listRedo.size();
}

void Teach::UndoTeach()
{
    if(listUndo.empty()){
        return;
    }

    listRedo.push_back(sectionList);//存入当前的QLsit
    sectionList=listUndo.back();//取出之前的QList
    listUndo.pop_back();
}

void Teach::RedoTeach()
{
    if(listRedo.empty()){
        return;
    }

    listUndo.push_back(sectionList);//存入当前的QLsit
    sectionList=listRedo.back();//取出之前的QList
    listRedo.pop_back();
}

void Teach::ClearCurrentSection()
{
    currentSection.reset();
}

int Teach::ConstructAxisLocateMoveTeachFile(int axisNum, int absoluteAngle, double speed, const std::string & filename)
{
    if(axisNum<0 || axisNum>= RobotParams::axisNum){
        PRINTF(LOG_ERR, "%s: unvalid axisNum(%d)\n", __func__, axisNum);
        return -1;
    }

    //1) R段
    RSectionInfo rsi;
    rsi.calibratingPoints = Configuration::Instance()->calibratingPoints;

    //2） 按过渡段运动
    TSectionInfo tsi;
    tsi.transSpeed = speed;
    RecordPosition(tsi.mat, tsi.theta);
    tsi.theta[axisNum] = absoluteAngle;//单轴运动的目标

    //3) 生成示教文件
    SectionInfoPtrList sectionInfoList;
    sectionInfoList.push_back( SectionInfoPtr(new RSectionInfo(rsi)) );
    sectionInfoList.push_back( SectionInfoPtr(new TSectionInfo(tsi)) );

    bool stringifyOk = SectionInfo::StringifySectionInfoListIntoFile(sectionInfoList, filename);
    return stringifyOk ? 0 : -1;
}

int Teach::ConstructMonitoringTeachFile(MSectionInfo &msi, const std::string &filename)
{
    if(msi.internalCycle < 1){
        PRINTF(LOG_INFO, "%s: internalCycle must > 1!\n",__func__);
        return -1;
    }

    SectionInfoPtrList sectionInfoList;
    sectionInfoList.push_back( SectionInfoPtr(new RSectionInfo()) );
    sectionInfoList.push_back( SectionInfoPtr(new MSectionInfo(msi)) );

    bool stringifyOk = SectionInfo::StringifySectionInfoListIntoFile(sectionInfoList, filename);
    return stringifyOk ? 0 : -1;
}

bool Teach::IsSectionsNearby()
{
    double distance = MyMathLib::CalcEuclideanDistance(matrixCurrStartPoint, matrixLastEndPoint, 3);
    if(distance > ContinuesSegmentsThreshold){
        switch(lastSectionMode){
        case 'C':
        case 'O':
        case 'L':
        case 'S':
            return false;
        default:
            break;
        }
    }
    return true;
}

void Teach::ShowAutoContinuesInformMessageBox()
{
    MyMessageBox::ShowInformMessageBox("两段焊接过程不连续!\n将自动过渡到起始点!");
}

char Teach::QuestionCircleSectionType()
{
    int button = QMessageBox::information(Q_NULLPTR, "Question", QObject::tr("该段示教为圆弧或整圆:"), QObject::tr(" 圆弧段 "), QObject::tr(" 整圆段 "),QObject::tr(" 返回 "));
    switch(button){
    case 0:
        return 'C';
    case 1:
        return 'O';
    default:
        return 0;
    }
}

void Teach::UpdateMatrixLastEndPoint(const double *matrixValue)
{
    for(int i=0; i<RobotParams::dof; ++i){
        matrixLastEndPoint[i] = matrixValue[i];
    }
}
