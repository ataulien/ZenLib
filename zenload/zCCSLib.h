#pragma once
#include "zTypes.h"
#include <map>

namespace VDFS
{
    class FileIndex;
}

namespace ZenLoad
{
    class ZenParser;
    class zCCSLib
    {
    public:

        /**
		 * @brief Loads the file from the given VDF-Archive
		 */
        zCCSLib(const std::string& fileName, const VDFS::FileIndex& fileIndex);
        zCCSLib(const std::string& file);

        /**
        * Reads this object from an internal zen
        */
        void readObjectData(ZenParser& parser);

        /**
         * @return the message of the given name
         */
        const oCMsgConversationData& getMessageByName(const std::string& name);

        /**
         * @return true if the message was found
         */
        bool messageExists(const std::string& name) const;

        const zCCSLibData& getData(){ return m_Data; }
    private:

        /**
         * Loaded data straight from the file
         */
        zCCSLibData m_Data;

        /**
         * Message indices by their names
         */
        std::map<std::string, size_t> m_MessagesByName;
    };
}
