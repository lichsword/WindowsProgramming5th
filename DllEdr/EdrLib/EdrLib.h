#ifdef __cplusplus
#define EXPORT extern "C" __declspec(dllexport)
#else
#define EXPORT __declspec(dllexport)
#endif

EXPORT BOOL CALLBACK EdrCenterText(HDC, PRECT, PTSTR);