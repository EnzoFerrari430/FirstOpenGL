###Q: 关于VAO和EBO解绑定的坑
unbind VAO之前 unbind了EBO，然后在glDrawElements处就会出现crash
或者bind VAO之前 bind了EBO，同样在glDrawElements处会出现crash

###A:
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
