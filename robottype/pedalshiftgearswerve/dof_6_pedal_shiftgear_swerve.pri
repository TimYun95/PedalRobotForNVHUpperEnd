#This file is included in .pro

#NOTICE: put this include before include(robottype/pedalrobot/dof_2_pedal.pri)

contains(DEFINES, DOF_6_PEDAL_SHIFTGEAR_SWERVE){
DEFINES += \
    DOF_3_PEDAL_API \#serve as this type
    USE_ABS_ENCODER_WIDGET \#used in URI::InitSettingsWidget()
}
