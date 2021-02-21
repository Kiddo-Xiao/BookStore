#ifndef SRC_SQUARELINKLIST_H
#define SRC_SQUARELINKLIST_H

#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <fstream>
#include <algorithm>
//定义常数
#define cutremain 160
#define maxblock 320
#define maxkey 65
#define mergeedge 288

using namespace std;

//定义类
class Element {
    //一对key&offset；重载<寻找元素；维护key升序
public:
    int offset;
    char key[maxkey];

    bool operator<(const Element &ele) const;

    Element(int offset_ = -1, const string &key_ = "");

    Element(const Element &ele);

    Element &operator=(const Element &ele);
};

class Block {
    //块状链表中一个块，若干Block组成整个块状链表
    //存放上下Block的地址（无-1）；当前块内元素个数sum，长度为Block_size的数组，元素按key升序
public:
    int last, next, sum;
    Element array[maxblock];

    Block();
};

class SquareLinkList {
    //对外接口addElement,findElement,deleteElement
    //由若干Block类元素排列组成，首块默认文件开头
    //-1|Block_1|Block_2|...|Block_n|-1
private:
    string filename;
    fstream fin, fout, fin_next;//读入，写出，从下一Block中读入（13徐同时使用

    int next_offset(int this_offset);

    void mergeBlock(int this_offset, int next_offset);

    void cutBlock(int this_offset);

public:
    SquareLinkList(const string &filename_) : filename(filename_) {}

    void addElement(const Element &ele);
    //具体实现算法为：
    //从第一个块开始读入块的第一个Element，比较其与添加元素key的大小，不满足则按照块内存放的下个块的地址，再去找该块的第一个Element进行比较，如此找到添加元素应添加到哪一个块内
    //然后在块内利用二分查找确定元素的插入位置，将插入位置后的元素统一后移一位，再将添加元素放置到相应位置
    //若添加元素后块内数组被存满（即length==BLOCK_SIZE）则分裂被添加元素的块

    void findElement(const string &key, vector<int> &result);//找key相同元素的offset push_back进result

    void deleteElement(const Element &ele);
    //具体实现算法为：
    //与addElement同理找到删除元素所在的块
    //在块内利用二分查找确定删除元素的位置（要求offset也相同），直接将该位置后的元素统一前移一位覆盖掉该元素即可
    //若删除元素后，被删除元素的块该块后面一个块元素个数之和小于BLOCK_SIZE，则合并这两个块
};

#endif //SRC_SQUARELINKLIST_H
