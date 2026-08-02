#ifndef DRIVERMGR_H
#define DRIVERMGR_H

#include "types.h"

class DriverModel;

class DriverLoader {
public:

    enum AnmEnum {};
    enum AnmSoundEnum {};

    DriverLoader(int);
    void convAnmIDToKeepID(DriverLoader::AnmEnum);
    void getAnmFile(int);
    void convSndIDToKeepID(DriverLoader::AnmSoundEnum);
    void getSoundDataFile(int);
    void createModelData(int, int);


    // Inline/Unused
    void getBmdFile(int, int);

 // static void *mscAnm2SoundList;
 // static void *msAnmMRAMList;
 // static void *mscaaMRAMAnmFileName;
 // static void *mscaaMRAMAnmSoundFileName;
 // static void *sAnmFileNameTable;
 // static void *sAnmSoundFileNameTable;
 // static void *mscpaDriverName;
 // static void *mscpaDriverFolderName2;
 // static void *mscaaJntName;
 // static void *sModelFileNameTable;
 // static void *sDriverFolderName3;
 // static void *msAnmSoundMRAMList;
 // static void *mscaBoneJnt;

}; // class DriverLoader

class DriverManager {
public:
    DriverManager();
    ~DriverManager();
    static DriverManager *ptr();
    const char *getFolderName_Sound(int, int);
    void *SetDriverModelPtr(DriverModel *);

    // Inline/Unused
    bool IsAllARAMDMAFinished();
    void CheckARAMDMA(f64, int);

    static DriverManager *mspDriverManager;
}; // class DriverManager


#endif // DRIVERMGR_H
