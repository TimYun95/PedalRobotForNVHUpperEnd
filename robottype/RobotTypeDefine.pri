#assign the robot type(can only choose one type)

#remember to switch to Release mode to build dll

DEFINES += \
    #DOF_4_MEDICAL #dll
    DOF_2_PEDAL #app
    #DOF_3_PEDAL_API #dll
    #DOF_5_SCARA_PAINT #app
    #DOF_5_IGUS_FLEXIBLE_SHAFT #app
    #DOF_6_XYZ_VESSEL_PUNCTURE #app
    #DOF_6_PEDAL_SHIFTGEAR_SWERVE #dll

contains(DEFINES, DOF_2_PEDAL){
DEFINES += NVH_USED
}
