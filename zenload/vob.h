#pragma once

#include <vector>
#include <string>
#include <unordered_map>

namespace ZenLoad
{
    class Chunk
    {
    public:
        Chunk(const std::string &name, const std::string &className, int classVersion, Chunk *pParent = nullptr, int objectID = 0);
        ~Chunk();

        Chunk *addVob(const std::string &name, const std::string &className, int classVersion, int objectID = 0, Chunk *pReference = nullptr);
        void addAttribute(const std::string &name, const std::string &value);

        const std::string &name() const { return m_Name; }
        const std::string &className() const { return m_ClassName; }
        int objectID();
        std::unordered_map<std::string, std::string> &attributes() { return m_Attributes; }
        const Chunk *reference() const { return m_pReference; }

        int childCount() const { return m_Vobs.size(); }
        Chunk *const child(uint32_t i) { return m_Vobs[i]; }

    private:
        std::string m_Name;
        std::string m_ClassName;
        int m_ClassVersion;
        int m_ObjectID;
        Chunk *m_pParent;
        std::unordered_map<std::string, std::string> m_Attributes;
        std::vector<Chunk *> m_Vobs;
        Chunk *m_pReference;
    };
}
