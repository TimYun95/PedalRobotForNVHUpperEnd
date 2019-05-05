#This file is included in .pro

contains(DEFINES, DOF_5_SCARA_PAINT){
DEFINES += \
    USE_ABS_ENCODER_WIDGET \#used in URI::InitSettingsWidget()
}
