#include <iostream>

#include "vob.h"

ZenLoad::Chunk::Chunk(const std::string &name, const std::string &className, int classVersion, Chunk *pParent, int objectID) :
    m_Name(name),
    m_ClassName(className),
    m_ClassVersion(classVersion),
    m_ObjectID(objectID),
    m_pParent(pParent),
    m_pReference(nullptr)
{
}

ZenLoad::Chunk::~Chunk()
{
    for(auto pChild : m_Vobs)
        delete pChild;
    m_Vobs.clear();
}

ZenLoad::Chunk *ZenLoad::Chunk::addVob(const std::string &name, const std::string &className, int classVersion, int objectID, Chunk *pReference)
{
    Chunk *pChild = new Chunk(name, className, classVersion, this, objectID);
    pChild->m_pReference = pReference;
    m_Vobs.push_back(pChild);
    return pChild;
}

void ZenLoad::Chunk::addAttribute(const std::string &name, const std::string &value)
{
    if(m_Attributes.find(name) != m_Attributes.end())
        throw std::runtime_error("Same attribute was set earlier");

    m_Attributes.emplace(name, value);
}

int ZenLoad::Chunk::objectID()
{
    return m_ObjectID;
}
