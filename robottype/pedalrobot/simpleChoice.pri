# This file is included in dof_2_pedal.pri
# FAKE_CAN                                         ---     for debug using to give fake CAN datas from pedal position
# ENABLE_BRAKE_NVH                        ---     enable brake axis in NVH operation
# ENABLE_MOREMOTORMODE          ---     enable separate mode of motors to run

contains(DEFINES, DOF_2_PEDAL){
DEFINES += \
    FAKE_CAN\
#    ENABLE_BRAKE_NVH\
#    ENABLE_MOREMOTORMODE
}
