#ifdef __cplusplus
#define EXPORT extern "C" __declspec(dllexport)
#else
#define EXPORT __declspec(dllexport)
#endif

#define MAX_STRINGS 256
#define MAX_LENGTH	63

typedef BOOL (CALLBACK *GetStrCb)(PCTSTR, PVOID);

EXPORT BOOL CALLBACK AddString(PCTSTR);
EXPORT BOOL CALLBACK DeleteString(PCTSTR);
EXPORT BOOL CALLBACK GetStrings(GetStrCb, PVOID);