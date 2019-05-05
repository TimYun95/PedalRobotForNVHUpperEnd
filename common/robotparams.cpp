#include "robotparams.h"

#if defined DOF_4_MEDICAL
const char* RobotParams::robotType="dof_4_medical";
const char* RobotParams::robotFolder="MedicalRobot";
#elif defined(DOF_6_PEDAL_SHIFTGEAR_SWERVE)
const char* RobotParams::robotType="dof_6_autodrive";
const char* RobotParams::robotFolder="AutoDriveRobot";
#elif defined(DOF_2_PEDAL) || defined(DOF_3_PEDAL_API)
const char* RobotParams::robotType="dof_2_pedal";
const char* RobotParams::robotFolder="PedalRobot";
#elif defined DOF_5_SCARA_PAINT
const char* RobotParams::robotType="dof_5_scara_paint";
const char* RobotParams::robotFolder="ScaraPaintRobot";
#elif defined DOF_5_IGUS_FLEXIBLE_SHAFT
const char* RobotParams::robotType="dof_5_igus_flexible_shaft";
const char* RobotParams::robotFolder="IgusFlexibleShaft";
#elif DOF_6_XYZ_VESSEL_PUNCTURE
const char* RobotParams::robotType="dof_6_xyz_vessel_puncture";
const char* RobotParams::robotFolder="XyzVesselPuncture";
#endif
