#ifndef __OLED_UI_MENUDATA_H
#define __OLED_UI_MENUDATA_H
// 检测是否是C++编译器
#ifdef __cplusplus
extern "C" {
#endif
#include "OLED_UI.h"

//进行前置声明
extern //菜单项内容数组
MenuItem MainMenuItems[],

	SettingsMenuItems[],    
		AboutThisDeviceMenuItems[],
		AboutOLED_UIMenuItems[],

	VIDEOMenuItems[],
		VIDEOWRITEMenuItems[]
;

extern //菜单属性
MenuPage MainMenuPage,

	SettingsMenuPage,
		AboutThisDeviceMenuPage,
		AboutOLED_UIMenuPage,

	VIDEOMenuPage,
		VIDEOWRITEMenuPage
;


#ifdef __cplusplus
}  // extern "C"
#endif

#endif
