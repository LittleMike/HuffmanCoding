#include<stdio.h>
#include<stdlib.h>

/* 各种结构 */
//Huffman树结点
struct HuffmanNode
{
    //指向左右孩子的指针
    struct HuffmanNode* left;
    struct HuffmanNode* right;
    char data;//数据域
    int weight;//权值
};

//字符频率结构
struct CharFrency
{
    char mChr;//存放字符
    int mFrency;//存放字符频率
};

//编码结构
struct Code
{
    char mchr;//字符
    int mCNum;//编码位数
    int* mCode;//编码
};
//Huffman树结构
struct HuffmanTree
{
    struct HuffmanNode* root;//树根
    struct Code* mCodeArray;//字符编码结构数组
} ht;//定义一个树变量

static int CodeStack[128];//用数组模拟栈，存编码
static int top = 0;//模拟栈顶指针，也可以作为编码个数

static char* data = NULL;//数据域，字符种数不定
static int* weight = NULL;//权值部分，需要已排好序的
static int M = 0;;//结点个数,字符种数
static int imCodeArray = 0;//HuffmanTree的mCodeArray[]的整数指针

/************************************************************************/
//Param:字符频率结构数组，字符个数
void SortByWeight(struct CharFrency cf[],int num)
{
    //定义
    int i,j;
    struct CharFrency tmp;
    //语句
    if(cf == NULL) return;
    //使用直接插入排序
    for(j=1; j<num; j++)
    {
        tmp = cf[j];
        i = j - 1;
        while(tmp.mFrency<cf[i].mFrency && i>=0)
        {
            cf[i+1] = cf[i];
            i--;
        }
        cf[i+1] = tmp;
    }
}

//输入各结点的data和weight值
void InputNodes(struct CharFrency cf[],int num)
{
    int i;
    if(cf == NULL) return;
    M = num;//M得到值
    data = (char*)malloc(num * sizeof(char));
    weight = (int*)malloc(num * sizeof(int));
    for(i=0; i<num; i++)
    {
        data[i] = cf[i].mChr;
        weight[i] = cf[i].mFrency;
    }
}
//释放动态申请的空间
void DeleteDataAndWeight()
{
    if(data != NULL) free(data);
    if(weight != NULL) free(weight);
}

//构造Huffman树
void CreateHuffmanTree()
{
    struct HuffmanNode** H = (struct HuffmanNode**)malloc(M *
                             sizeof(struct HuffmanNode*));
    struct HuffmanNode* p1 = NULL;
    struct HuffmanNode* p2 = NULL;
    struct HuffmanNode* p;
    struct HuffmanNode* t;
    int i=0;
    int j=0;
    //初始化结点
    for(i=0; i<M; i++)
    {
        H[i] = (struct HuffmanNode*)malloc(sizeof(struct HuffmanNode));
        H[i]->data = data[i];
        H[i]->weight = weight[i];
        H[i]->left = NULL;
        H[i]->right = NULL;
    }
    for(i=0; i<M-1; i++) //组合过程
    {
        t = (struct HuffmanNode*)malloc(sizeof(struct HuffmanNode));
        p1 = H[i];
        p2 = H[i+1];
        t->weight = p1->weight + p2->weight;
        t->left = p1;
        t->right = p2;
        p = t;
        //把新组合结点t的地址p插入到数组H中，使得Weight(H[i+1])<=Weight(H[M])
        j = i + 2;
        while(j<M && (p->weight) > (H[j]->weight))
        {
            H[j-1] = H[j];
            j = j + 1;
        }
        H[j-1] = p;
    }
    ht.root = H[M-1];
    free(H);//释放掉之前申请的指针数组
    DeleteDataAndWeight();//释放掉Data域和Weight域
}

//CreateCoding()的子函数
void Create(struct HuffmanNode* t,int* top,int value)
{
    if(t!=NULL)
    {
        CodeStack[*top] = value;
        if(t->left != NULL)
        {
            (*top)++;
            Create(t->left,top,0);
            Create(t->right,top,1);
            (*top)--;
        }
        else //为编码结构赋值
        {
            //static int imCodeArray = 0;//这里需要注意一下,重复操作时值的设定
            int* tt = NULL;
            int j;
            ht.mCodeArray[imCodeArray].mchr = t->data;
            ht.mCodeArray[imCodeArray].mCNum = *top;
            tt = (int*)malloc((*top) * sizeof(int));
            for(j=1; j<=*top; j++)
            {
                tt[j-1] = CodeStack[j];
            }
            ht.mCodeArray[imCodeArray].mCode = tt;
            imCodeArray++;
        }
    }
}
//构造编码
void CreateCoding()
{
    if(ht.root == NULL) return;
    ht.mCodeArray = (struct Code*)malloc(M * sizeof(struct Code));
    Create(ht.root,&top,-1);
}

//Huffman编码 Param:结构数组，个数
void HuffmanCoding(struct CharFrency cf[],int num)
{
    SortByWeight(cf,num);
    InputNodes(cf,num);
    CreateHuffmanTree();
    CreateCoding();
}
//释放树结点，Reset()的子函数
void freeTreeNode(struct HuffmanNode* t)
{
    if(t != NULL)
    {
        if(t->left != NULL)
        {
            freeTreeNode(t->left);
            freeTreeNode(t->right);
            free(t);
            //printf("根");
        }
        else
        {
            free(t);
            //printf("叶");
        }
    }
}
//重置一棵Huffman树
void Reset()
{
    int i;
    //释放编码结构
    for(i=0; i<M; i++)
    {
        free(ht.mCodeArray[i].mCode);
    }
    free(ht.mCodeArray);
    //释放树结点
    freeTreeNode(ht.root);
    //重置变量的值
    M = 0;
    imCodeArray = 0;
    top = 0;
}

//其他操作
struct HuffmanNode* GetRoot()
{
    return ht.root;   //得到Huffman树
}
void SetRoot(struct HuffmanNode* t)
{
    ht.root = t;
}
struct Code* GetCode()
{
    return ht.mCodeArray;   //得到Huffman编码
}

/* 显示测试函数 */
//显示编码结构信息
void DisplayCode()
{
    int i,j;
    for(i=0; i<M; i++)
    {
        printf("%c %d ",ht.mCodeArray[i].mchr,ht.mCodeArray[i].mCNum);
        for(j=0; j<ht.mCodeArray[i].mCNum; j++)
        {
            printf("%d",ht.mCodeArray[i].mCode[j]);
        }
        printf("\n");
    }
}
