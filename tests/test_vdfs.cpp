#include <iostream>
#include <vdfs/fileIndex.h>
#include <assert.h>
#include <set>
#include <gtest/gtest.h>
#include <stdio.h>

const char* TEST_ARCHIVE = "files/test.vdf";

bool readFile(const std::string& fileName, std::vector<uint8_t>& data)
{
    FILE* f = fopen(fileName.c_str(), "rb");

    if(!f)
        return false;

    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    fseek(f, 0, SEEK_SET);

    data.resize(size);
    fread(&data[0], size, 1, f);

    fclose(f);

    return true;
}

int main(int argc, char** argv)
{
    // Initialize the system
    VDFS::FileIndex::initVDFS(argv[0]);

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

TEST(VDFS, LoadAny) {
    VDFS::FileIndex idx;
    ASSERT_TRUE(idx.loadVDF(TEST_ARCHIVE));
    idx.finalizeLoad();

    // Just make sure there are any files loaded
    EXPECT_NE(idx.getKnownFiles().size(), 0);
}

TEST(VDFS, LoadTestTxt) {
    VDFS::FileIndex idx;
    ASSERT_TRUE(idx.loadVDF(TEST_ARCHIVE));
    idx.finalizeLoad();

    std::vector<uint8_t> data_from_archive;
    std::vector<uint8_t> data_from_file;
    ASSERT_TRUE(idx.getFileData("test.txt", data_from_archive));
    ASSERT_TRUE(readFile("files/test.txt", data_from_file));

    ASSERT_EQ(data_from_archive, data_from_file);
}

TEST(VDFS, FileNames)
{
    VDFS::FileIndex idx;
    ASSERT_TRUE(idx.loadVDF(TEST_ARCHIVE));
    idx.finalizeLoad();

    ASSERT_TRUE(idx.hasFile("TEST.TXT"));
    ASSERT_TRUE(idx.hasFile("Test.TXT"));
    ASSERT_TRUE(idx.hasFile("TEST.Txt"));
    ASSERT_TRUE(idx.hasFile("test.TXT"));
    ASSERT_TRUE(idx.hasFile("tEST.TXT"));
    ASSERT_TRUE(idx.hasFile("test.tXT"));
    ASSERT_TRUE(idx.hasFile("teSt.tXt"));
    ASSERT_FALSE(idx.hasFile("this-isnt-in-here.whatever"));
}
