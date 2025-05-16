#pragma once


#define StringToWString(str) MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);