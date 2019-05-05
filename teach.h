#ifndef TEACH_H
#define TEACH_H

#include "common/sectioninfo.h"
#include "common/globalvariables.h"
#include "configuration.h"

class Teach
{
public:
    explicit Teach();
    virtual ~Teach();

    enum TeachStatus{
        IDLE=0,
        NEW_TEACH,
        EDIT_TEACH,
        INSERT_TEACH,

        MAX_TEACH_STATUS
    };

    bool StartTeach();

    void RecordTransSection();
    void RecordSuspendSection();
    void RecordJointSection();
    int RecordLineSectionStartPoint(double weldSpeed, int vibType, double vibrationFreqFB=0, double vibrationAmplFB=0, double vibrationFreqLR=0, double vibrationAmplLR=0);
    int RecordLineSectionEndPoint();
    int RecordCircleSectionPoint1(double weldSpeed, int vibType, double vibrationFreqFB=0, double vibrationAmplFB=0, double vibrationFreqLR=0, double vibrationAmplLR=0);
    int RecordCircleSectionPoint2();
    int RecordCircleSectionPoint3();
    int RecordSegmentSectionFirst(double weldSpeed, int vibType, double vibrationFreqFB=0, double vibrationAmplFB=0, double vibrationFreqLR=0, double vibrationAmplLR=0);
    int RecordSegmentSectionNext();
    int RecordSegmentSectionFinish();
    int UndoRecordSegmentSection();
    int GetSegmentSectionPointsNum();
    void RecordDotSection(int dotTime);
    void RecordPlcSection();

    void StopTeach();
    int EditTeach(const std::string &fileName);
    void DeleteSection(int pos);
    void MakeOneSection(SectionInfoPtr);
    void UndoTeach();
    void RedoTeach();
    void ClearCurrentSection();

    TeachStatus GetTeachStatus();
    SectionInfo* GetSectionInfo(int sectionIndex);
    static void RecordPosition(double*matrix, double*theta);
    void InsertTeach(int insertIndex);
    void CancelInsertTeach();
    std::vector<char> GetSectionsList();
    int GetUndoListCount();
    int GetRedoListCount();
    void CopyCurrentMatrixTo(double *target);

    //生成 单轴定位运动的示教文件
    static int ConstructAxisLocateMoveTeachFile(int axisNum, int absoluteAngle, double weldSpeed, const std::string &filename);
    //生成 进入监听模式的示教文件
    static int ConstructMonitoringTeachFile(MSectionInfo &msi, const std::string &filename);

protected:
    bool IsSectionsNearby();
    void ShowAutoContinuesInformMessageBox();
    char QuestionCircleSectionType();

    void UpdateMatrixLastEndPoint(const double *matrixValue);

private:
    Configuration *conf;

    std::string teachFileName;
    TeachStatus teachStatusCurr;
    TeachStatus teachStatusLast;

    std::vector<SectionInfoPtr> sectionList;
    std::vector< std::vector<SectionInfoPtr> > listUndo;//撤销
    std::vector< std::vector<SectionInfoPtr> > listRedo;//重做

    char lastSectionMode;
    int insertTeachIndex;

    double matrixCurrStartPoint[RobotParams::dof];
    double matrixLastEndPoint[RobotParams::dof];

    SectionInfoPtr currentSection;

    double matrixArc1[RobotParams::dof];
    double matrixArc2[RobotParams::dof];
    double matrixArc3[RobotParams::dof];
};

#endif // TEACH_H
