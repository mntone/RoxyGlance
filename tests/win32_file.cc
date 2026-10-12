#include "pch.h"
#include "app/win32/file.h"

#include <Windows.h>

#include <filesystem>
#include <fstream>
#include <iterator>

namespace test::roxyg::win32 {

using namespace ::roxyg::win32;

class FileTest: public ::testing::Test {
protected:
  void SetUp() override {
    std::array<wchar_t, MAX_PATH + 1> temp_directory;
    DWORD const temp_directory_length = GetTempPathW(
      static_cast<DWORD>(temp_directory.size()), temp_directory.data());
    ASSERT_NE(temp_directory_length, 0U);
    ASSERT_LT(static_cast<size_t>(temp_directory_length), temp_directory.size());

    std::array<wchar_t, MAX_PATH + 1> temp_file;
    ASSERT_NE(GetTempFileNameW(temp_directory.data(), L"rgf", 0, temp_file.data()), 0U);
    filepath_ = temp_file.data();
  }

  void TearDown() override {
    std::error_code error;
    std::filesystem::remove(filepath_, error);
    filepath_ += L".tmp";
    std::filesystem::remove(filepath_, error);
  }

  std::filesystem::path filepath_;
};

TEST_F(FileTest, ReadFileReadsExistingContent) {
  std::string const expected{"file\0content", 12};
  {
    std::ofstream output{filepath_, std::ios::binary | std::ios::trunc};
    ASSERT_TRUE(output.is_open());
    output.write(expected.data(), static_cast<std::streamsize>(expected.size()));
    ASSERT_TRUE(output.good());
  }

  std::string content;
  EXPECT_EQ(ReadFile(filepath_, content), S_OK);
  EXPECT_EQ(content, expected);
}

TEST_F(FileTest, ReadFileClearsContentForEmptyFile) {
  std::string content{"previous content"};

  EXPECT_EQ(ReadFile(filepath_, content), S_OK);
  EXPECT_TRUE(content.empty());
}

TEST_F(FileTest, ReadFileReturnsErrorForMissingFile) {
  std::error_code error;
  ASSERT_TRUE(std::filesystem::remove(filepath_, error));
  ASSERT_FALSE(error);

  std::string content;
  EXPECT_NE(ReadFile(filepath_, content), S_OK);
}

TEST_F(FileTest, WriteFileReplacesExistingContentExactly) {
  {
    std::ofstream output{filepath_, std::ios::binary | std::ios::trunc};
    ASSERT_TRUE(output.is_open());
    output << "long previous content";
    ASSERT_TRUE(output.good());
  }

  std::string const expected{"new\0content", 11};
  EXPECT_EQ(WriteFileAtomic(filepath_, expected), S_OK);

  std::ifstream input{filepath_, std::ios::binary};
  ASSERT_TRUE(input.is_open());
  std::string const actual{
    std::istreambuf_iterator<char>{input}, std::istreambuf_iterator<char>{}
  };
  EXPECT_EQ(actual, expected);
}

TEST_F(FileTest, WriteFileWritesEmptyContent) {
  {
    std::ofstream output{filepath_, std::ios::binary | std::ios::trunc};
    ASSERT_TRUE(output.is_open());
    output << "previous content";
    ASSERT_TRUE(output.good());
  }

  EXPECT_EQ(WriteFileAtomic(filepath_, {}), S_OK);

  std::ifstream input{filepath_, std::ios::binary};
  ASSERT_TRUE(input.is_open());
  EXPECT_EQ(input.peek(), std::char_traits<char>::eof());
}

}  // namespace test::roxyg::win32
