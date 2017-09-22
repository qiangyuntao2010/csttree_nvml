/*************************************************************************
	> File Name: csttree.h
	> Author: 
	> Mail: 
	> Created Time: Wed 20 Sep 2017 01:55:14 PM KST
 ************************************************************************/

#ifndef _CSTTREE_H
#define _CSTTREE_H

#include <math.h>

#define ROWS 7
#define LISTS 4
#define LEVL_NODE_NUM_MAX
#define KV_T 
#define NODE_KV_NUM
#define MAXARR_KEY_NUM 7 

namespace stx{

typedef struct
{

}dm_meta;

typedef struct
{

}pm_meta;

typedef struct
{
    int key;
    int value;
}kv_pair;

typedef struct
{
    KV_T maxarr[MAXARR_KEY_NUM];
    kv_list* data_node_addr;//double dimensional array with structure or something... 
    key_node* leftchild;
    key_node* rightchild;
}key_node;

typedef struct
{
    kv_pair kv_data[ROWS][LISTS];
}kv_list;

typedef struct
{
    kv_pair array[NODE_KV_NUM];       
}data_node;

class csttree
{
public:
    
keycompare(KV_T key1, KV_T key2)
{
    return key1 < key2 ? -1 : key1 == key2 ? 0 : 1;
}

key_node* Findmin(key_node* tnode)
{
    if(tnode != NULL)
    {
        if(tnode->leftchild == NULL)
        {
            return tnode;
        }
        else
        {
            return Findmin(tnode->chil_addr);
        }
    }
    fprintf(stdout,"FUNCTION %s : error!\n",__func__);
    return NULL;
}

key_node* FindMax(key_node* tnode)
{
    if(tnode != NULL)
    {
        if(tnode->rightchild == NULL)
        {
            return tnode;
        }
        else
        {
            return FindMax(tnode->rightchild);
        }
    }
    fprintf(stdout,"FUNCTION %s : error!\n",__func__);
    return NULL;
}

/*KV_T FindMinKey(key_node* tnode)
{
    if(tnode->maxarr[3] > 0)
    {
        return tnode->maxarr[3];
    }
    else if(tnode->maxarr[4] > 0)
    {
        return tnode->maxarr[4];
    }
    else if(tnode->maxarr[1] > 0)
    {
        return tnode->maxarr[1];
    }
    else if(tnode->maxarr[0] > 0)
    {
        return tnode->maxarr[0];
    }
    else
    {
        return false;
    }
}

KV_T FindMaxKey(key_node* tnode)
{
    if(tnode->maxarr[6] > 0)
    {
        return tnode->maxarr[6];
    }
    else if(tnode->maxarr[5] > 0)
    {
        return tnode->maxarr[5];
    }
    else if(tnode->maxarr[2] > 0)
    {
        return tnode->maxarr[2];
    }
    else if(tnode->maxarr[0] > 0)
    {
        return tnode->maxarr[0];
    }
    else
    {
        return false;

    }
}*/



KV_T Find(KV_T key);
{
    key_node* tnode = root;
    while(tnode != NULL)
    {
        for(char count = 1; count < (MAXARR_KEY_NUM-1);)
        {
            if(key < tnode->maxarr[count-1])
            {
                if(count == 4)
                {
                    if((key > tnode->leftchild->maxarr[0]) == 1)
                    {
                        for(int inter_count = 0; inter_count < (LISTS-1); inter_count++)
                        {
                            if(tnode->data_node_addr->kv_data[count-1][inter_count].key == key)
                            {
                                return tnode->data_node_addr->kv_data[count-1][inter_count].value;
                            }
                            else
                            {
                                fprintf(stdout,"FUNCTION %s : Can not find the key-value\n");
                                return false;
                            }
                        }
                    }
                    else if
                    {
                        tnode = tnode->leftchild;
                        break;
                    }
                }
                else
                {    
                    if(((key <= tnode->maxarr[count*2-1])&&(count < 4)) == 1)
                    {
                        count *= 2;
                    }
                    else
                    {
                        for(int inter_count = 0; inter_count < (LISTS-1); inter_count++)
                        {
                            if(tnode->data_node_addr->kv_data[count-1][inter_count].key == key)
                            {
                                return tnode->data_node_addr->kv_data[count-1][inter_count].value;
                            }
                            else
                            {
                                fprintf(stdout,"FUNCTION %s : Can not find the key-value\n");
                                return false;
                            }
                        }
                    }
                }
            }
            else if(key = tnode->maxarr[count-1])
            {
                return tnode->maxarr[count-1];
            }
            else if(key > tnode->maxarr[count-1])
            {
                if(count < MAXARR_KEY_NUM)
                {
                    count = count * 2 + 1;
                }
                else
                {
                    tnode = tnode->rightchild;
                    break;
                }
            }
        }
    }
    fprintf(stdout,"FUNCTION %s : Did not find the key-value\n");
}














































































































/*    {
        if(tnode->maxarr[0] => 0)
        {
            for(int co xunt = 1; count <= 4;)
            {
                if(tnode->maxarr[count*2-1] < 0)
                {
                    if(tnode->maxarr[count*2] < 0)
                    {
                        return tnode->maxarr[count-1];
                    }
                    else
                    {
                        return tnode->maxarr[count*2];
                    }
                }
            
            count += 2;
    
        
    }*/
    
}

}




}






















#endif
