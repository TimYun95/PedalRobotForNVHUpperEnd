#ifndef SETTINGWIDGETINCLUDE_H
#define SETTINGWIDGETINCLUDE_H

#include "settingwidget/sectionattributedialog.h"
#include "settingwidget/settingwidgetencoder.h"
#include "settingwidget/settingwidgetlimit.h"
#include "settingwidget/settingwidgetmachine.h"
#include "settingwidget/settingwidgetmotor.h"
#include "settingwidget/settingwidgetpid.h"
#include "settingwidget/settingwidgetrun.h"
#include "settingwidget/settingwidgetspeed.h"
#include "settingwidget/settingwidgethardwareinterface.h"
#include "settingwidget/settingwidgetdirection.h"
#include "settingwidget/settingwidgetforcecontrol.h"
#include "settingwidget/settingwidgetabsvalueatorigin.h"
#include "settingwidget/settingwidgetgohomespeed.h"
#include "settingwidget/settingwidgeterrorlimit.h"
#include "settingwidget/settingwidgetmachineparams.h"
#include "settingwidget/settingwidgetsingleaxis.h"
#include "settingwidget/settingwidgetmanualcontrol.h"
#include "settingwidget/settingwidgetmaxmonitordifftheta.h"
#ifdef ENABLE_PEDAL_ROBOT_WIDGETS
 #ifdef NVH_USED
  #include "settingwidget/settingwidgetpedalrobotnvhgetspeed.h"
  #include "settingwidget/settingwidgetpedalrobotnvhdeathzone.h"
  #include "settingwidget/settingwidgetpedalrobotnvhcontrolparameter.h"
 #else
  #include "settingwidget/settingwidgetpedalrobotgetspeed.h"
  #include "settingwidget/settingwidgetpedalrobotdeathzone.h"
  #include "settingwidget/settingwidgetpedalrobotstudy.h"
  #include "settingwidget/settingwidgetpedalrobotstudywltc.h"
 #endif
#endif
#ifdef ENABLE_WEBSOCKET_COMMUNICATION
 #include "settingwidget/settingwidgetwebsocket.h"
#endif

#endif // SETTINGWIDGETINCLUDE_H
