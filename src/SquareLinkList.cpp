#include "SquareLinkList.h"

//Element:
bool Element::operator<(const Element &ele) const {
    return strcmp(key,ele.key)<0;
}

Element::Element(int offset_, const string &key_):offset(offset_){
    memset(key,0,sizeof(key));//key[]统改0
    strcpy(key,key_.c_str());
}

Element::Element(const Element &ele) {
    memset(key,0,sizeof(key));
    strcpy(key,ele.key);
}

Element &Element::operator=(const Element &ele) {
    if(this==&ele) return *this;
    offset=ele.offset;
    memset(key,0,sizeof(key));
    strcpy(key,ele.key);
    return *this;
}

//Block
Block::Block(): last(-1),next(-1),sum(0){}//无-1

//SquareLinkList
int SquareLinkList::next_offset(int this_offset) {//fin&fin_next需同时打开
    fin_next.open(filename,ios::in|ios::binary);//二进制打开
    if(!fin_next)cerr<<"[Error] Fail open next Block in SquareLinkList"<<endl;
    fin_next.seekg(this_offset + sizeof(int));
    int next;
    fin_next.read(reinterpret_cast<char *>(&next), sizeof(int));
    fin_next.close();
    return next;
}

void SquareLinkList::mergeBlock(int this_offset, int next_offset) {
    fin.open(filename,ios::in|ios::binary);
    fout.open(filename,ios::in|ios::out|ios::binary);
    if((!fin)|(!fout))cerr<<"[Error] Fail open next SquareLinkList in SquareLinkList"<<endl;

    Block block_1,block_2;
    fin.seekg(this_offset);
    fin.read(reinterpret_cast<char*>(&block_1),sizeof(Block));
    fin.seekg(next_offset);
    fin.read(reinterpret_cast<char*>(&block_2),sizeof(Block));
    //block1->next连block2->next
    block_1.next=block_2.next;
    //block2->next->last连block1(block左右都连）
    if(block_2.next>=0){
        fout.seekp(block_2.next+sizeof(int));
        fout.write(reinterpret_cast<const char *>(&this_offset), sizeof(int));
    }
    for(int i=0;i<block_2.sum;++i)block_1.array[i+block_2.sum]=block_2.array[i];//next元素连在this后
    block_1.sum+=block_2.sum;//merge sum
    fout.seekp(this_offset);//定位原this_block
    fout.write(reinterpret_cast<char*>(&block_1),sizeof(Block));//new block_1 放回

    fin.close();
    fout.close();
}

void SquareLinkList::cutBlock(int this_offset) {
    fin.open(filename,ios::in|ios::binary);
    fout.open(filename,ios::in|ios::out|ios::binary);
    if((!fin)|(!fout))cerr<<"[Error]Fail open in SquareLinkList cutBlock"<<endl;

    fin.seekg(0,ios::end);//新建一个block(从尾开始偏移量为0)
    int tmp=fin.tellg();//tmp为读取指针位置
    Block this_block,tmp_block;
    fin.seekg(this_offset);//改指针定位
    fin.read(reinterpret_cast<char*>(&this_block),sizeof(Block));//读取位置，读取大小（范围）

    //tmp->last=this
    tmp_block.last=this_offset;
    //tmp->next=this->next
    tmp_block.next=this_block.next;
    //this->next->last=tmp
    if(this_block.next>=0){
        fout.seekp(this_block.next+sizeof(int));
        fout.write(reinterpret_cast<char*>(&tmp),sizeof(int));
    }
    //this->next=tmp
    this_block.next=tmp;

    for(int i=cutremain;i<maxblock;++i)tmp_block.array[i-cutremain]=this_block.array[i];
    this_block.sum=cutremain;
    tmp_block.sum=maxblock-cutremain;

    fout.seekp(tmp);
    fout.write(reinterpret_cast<char*>(&tmp_block),sizeof(Block));
    fout.seekp(this_offset);//seekp(int型位置)；write(...<char*>(&block),dizeof(Block)
    fout.write(reinterpret_cast<char*>(&this_block),sizeof(Block));

    fin.close();
    fout.close();
}

void SquareLinkList::addElement(const Element &ele){
    fin.open(filename,ios::in|ios::binary);
    fout.open(filename,ios::in|ios::out|ios::binary);
    if((!fin)|(!fout))cerr<<"[Error] Fail open in SquareLinkList addElement"<<endl;

    //cout<<ele.key<<endl;
    fin.seekg(0,ios::end);
    if(fin.tellg()==0){//空
        Block tmp;
        fout.write(reinterpret_cast<const char*>(&tmp),sizeof(Block));//添加一列ele
    }

    fin.seekg(0);//首
    int now=fin.tellg();//now=首地址
    int next=next_offset(now);//next=next首地址
    Element nextelement;
    while(next!=-1){
        fin.seekg(next+3*sizeof(int));
        fin.read(reinterpret_cast<char*>(&nextelement),sizeof(Element));
        if(ele<nextelement) break;//此时next为ele之后的第一个block
        now=next;
        next=next_offset(now);
    }

    Block tmpblock;
    fin.seekg(now);
    fin.read(reinterpret_cast<char*>(&tmpblock),sizeof(Block));
    bool flag=true;
    int sum=tmpblock.sum;
    int pos;
    for(int i=0;i<sum;++i){
        if(ele<tmpblock.array[i]){
            pos=i,flag=false;
//            cout<<"pos:"<<pos<<endl;
            break;//pos为ele之后的第一个element
        }
    }
    if(flag)pos=sum;//pos在block末端
//    cout<<"pos:"<<pos<<endl;
    for(int i=sum-1;i>=pos;--i)tmpblock.array[i+1]=tmpblock.array[i];//依次后移
    tmpblock.array[pos]=ele;//顺利插入正确位置
    tmpblock.sum=sum+1;
//    cout<<tmpblock.array[pos].key;√
//    cout<<tmpblock.sum;√
    fout.seekp(now);
    fout.write(reinterpret_cast<const char*>(&tmpblock),sizeof(Block));

    fin.close();
    fout.close();
    if(sum==maxblock)cutBlock(now);
}

