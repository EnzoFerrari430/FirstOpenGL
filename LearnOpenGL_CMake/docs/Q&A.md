### Q: 关于VAO和EBO解绑定的坑
unbind VAO之前 unbind了EBO，然后在glDrawElements处就会出现crash
或者bind VAO之前 bind了EBO，同样在glDrawElements处会出现crash

### A:
因为 一个VAO 保存一组VertexAttributeState
和一个ElementArrayBufferObject
也就是一组顶点属性和缓冲区的bind关系和一个索引数组
bind VAO之前bind EBO
那么该VAO就没有bind索引数组了

glDrawElements自然是错的
具体理解 请看下面的一个伪实现代码
```
struct VertexAttributeState
{
    bool                bIsEnabled = false;
    int                 iSize = 4; //This is the number of elements in each attrib, 1-4.
    unsigned int        iStride = 0;
    VertexAttribType    eType = GL_FLOAT;
    bool                bIsNormalized = false;
    bool                bIsIntegral = false;
    void *              pPtrOrBufferObjectOffset = 0;
    BufferObject *      pBufferObj = 0;
};

struct VertexArrayObjectState
{
    BufferObject *pElementArrayBufferObject = NULL;
    VertexAttributeState attributes[MAX_VERTEX_ATTRIB];
}

static VertexArrayObjectState *pContextVAOState = new VertexArrayObjectState();  
static BufferObject *pCurrentArrayBuffer = NULL;

void glBindBuffer(enum target, uint buffer)  
{
    BufferObject *pBuffer = ConvNameToBufferObj(buffer);

    switch(target)
    {
    case GL_ARRAY_BUFFER:
        pCurrentArrayBuffer = pBuffer;
        break;
    case GL_ELEMENT_ARRAY_BUFFER:
        pContextVAOState->pElementArrayBufferObject = pBuffer;  
        break;

    }
}

void glEnableVertexAttribArray(uint index)
{
    pContextVAOState->attributes[index].bIsEnabled = true;
}

void glDisableVertexAttribArray(uint index)
{
    pContextVAOState->attributes[index].bIsEnabled = false;
}

void glVertexAttribPointer(uint index, int size, enum type, boolean normalized, sizei stride, const void *pointer)
{
    VertexAttributeState &currAttrib = pContextVAOState->attributes[index];

    currAttrib.iSize = size;
    currAttrib.eType = type;
    currAttrib.iStride = stride;
    currAttrib.bIsNormalized = normalized;
    currAttrib.bIsIntegral = true;
    currAttrib.pPtrOrBufferObjectOffset = pointer;
    currAttrib.pBufferObj = pCurrentArrayBuffer;
}
```
相关链接[VAO和EBO解绑定的坑？](https://www.zhihu.com/question/39082624)



### Q: OpenGL管线中的深度测试和模板测试为什么在片段着色器之后而非之前？

### A:
因为fragment shader可以discard（texkill），比如最常见的alpha test。这将导致镂空的地方经过fragment shader stage之后没有写入backbuffer，也就没有写深度、模板。fragment shader本身也可以输出深度值，所以fragment shader之后才知道深度到底是否写入、写入的值是多少。所以depth test在fragment shader以后才绝对正确、最安全。

现代硬件有自己的优化，在没有开启alpha test、fragment depth output等类似功能的时候，可以根据polygon信息，在fragment shader之前，提前做depth、stencil test，从而优化管线效率，这叫做early-z、early stencil。在DX11以前， early-z都是硬件内部的优化，其状态的开启关闭都是hardware implementation detail，一般来说每一帧开始开启（Clear事件），遇到fragment depth output或者fragment discard就自动关闭，图形程序也可以针对EarlyZ做专门的优化，比如将alpha test物体放在opaque之后绘制，尽量保证EarlyZ失效最晚。在现代图形API里，比如DX12里， 已经有方法可以去控制EarlyZ的行为。

相关链接[OpenGL管线中的深度测试和模板测试为什么在片段着色器之后而非之前？](https://www.zhihu.com/question/40576087)


### Q: 四维变换的逆变换

### A:
由于线性变换是可逆的，所以我们可以看一下位移旋转缩放的逆矩阵。
1. 位移
T的逆矩阵是-T，即向反方向移动。
2. 旋转
R的逆矩阵是R的转置矩阵，即以对角线翻转矩阵。
怎么理解呢？比如R是绕X轴旋转θ，那么逆操作就是绕X轴旋转-θ，带入-θ就会发现它变成了转置矩阵。
3. 缩放
S的逆矩阵是1/S，即把对角线上的三个元素都变成倒数，即反向缩放。

应用：
如果模型矩阵执行了不等比缩放，顶点的改变会导致法向量不再垂直于表面了。
当一个模型矩阵执行了不等比缩放我们需要使用一个为法向量专门定制的模型矩阵。
这个矩阵称之为法线矩阵(Normal Matrix)，它使用了一些线性代数的操作来移除对法向量错误缩放的影响。
法线矩阵被定义为「模型矩阵左上角3x3部分的逆矩阵的转置矩阵」

相关链接[图形学 位移，旋转，缩放矩阵变换](https://www.jianshu.com/p/ac1b34420be7)