#include "gtest/gtest.h"
#include "../src/data.h"
namespace {

TEST(Open, txt) {
  // we launch in <root>/build/bin
  // data is in <root>/data
  sc::Model mdl{std::string("../../data/ecg.txt")};
  mdl.slidingrange = 0.f;   // no normalization 
  auto data = sc::open(mdl);

  EXPECT_EQ(data.raw.size(), data.normed.size());
  EXPECT_EQ(data.raw.size(), 12254lu);
  EXPECT_EQ(data.raw.front().time, 0.f);
  EXPECT_NEAR(data.raw.back().time, 12254lu / 500.f, 1e-3);
  EXPECT_EQ(data.raw.front().value, -444);
  EXPECT_EQ(data.raw.back().value, -5151);
}

TEST(Open, 7z) {
  // TODO
}

TEST(Data, normalize) {
  // TODO
}
}
