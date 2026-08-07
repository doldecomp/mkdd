#ifndef JAUDIO_JAUSOUNDTABLE_H
#define JAUDIO_JAUSOUNDTABLE_H

#include "JSystem/JAudio/Interface/JAISound.h"
#include "JSystem/JAudio/System/JASGadget.h"

struct JAUSoundTableItem
{
    u8 mPriority; // 0
    u8 _1; // 1
    u16 mResourceId; // 2
    u32 _4; // 4
    u16 _8; // 8
    u16 _10; // 10
};

template <typename Root, typename Section, typename Group, typename Typename_0>
struct JAUSoundTable_
{
    JAUSoundTable_()
    {
        mRes = NULL;
        _4 = 0;
    }

    void reset()
    {
        mRes = NULL;
        _4 = NULL;
    }

    void init(const void *res)
    {
        mRes = res;
        // magic number is not in debug rom. I'm not sure what this comparison is (maybe some sort of '' number?)
        // I also do not know how it is different between JAUSoundTable and JAUSoundNameTable
        if (*(u32 *)mRes + 0xbdad0000 != Root::magicNumber())
        {
            mRes = NULL;
        }
        else
        {
            _4 = (Root *)((u8 *)mRes + *((u32 *)mRes + 3));
        }
    }

    Section *getSection(int index) const
    {
        if (index < 0)
        {
            return NULL;
        }
        if ((u32)index >= _4->mSectionNumber)
        {
            return NULL;
        }
        u32 offset = _4->mSectionOffsets[index];
        if (offset == 0)
        {
            return NULL;
        }
        return (Section *)((u8 *)mRes + offset);
    }

    Group *getGroup(Section *section, int index) const
    {
        int iVar1;

        if (index < 0)
        {
            return NULL;
        }
        if ((u32)index >= section->mNumGroups)
        {
            return NULL;
        }
        u32 offset = section->getGroupOffset(index);
        if (offset == 0)
        {
            return NULL;
        }
        return (Group *)((u8 *)mRes + offset);
    }

    const void *mRes;
    Root *_4;
    //u32 _8;
    //u32 _c;
};

struct JAUSoundTableRoot
{
    static inline u32 magicNumber() { return 0x5420; }
    u32 mSectionNumber;
    u32 mSectionOffsets[0];
};

struct JAUSoundTableSection
{
    int getGroupOffset(int index) const
    {
        if (index < 0)
        {
            return 0;
        }
        if (index >= mNumGroups)
        {
            return 0;
        }
        return mGroupOffsets[index];
    }

    u32 mNumGroups;
    u32 mGroupOffsets[0];
};

struct JAUSoundTableGroup
{
    u8 getTypeID(int index) const
    {
        if (index < 0)
        {
            return 0;
        }
        if (index >= mNumItems)
        {
            return 0xff;
        }
        return mTypeIds[index * 4];
    }

    u32 getItemOffset(int index) const
    {
        if (index < 0)
        {
            return 0;
        }
        if (index >= mNumItems)
        {
            return 0;
        }
        return *(u32 *)(mTypeIds + index * 4) & 0xffffff;
    }

    u32 mNumItems;
    u32 _4;
    u8 mTypeIds[0];
};

struct JAUSoundTable : public JASGlobalInstance<JAUSoundTable>
{
    JAUSoundTable(bool p1) : JASGlobalInstance<JAUSoundTable>(p1)
    {
    }
    ~JAUSoundTable() {}

    void init(void const *);
    u8 getTypeID(JAISoundID) const;
    JAUSoundTableItem *getData(JAISoundID) const;
    u32 getNumGroups_inSection(u8) const;
    u32 getNumItems_inGroup(u8, u8) const;

    JAUSoundTableItem *getItem(JAUSoundTableGroup *group, int index) const
    {
        u32 offset = group->getItemOffset(index);
        if (offset == 0)
        {
            return NULL;
        }
        return (JAUSoundTableItem *)((u8 *)mTable.mRes + offset);
    }

    const void *getResource() { return mTable.mRes; }

    JAUSoundTable_<JAUSoundTableRoot, JAUSoundTableSection, JAUSoundTableGroup, void> mTable;
};

struct JAUSoundNameTableRoot
{
    static inline u32 magicNumber() { return 0x544e; }
    u32 mSectionNumber;
    u32 mSectionOffsets[0];
};

struct JAUSoundNameTableSection
{
};

struct JAUSoundNameTableGroup
{
};

struct JAUSoundNameTable : public JASGlobalInstance<JAUSoundNameTable>
{
    JAUSoundNameTable(bool p1) : JASGlobalInstance<JAUSoundNameTable>(p1)
    {
    }
    ~JAUSoundNameTable() {}
    void init(const void *);

    JAUSoundTable_<JAUSoundNameTableRoot, JAUSoundNameTableSection, JAUSoundNameTableGroup, char> mTable;
};

#endif
