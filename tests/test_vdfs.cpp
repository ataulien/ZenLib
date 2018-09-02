#include <iostream>
#include <vdfs/fileIndex.h>
#include <assert.h>
#include <set>
#include <gtest/gtest.h>

const char* TEST_ARCHIVE = "files/Meshes.vdf";

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

TEST(VDFS, LoadCameraMRM) {
    VDFS::FileIndex idx;
    ASSERT_TRUE(idx.loadVDF(TEST_ARCHIVE));
    idx.finalizeLoad();

    std::vector<uint8_t> data;
    ASSERT_TRUE(idx.getFileData("CAMERA.MRM", data));

    // Make sure this is the exact file we want
    ASSERT_EQ(data.size(), 8758);

    uint32_t actualChecksum = 0;
    for(auto b : data)
    {
        actualChecksum += b;
    }

    const uint32_t expectedChecksum = 410570;

    ASSERT_EQ(expectedChecksum, actualChecksum);
}

TEST(VDFS, FileNames)
{
    VDFS::FileIndex idx;
    ASSERT_TRUE(idx.loadVDF(TEST_ARCHIVE));
    idx.finalizeLoad();

    ASSERT_TRUE(idx.hasFile("CAMERA.MRM"));
    ASSERT_TRUE(idx.hasFile("Camera.MRM"));
    ASSERT_TRUE(idx.hasFile("CAMERA.Mrm"));
    ASSERT_TRUE(idx.hasFile("camera.MRM"));
    ASSERT_TRUE(idx.hasFile("cAMERA.MRM"));
    ASSERT_TRUE(idx.hasFile("camera.mRM"));
    ASSERT_TRUE(idx.hasFile("cameRa.mRM"));
    ASSERT_FALSE(idx.hasFile("this-isnt-in-here.whatever"));
}