void SquareLinkList::findElement(const string &key, vector<int> &result) {
    fin.open(filename, ios::in | ios::binary);
    if (!fin)cerr << "[Error] Fail open in SquareLinkList findElement" << endl;

    fin.seekg(0, ios::end);
    if (fin.tellg() == 0) {//正序偏移为0，成立即流为空
        Block tmp;
        fout.open(filename, ios::in | ios::out | ios::binary);
        fout.write(reinterpret_cast<const char *>(&tmp), sizeof(Block));
        fout.close();
    }
    char key_[maxkey] = {0};
    for (int i = 0; i < key.length(); ++i)key_[i] = key[i];

    fin.seekg(0);//正序从0读取
    int now = fin.tellg();
    int next = next_offset(now);
    Element nextElement;
    while (next != -1) {//kay与每列block的首元素比较
        fin.seekg(next + 3 * sizeof(int));
        fin.read(reinterpret_cast<char *>(&nextElement), sizeof(Element));//读取首元素
        if (strcmp(key_, nextElement.key) <= 0) break;//key升序，now<next时break
        now = next;
        next = next_offset(now);
    }
    //已锁定列
    bool flag = true;
    while (flag && (now>=0)) {
        flag = false;
        Block tmpblock;
        Element tmpelement(-1, key);
        fin.seekg(now);
        fin.read(reinterpret_cast<char *>(&tmpblock), sizeof(Block));//定位该列列首
        int sum = tmpblock.sum;
        int pos = lower_bound(tmpblock.array, tmpblock.array + sum, tmpelement) -
                  tmpblock.array;//array中第一个<=tmpele并返回该地址，-array得下标

        for (int i = pos; i < sum; ++i) {
            if (strcmp(tmpblock.array[i].key, key_) == 0) {
                result.push_back(tmpblock.array[i].offset);
                if (i == sum - 1) flag = true, now = next_offset(now);
            }
            if (strcmp(tmpblock.array[i].key, key_) < 0) break;
        }
        if (pos == sum)flag = true, now = next_offset(now);

        fin.close();
    }
}

void SquareLinkList::deleteElement(const Element &ele) {
    fin.open(filename, ios::in | ios::binary);
    fout.open(filename, ios::in | ios::out | ios::binary);
    if ((!fin) || (!fout))cerr << "[Error] Fail open in SquareLinkList deleteElement" << endl;

    fin.seekg(0, ios::end);
    if (fin.tellg() == 0) {//空
        puts("empty");
        Block tmpblock;
        fout.write(reinterpret_cast<char *>(&tmpblock), sizeof(Block));
    }

    fin.seekg(0);//首位
    int now = fin.tellg();
    int next = next_offset(now);
    Element nextelement;
    while (next != -1) {//比较block首元素确定block
        fin.seekg(next + 3 * sizeof(int));
        fin.read(reinterpret_cast<char *>(&nextelement), sizeof(Element));
//        Block tmp;
//        fin.read(reinterpret_cast<char *>(&tmp), sizeof(Block));
//        cout << "sum = " << tmp.sum << endl;
//        for(int i = 0; i < tmp.sum; i++)
//            cout << tmp.array[i].key << endl;
        if (strcmp(ele.key, nextelement.key) < 0)break;//next为ele后第一个
        now = next;
        next = next_offset(now);
    }

    Block tmpblock;
    Element tmpelement(ele);
    int pos = -1, sum = tmpblock.sum;
    while (pos < 0 && now >= 0) {
        fin.seekg(now);
        fin.read(reinterpret_cast<char *>(&tmpblock), sizeof(Block));
        sum = tmpblock.sum;
//        puts("Before");
//        cout << "sum = " << sum
//        for(int i = 0; i < sum; i++)
//            cout << tmpblock.array[i].key << endl;

        int samekey_pos = lower_bound(tmpblock.array, tmpblock.array + sum, tmpelement) - tmpblock.array;

        for (int i = samekey_pos; i < sum; ++i) {
            if (strcmp(tmpblock.array[i].key, ele.key) == 0 && (tmpblock.array[i].offset == ele.offset)) {
                pos = i;
                break;
            }//pos为ele（now）的偏移量
        }
        if (pos < 0)now = next_offset(now);
    }
    for (int i = pos; i < sum; ++i)tmpblock.array[i] = tmpblock.array[i + 1];//依次前推
    tmpblock.sum--;
//    puts("After");
//    for(int i = 0; i < sum; i++)
//        cout << tmpblock.array[i].key << endl;
    fout.seekp(now);
    fout.write(reinterpret_cast<char *>(&tmpblock), sizeof(Block));
    next = next_offset(now);
    int nextsum;
    if (next > 0) {
        fin.seekg(next + 2 * sizeof(int));
        fin.read(reinterpret_cast<char *>(&nextsum), sizeof(int));
    }

    fin.close();
    fout.close();

    if (next >= 0 && (sum + nextsum <= mergeedge))mergeBlock(now, next);
}

