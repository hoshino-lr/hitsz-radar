#include <mapping.h>

using namespace std;

//构造函数
KdTree::KdTree()
{
    parent = NULL;
    leftChild = NULL;
    rightChild = NULL;
}
//判断kd树为空
bool KdTree::isEmpty()
{
    return root.empty();
}
//判断是否为叶节点
bool KdTree::isLeaf()
{
    return (!root.empty()) && leftChild == NULL && rightChild == NULL;

}
//判断是否为根节点
bool KdTree::isRoot()
{
    return (!root.empty()) && parent == NULL;
}
//判断是否为父节点的左节点
bool KdTree::isLeft()
{
    return parent->leftChild->root == root;
}
//判断是否为父节点的右节点
bool KdTree::isRight()
{
    return parent->rightChild->root == root;
}


vector<vector<int> > Transpose(vector<vector<int> > Matrix)
{
    unsigned row = Matrix.size();
    unsigned col = Matrix[0].size();
    vector<vector<int> > Trans(col, vector<int>(row, 0));
    for (unsigned i = 0; i < col; ++i)
    {
        for (unsigned j = 0; j < row; ++j)
        {
            Trans[i][j] = Matrix[j][i];
        }
    }
    return Trans;
}

double measureDistance(vector<int> point1, vector<int> point2)
{
    if (point1.size() != point2.size())
    {
        cerr << "Dimensions don't match！！";
        exit(1);
    }
    double res = 0;
    for (unsigned i = 0; i < point1.size(); ++i)
    {
        res += pow((point1[i] - point2[i]), 2);
    }
    return sqrt(res);
}

int findMiddleValue(vector<int> vec)
{
    sort(vec.begin(), vec.end());
    int pos = vec.size() / 2;
    return vec[pos];
}

//构建kd树
void buildKdTree(KdTree* tree, vector<vector<int> > data, unsigned depth)
{
    //样本的数量
    unsigned samplesNum = data.size();
    //终止条件
    if (samplesNum == 0)
    {
        return;
    }
    if (samplesNum == 1)
    {
        tree->root = data[0];
        return;
    }
    //样本的维度
    unsigned k = data[0].size();
    vector<vector<int> > transData = Transpose(data);
    //选择当前切分维度
    unsigned splitAttribute = depth % k;
    vector<int> splitAttributeValues = transData[splitAttribute];
    //选择切分值（当前维度中值）
    double splitValue = findMiddleValue(splitAttributeValues);

    // 根据选定的切分维度和切分值，将数据集分为两个子集
    vector<vector<int> > subset1;
    vector<vector<int> > subset2;
    for (unsigned i = 0; i < samplesNum; i++)
    {
        if (splitAttributeValues[i] == splitValue && tree->root.empty())
            tree->root = data[i];
        else
        {
            if (splitAttributeValues[i] < splitValue)
                subset1.push_back(data[i]);
            else
                subset2.push_back(data[i]);
        }
    }

    //子集递归调用buildKdTree函数
    tree->leftChild = new KdTree;
    tree->leftChild->parent = tree;
    tree->rightChild = new KdTree;
    tree->rightChild->parent = tree;
    buildKdTree(tree->leftChild, subset1, depth + 1);
    buildKdTree(tree->rightChild, subset2, depth + 1);
}

