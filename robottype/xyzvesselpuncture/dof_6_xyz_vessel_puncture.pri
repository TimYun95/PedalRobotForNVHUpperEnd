#This file is included in .pro

contains(DEFINES, DOF_6_XYZ_VESSEL_PUNCTURE){
DEFINES += \
    USE_ABS_ENCODER_WIDGET \#used in URI::InitSettingsWidget()
}
