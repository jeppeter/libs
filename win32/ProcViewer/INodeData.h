#ifndef _INODE_DATA_H_
#define _INODE_DATA_H_

/**
 * Copyright(c) 2007 Nibu babu thomas
 *
 * ITEM_TYPE_e - Item data type
 *
 * @author :    Nibu babu thomas
 * @version:    1.0            Date:  2007-06-05
 */
enum ITEM_TYPE_e
{
    IT_PROC_INFO = 100,
    IT_WINDOW_INFO,
    IT_NONE
};


/**
 * Copyright(c) 2007 Nibu babu thomas
 *
 * ITEM_DATA_t - Represents a tree node's item data.
 *
 * @author :    Nibu babu thomas
 * @version:    1.0            Date:  2007-06-05
 */
typedef struct INodeData
{

private:

    friend class CTreeCtrlEx;
    ITEM_TYPE_e eItemType;

public:

    INodeData() : eItemType( IT_NONE ) {}
    virtual ~INodeData(){}

    // Return item type
    ITEM_TYPE_e GetItemType() const
    {
        return eItemType;
    }

    virtual bool DeleteNodeData() const = 0;
}*PINodeData;

#endif //_INODE_DATA_H_