//在kd树tree中搜索目标点goal的最近邻
//输入：目标点；已构造的kd树
//输出：目标点的最近邻
vector<int> searchNearestNeighbor(vector<int> goal, KdTree* tree)
{
    unsigned k = tree->root.size();//计算出数据的维数
    unsigned d = 0;//维度初始化为0，即从第1维开始
    KdTree* currentTree = tree;
    vector<int> currentNearest = currentTree->root;
    while (!currentTree->isLeaf())
    {
        unsigned index = d % k;//计算当前维
        currentNearest = currentTree->root;
        if (currentTree->rightChild->isEmpty() || goal[index] < currentNearest[index])
        {
            if(!currentTree->leftChild->isEmpty())
                currentTree = currentTree->leftChild;
            else
                currentTree = currentTree->rightChild;
        }
        else
        {
            if (!currentTree->rightChild->isEmpty())
                currentTree = currentTree->rightChild;
            else
                currentTree = currentTree->leftChild;
        }
        d++;
    }
    currentNearest = currentTree->root;

    //当前最近邻与目标点的距离
    double currentDistance = measureDistance(goal, currentNearest);
    //如果当前子kd树的根结点是其父结点的左节点，则搜索其父结点的右结点所代表的区域，反之亦反
    KdTree* searchDistrict;
    if (currentTree->isLeft())
    {
        if (currentTree->parent->rightChild == NULL)
            searchDistrict = currentTree;
        else
            searchDistrict = currentTree->parent->rightChild;
    }
    else
    {
        searchDistrict = currentTree->parent->leftChild;
    }

    //如果搜索区域对应的子kd树的根结点不是整个kd树的根结点，继续回退搜索
    while (searchDistrict->parent != NULL)
    {
        //搜索区域与目标点的最近距离
        double districtDistance = abs(goal[(d + 1) % k] - searchDistrict->parent->root[(d + 1) % k]);

        //如果“搜索区域与目标点的最近距离”比“当前最近邻与目标点的距离”短，表明搜索区域内可能存在距离目标点更近的点
        if (districtDistance < currentDistance)//&& !searchDistrict->isEmpty()
        {

            double parentDistance = measureDistance(goal, searchDistrict->parent->root);
            if (parentDistance < currentDistance)
            {
                currentDistance = parentDistance;
                currentTree = searchDistrict->parent;
                currentNearest = currentTree->root;
            }
            if (!searchDistrict->isEmpty())
            {
                double rootDistance = measureDistance(goal, searchDistrict->root);
                if (rootDistance < currentDistance)
                {
                    currentDistance = rootDistance;
                    currentTree = searchDistrict;
                    currentNearest = currentTree->root;
                }
            }
            if (searchDistrict->leftChild != NULL && !searchDistrict->leftChild->isEmpty())
            {
                double leftDistance = measureDistance(goal, searchDistrict->leftChild->root);
                if (leftDistance < currentDistance)
                {
                    currentDistance = leftDistance;
                    currentTree = searchDistrict;
                    currentNearest = currentTree->root;
                }
            }
            if (searchDistrict->rightChild != NULL && !searchDistrict->rightChild->isEmpty())
            {
                int rightDistance = measureDistance(goal, searchDistrict->rightChild->root);
                if (rightDistance < currentDistance)
                {
                    currentDistance = rightDistance;
                    currentTree = searchDistrict;
                    currentNearest = currentTree->root;
                }
            }
        }

        if (searchDistrict->parent->parent != NULL)
        {
            searchDistrict = searchDistrict->parent->isLeft() ?
                searchDistrict->parent->parent->rightChild :
                searchDistrict->parent->parent->leftChild;
        }
        else
        {
            searchDistrict = searchDistrict->parent;
        }
        ++d;
    }//end while
    return currentNearest;
}

//逐层打印kd树
void printKdTree(KdTree* tree, unsigned depth)
{
    for (unsigned i = 0; i < depth; ++i)
        cout << "\t";

    for (vector<int>::size_type j = 0; j < tree->root.size(); ++j)
        cout << tree->root[j] << ",";
    cout << endl;
    if (tree->leftChild == NULL && tree->rightChild == NULL)//叶子节点
        return;
    else //非叶子节点
    {
        if (tree->leftChild != NULL)
        {
            for (unsigned i = 0; i < depth + 1; ++i)
                cout << "\t";
            cout << " left:";
            printKdTree(tree->leftChild, depth + 1);
        }

        cout << endl;
        if (tree->rightChild != NULL)
        {
            for (unsigned i = 0; i < depth + 1; ++i)
                cout << "\t";
            cout << "right:";
            printKdTree(tree->rightChild, depth + 1);
        }
        cout << endl;
    }
}




